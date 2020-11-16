#ifndef GRID_H
#define GRID_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
// 按下状态、插旗状态、打开状态（数字）、关闭状态（待扫描）、爆炸状态（点击后爆炸）、公布地雷状态、扫描成功状态


class Grid : public QLabel
{
    Q_OBJECT
public:
    enum STATUS{
        PRESS, // 按下状态
        FLAG, // 插旗状态
        OPEN, // 打开状态（数字）
        CLOSE, // 关闭状态（待扫描）
        BOOM, // 爆炸状态（点击后爆炸）
        MINE, // 公布地雷状态
        ERRORMINE // 扫描错误状态
    };

    Grid(QWidget *parent = nullptr, int rowIndex = -1, int colIndex = -1,int data = 0, bool isMine = false);
    // 获取状态
    STATUS getStatus();
    // 获取数据
    int getData();
    // 设置数据
    void setData(int data);
    // 设置是否为地雷
    void setIsMine(bool isMine);
    // 获取是否为地雷
    bool getIsMine();



private:
    // 是否为地雷
    bool isMine;
    Grid::STATUS status;
    int data;
    int rowIndex, colIndex;

signals:
    // 修改状态信号
    void changeStatus(Grid::STATUS newStatus, Grid::STATUS oldStatus);
    // 展示状态信号
    void displayStatus();
    // 格子打开信号
    void gridOpen(int rowIndex, int colIndex, bool isData);

public slots:
    // 修改状态
    void slotChangeStatus(Grid::STATUS status);
    // 展示状态
    void slotDisplayStatus();

protected:
    // 鼠标点击事件处理器
    void mousePressEvent(QMouseEvent *event);
    // 鼠标释放事件处理器
    void mouseReleaseEvent(QMouseEvent *event);
    // 鼠标移出事件处理器
    void leaveEvent(QEvent *event);



};

#endif // GRID_H
