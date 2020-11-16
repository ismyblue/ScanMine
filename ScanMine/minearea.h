#ifndef MINEAREA_H
#define MINEAREA_H

#include <QFrame>
#include "grid.h"
#include <QGridLayout>

class MineArea : public QFrame
{
    Q_OBJECT
public:
    MineArea(QWidget *parent = nullptr, int row = 9, int col = 9, int mineCount = 10);
    // 行数列数
    int row, col;
    // 总雷数
    int mineCount;
    // 插旗子数
    int flagCount;
    // 完成扫描数
    int scanedCount;


private:
    // Grid*指针数组  二维
    Grid ***grids;

    // 网格
    QGridLayout *mainLayout;

    // 初始化格子
    void randomGridsData();



signals:
    // 判断结果信号
    void judgeIsWin();
    // 结束扫描信号
    void finishedScan(bool result);
    // 更新插旗数信号
    void updateFlagCount(int flagCount);

public slots:
    // 打开一个格子的周围空白格子
    void slotOpenEmptyGrid(int rowIndex, int colIndex, bool isData);
    // 更新插旗数
    void slotUpdateFlagCount(Grid::STATUS newStatus, Grid::STATUS oldStatus);
    // 更新扫描完成数
    void slotUpdateScanedCount(Grid::STATUS newStatus, Grid::STATUS oldStatus);
    // 判断是否胜利
    bool slotJudgeIsWin();
    // 打开所有格子,公布结果
    void slotOpenAllGrid();
    // 重新开始游戏
    void slotRestartGame(int newRow, int newCol, int newMineCount);



};

#endif // MINEAREA_H
