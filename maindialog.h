#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include<QEvent>
#include<QCloseEvent>
#include<QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_close();
    void SIG_createRoom();
    void SIG_joinRoom();
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void setUserInfo(QString name);
private slots:
    void on_tb_create_clicked();

    void on_tb_join_clicked();

private:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
