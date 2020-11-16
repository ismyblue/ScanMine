#include "scanmine.h"
#include "ui_scanmine.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QInputDialog>

ScanMine::ScanMine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ScanMine)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/ui/mine.png"));
    mineArea = new MineArea(ui->frame_playPanel, 10, 10, 10);

    mainLayout = new QGridLayout(ui->frame_playPanel);
    mainLayout->addWidget(mineArea, 0, 0);
    mainLayout->setMargin(1);
    mainLayout->setSpacing(0);

    this->resize(this->minimumSize());
    //    this->move

    // 完成扫描->公布结果
    connect(mineArea, SIGNAL(finishedScan(bool)), mineArea, SLOT(slotOpenAllGrid()));
    // 完成扫描->游戏结束，弹窗
    connect(mineArea, SIGNAL(finishedScan(bool)), this, SLOT(slotShowGameResult(bool)));

    // 计时清零
    playSeconds = 0;
    ui->lcdNumber_time->setProperty("intValue", QVariant(playSeconds));
    // 插旗数清零
    ui->lcdNumber_flagCount->setProperty("intValue", QVariant(mineArea->mineCount));

    timer = new QTimer();
    timer->start(1000);
    // 更新时间
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));

    // 更新插旗数
    connect(mineArea, SIGNAL(updateFlagCount(int)), this, SLOT(flag_count(int)));


}

ScanMine::~ScanMine()
{
    delete ui;
}

// 游戏结束，弹窗
void ScanMine::slotShowGameResult(bool result)
{
    timer->stop();
    if(result)
        QMessageBox::information(this, "Game Over", QString("你赢了! 用时：%1 秒").arg(playSeconds));
    else
        QMessageBox::warning(this, "Game Over", QString("你输了! 用时：%1 秒").arg(playSeconds));
}

void ScanMine::slotRestartGame(int row, int col, int mineCount)
{
    mineArea->slotRestartGame(row, col, mineCount);
    this->resize(this->minimumSize());
    // 计时器清零
    playSeconds = 0;
    ui->lcdNumber_time->setProperty("intValue", QVariant(playSeconds));
    // 剩余地雷数
    ui->lcdNumber_flagCount->setProperty("intValue", QVariant(mineArea->mineCount));
    // 开启计时器
    timer->stop();
    timer->start(1000);
}


void ScanMine::timer_timeout()
{
    playSeconds += 1;
    // 设置时间
    ui->lcdNumber_time->setProperty("intValue", QVariant(playSeconds));
}

void ScanMine::flag_count(int flagCount)
{
    ui->lcdNumber_flagCount->setProperty("intValue", QVariant(mineArea->mineCount - flagCount));
}


// 开始按钮
void ScanMine::on_pushButton_start_clicked()
{
    slotRestartGame(mineArea->row, mineArea->col, mineArea->mineCount);
}

// 初级
void ScanMine::on_actionone_triggered(bool checked)
{
    slotRestartGame(10, 10, 10);

}

// 中级
void ScanMine::on_actiontwo_triggered(bool checked)
{
    slotRestartGame(15, 15, 40);
}

// 高级
void ScanMine::on_actionthree_triggered(bool checked)
{
    slotRestartGame(25, 25, 100);
}

void ScanMine::on_actiondiy_triggered(bool checked)
{
    bool ok;
    int row = QInputDialog::getInt(this, tr("自定义游戏"), tr("请输入行:"), 0);
    int col = QInputDialog::getInt(this, tr("自定义游戏"), tr("请输入列:"), 0);
    int mineCount = QInputDialog::getInt(this, tr("自定义游戏"), tr("请输入地雷数:"), 0);
    if(0 <= mineCount && mineCount <= (row * col) && (row * col) > 0 )
        slotRestartGame(row, col, mineCount);
    else
        QMessageBox::warning(this, QString("Error"), QString("行列数必须大于0，并且地雷数不能少于1，也不能大于总格子数！"));

}
