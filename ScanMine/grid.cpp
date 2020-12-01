#include "grid.h"
#include <QDebug>
#include <Qt>
#include <QFont>

Grid::Grid(QWidget *parent, int rowIndex, int colIndex, int data, bool isMine)
    : QLabel(parent),rowIndex(rowIndex),colIndex(colIndex),data(data),isMine(isMine)
{    
    this->setMargin(0);
    this->setMouseTracking(true);
    this->setScaledContents(true);


    this->status = Grid::CLOSE;
    connect(this, &Grid::changeStatus, this, &Grid::slotChangeStatus);
    connect(this, &Grid::displayStatus, this, &Grid::slotDisplayStatus);
//    this->setText(QString("shit"));
    emit displayStatus();
}

// 获取状态
Grid::STATUS Grid::getStatus()
{
    return this->status;
}

// 获取数据
int Grid::getData()
{
    return this->data;
}

// 设置数据
void Grid::setData(int data)
{
    this->data = data;
}

void Grid::setIsMine(bool isMine)
{
    this->isMine = isMine;
}


// 返回是否为地雷
bool Grid::getIsMine()
{
    if(isMine)
        return true;
    else
        return false;
}

// 改变状态的槽
void Grid::slotChangeStatus(Grid::STATUS status)
{
    this->status = status;
    // 发送格子打开信号
    if(this->status == Grid::OPEN && this->data >= 0)
    {
        bool isData = (this->data>0?true:false);
        emit gridOpen(rowIndex, colIndex, isData);
    }
    // 发送展示状态信号
    emit displayStatus();
}

// 展示状态的槽
void Grid::slotDisplayStatus()
{
    this->clear();
    // 按压状态
    if(status == Grid::PRESS)
    {
        this->setFrameStyle(QFrame::NoFrame);
    }
    // 插旗状态
    else if(status == Grid::FLAG)
    {
        this->setFrameStyle(QFrame::Panel | QFrame::Raised);
        QPixmap flag = QPixmap(":/ui/flag.png");
        flag.scaled(this->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->setPixmap(flag);
    }
    // 打开状态
    else if(status == Grid::OPEN)
    {        
        this->setFrameStyle(QFrame::NoFrame);
        if(isMine){
            emit changeStatus(Grid::BOOM, this->status);
        }
        else
        {            
            this->setAlignment(Qt::AlignCenter);
            QFont font("宋体", this->width() / 2, 1);
            this->setFont(font);
            if(data > 0){
                this->setFrameStyle(QFrame::StyledPanel);
                this->setText(QString::number(data));
            }
        }
    }
    // 关闭状态
    else if(status == Grid::CLOSE)
    {
        this->setFrameStyle(QFrame::Panel | QFrame::Raised);
        this->clear();
    }
    // 爆炸状态（点击后爆炸）
    else if(status == Grid::BOOM)
    {
        this->setFrameStyle(QFrame::NoFrame);
        QPixmap boom = QPixmap(":/ui/boom.png");
        boom.scaled(this->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->setPixmap(boom);
    }
    // 公布地雷状态
    else if(status == Grid::MINE)
    {
        this->setFrameStyle(QFrame::NoFrame);
        QPixmap mine = QPixmap(":/ui/mine.png");
        mine.scaled(this->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->setPixmap(mine);
    }
    // 扫描错误状态
    else if(status == Grid::ERRORMINE)
    {
        this->setFrameStyle(QFrame::NoFrame);
        QPixmap scanedMine = QPixmap(":/ui/errormine.png");
        scanedMine.scaled(this->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->setPixmap(scanedMine);
    }
}


// 鼠标点击事件处理器
void Grid::mousePressEvent(QMouseEvent *event)
{
    // 在关闭的格子上点击鼠标左键
    if( status == Grid::CLOSE && event->button() == Qt::LeftButton)
    {
        emit changeStatus(Grid::PRESS, this->status);
    }
    // 在关闭的格子上点击鼠标右键
    else if(status == Grid::CLOSE && event->button() == Qt::RightButton)
    {        
        emit changeStatus(Grid::FLAG, this->status);
    }
    // 在插旗子的格子上点击鼠标右键
    else if(status == Grid::FLAG && event->button() == Qt::RightButton)
    {
        emit changeStatus(Grid::CLOSE, this->status);
    }

}

// 鼠标释放事件处理器
void Grid::mouseReleaseEvent(QMouseEvent *event)
{    
    // 在关闭的格子上弹起鼠标左键
    if((status == Grid::PRESS || status == Grid::CLOSE) && event->button() == Qt::LeftButton
            && (0 <= event->pos().x() && event->pos().x() <= this->width() && 0 <= event->pos().y() && event->pos().y() <= this->height()))
    {
        emit changeStatus(Grid::OPEN, this->status);
    }

}

// 鼠标移出事件处理器
void Grid::leaveEvent(QEvent *event)
{
    if(status == Grid::PRESS)
    {
        emit changeStatus(Grid::CLOSE, this->status);
    }
}
