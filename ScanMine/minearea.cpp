#include "minearea.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QQueue>

MineArea::MineArea(QWidget *parent, int row, int col, int mineCount) : QFrame(parent), row(row), col(col), mineCount(mineCount)
{
    // 设置边框以及大小
    this->setFrameShape(QFrame::StyledPanel);
    this->setFrameShadow(QFrame::Sunken);

    this->setMaximumSize(col*20, row*20);
    this->setMinimumSize(col*20, row*20);
    this->resize(this->minimumSize());
    // 布局
    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);


    // 设置格子
    randomGridsData();

    // 插旗数为0
    this->flagCount = 0;
    // 扫描完成数
    this->scanedCount = 0;

    // 判断结果信号，连接判断结果槽
    connect(this, &MineArea::judgeIsWin, this, &MineArea::slotJudgeIsWin);
}

// 随机化数据
void MineArea::randomGridsData()
{
    // new 指针二维数组
    grids = new Grid**[row];
    for(int i = 0;i < row;i++)
        grids[i] = new Grid*[col];

    // 创建内存，然后设置数据为0
    for(int i=0;i < row;i++)
    {
        for(int j = 0;j < col;j++)
        {
            grids[i][j] = new Grid(this, i, j, 0, false);
            mainLayout->addWidget(grids[i][j], i, j);
            // 格子打开信号->打开周围空白格子
            connect(grids[i][j], &Grid::gridOpen, this, &MineArea::slotOpenEmptyGrid);
            // 格子状态改变信号->统计插旗数
            connect(grids[i][j], &Grid::changeStatus, this, &MineArea::slotUpdateFlagCount);
            // 格子状态改变信号->统计完成扫描数
            connect(grids[i][j], &Grid::changeStatus, this, &MineArea::slotUpdateScanedCount);

        }
    }

    int r,c;
    // 设置mineCount个地雷
    for(int k = 0;k < mineCount;k++)
    {
        // 生成随机坐标点
        r = QRandomGenerator::global()->bounded(0, row);
        c = QRandomGenerator::global()->bounded(0, col);

        // -1为有雷
        if(grids[r][c]->getData() != -1)
        {
            grids[r][c]->setData(-1);
            grids[r][c]->setIsMine(true);
        }
        else
        {
            k--;
            continue;
        }

        // 地雷周围数据加一
        for(int i = -1;i <= 1;i++)
        {
            for(int j = -1;j <= 1;j++)
            {
                int newR = r + i;
                int newC = c + j;
                // 边界之内，并且不是地雷，数据加一
                if(0 <= newR && newR < row && 0 <= newC && newC < col && grids[newR][newC]->getData() != -1)
                {
                    grids[newR][newC]->setData(grids[newR][newC]->getData() + 1);
                }
            }
        }
    }
}


// 重新开始游戏
void MineArea::slotRestartGame(int newRow, int newCol, int newMineCount)
{
    // delete 动态创建的内存
    for(int i = 0;i < row;i++)
    {
        for(int j = 0;j < col;j++)
        {
            mainLayout->removeWidget(grids[i][j]);
            grids[i][j]->setParent(nullptr);
            delete grids[i][j];
        }
        delete [] grids[i];
    }
    delete[] grids;



    //     更新棋盘大小
    row = newRow;
    col = newCol;
    mineCount = newMineCount;

    //     设置大小
    this->setMaximumSize(col*20, row*20);
    this->setMinimumSize(col*20, row*20);
    this->resize(col*20, row*20);

    // new 新内存
    randomGridsData();

    // 初始化插旗数和扫描数
    flagCount = 0;
    scanedCount = 0;



}


// 打开周围空格子的槽
int step[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
void MineArea::slotOpenEmptyGrid(int rowIndex, int colIndex, bool isData)
{
    // 如果是数字格子打开，那么不打开周围的格子
    if(isData)
        return ;
    // 打开空白格子四周的格子
    int newRow;
    int newCol;
    for(int i = 0;i < 4;i++)
    {
        newRow = rowIndex + step[i][0];
        newCol = colIndex + step[i][1];
        // 对边界之内的格子进行打开操作
        if(0 <= newRow && newRow < row && 0 <= newCol && newCol < col)
        {
            // 如果四周是空白格子或者数字格子，那么打开
            if(grids[newRow][newCol]->getData() >= 0 && grids[newRow][newCol]->getStatus() == Grid::CLOSE)
            {
                emit grids[newRow][newCol]->changeStatus(Grid::OPEN, grids[newRow][newCol]->getStatus());
            }
        }

    }

}

// 更新插旗数量的槽
void MineArea::slotUpdateFlagCount(Grid::STATUS newStatus, Grid::STATUS oldStatus)
{
    // 插旗动作，插旗数量加一
    if(oldStatus == Grid::CLOSE && newStatus == Grid::FLAG)
        this->flagCount++;
    // 取消插旗动作，插旗数量减一
    else if(oldStatus == Grid::FLAG && newStatus == Grid::CLOSE)
        this->flagCount--;

    emit updateFlagCount(flagCount);
}

// 更新扫描完成数量 的槽
void MineArea::slotUpdateScanedCount(Grid::STATUS newStatus, Grid::STATUS oldStatus)
{
    // 插旗动作，扫描完成的数量加一
    if(oldStatus == Grid::CLOSE && newStatus == Grid::FLAG)
        this->scanedCount++;
    // 取消插旗动作，扫描完成数量减一
    else if(oldStatus == Grid::FLAG && newStatus == Grid::CLOSE)
        this->scanedCount--;
    else if(newStatus == Grid::OPEN)// && newStatus == Grid::BOOM)
        this->scanedCount++;


    // 所有格子扫描完成并且插旗数量正确那么发射结束扫描信号 或者 产生爆炸时发射结束扫描信号
    if((oldStatus == Grid::OPEN && newStatus == Grid::BOOM) || (scanedCount == row*col && flagCount == mineCount))
    {
        emit judgeIsWin();
    }

}

// 判断是否成功的槽
bool MineArea::slotJudgeIsWin()
{
    for(int i = 0;i < row;i++)
    {
        for(int j =0;j < col;j++)
        {
            // 如果格子处于爆炸状态，游戏输
            if(grids[i][j]->getStatus() == Grid::BOOM)
            {
                emit finishedScan(false);
                return false;
            }
        }
    }
    emit finishedScan(true);
    return true;
}


// 打开所有格子，公布结果
void MineArea::slotOpenAllGrid()
{
    for(int i = 0;i < row;i++)
    {
        for(int j =0;j < col;j++)
        {
            // 如果是关闭状态，并且是雷,  MINE
            if(grids[i][j]->getStatus() == Grid::CLOSE && grids[i][j]->getIsMine()){
                grids[i][j]->slotChangeStatus(Grid::MINE);
            }
            // 如果是关闭状态，并且是数据,  OPEN
            else if(grids[i][j]->getStatus() == Grid::CLOSE)
            {
                grids[i][j]->slotChangeStatus(Grid::OPEN);
            }
            // 如果是插旗状态，并且是雷, FLAG
            else if(grids[i][j]->getStatus() == Grid::FLAG && grids[i][j]->getIsMine())
            {
                grids[i][j]->slotChangeStatus(Grid::FLAG);
            }
            // 如果是插旗状态，并且是数据, ERRORMINE
            else if(grids[i][j]->getStatus() == Grid::FLAG && !grids[i][j]->getIsMine())
            {
                grids[i][j]->slotChangeStatus(Grid::ERRORMINE);
            }

        }
    }
}

