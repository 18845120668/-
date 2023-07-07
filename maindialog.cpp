#include "maindialog.h"
#include "ui_maindialog.h"
#include<QMessageBox>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    setWindowTitle("会议");
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::setUserInfo(QString name)
{
    ui->lb_name->setText(name);
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    //关闭事件
    if( QMessageBox::Yes == QMessageBox::question(this, "提示","是否退出程序") )
    {
        Q_EMIT SIG_close();
        event->accept();
    }else{
        event->ignore();
    }
}

//创建会议
void MainDialog::on_tb_create_clicked()
{
    Q_EMIT SIG_createRoom();
}


void MainDialog::on_tb_join_clicked()
{
    Q_EMIT SIG_joinRoom();
}

