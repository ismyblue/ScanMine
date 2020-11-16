#ifndef SCANMINE_H
#define SCANMINE_H

#include <QMainWindow>
#include "minearea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ScanMine; }
QT_END_NAMESPACE

class ScanMine : public QMainWindow
{
    Q_OBJECT

public:
    ScanMine(QWidget *parent = nullptr);
    ~ScanMine();


private:
    Ui::ScanMine *ui;
    MineArea *mineArea;
    QGridLayout *mainLayout;
    QTimer *timer;
    int playSeconds;

public slots:
    // 游戏结束槽
    void slotShowGameResult(bool result);
    // 重启游戏槽
    void slotRestartGame(int row, int col, int mineCount);

private slots:
    // 开始按钮
    void on_pushButton_start_clicked();
    // 计时器响应
    void timer_timeout();
    // 插旗计数
    void flag_count(int flagCount);
    // 初级
    void on_actionone_triggered(bool checked);
    // 中级
    void on_actiontwo_triggered(bool checked);
    // 高级
    void on_actionthree_triggered(bool checked);
    // 自定义
    void on_actiondiy_triggered(bool checked);
};
#endif // SCANMINE_H
