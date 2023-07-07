#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include<QPaintEvent>
#include<QPainter>
#include<QMouseEvent>
#include<QTimer>
#include<QTime>

namespace Ui {
class userWidget;
}

class userWidget : public QWidget
{
    Q_OBJECT
signals:
    void SIG_itemClicked(int id, QString name);
public:
    explicit userWidget(QWidget *parent = nullptr);
    ~userWidget();


public slots:
    void slot_setInfo(int id, QString name);
    void paintEvent(QPaintEvent *event);
    void slot_setImage(QImage& img);
    void mousePressEvent(QMouseEvent *event);
    void slot_checkTimer();  //定时检测，接收界面是否超时
private:

    Ui::userWidget *ui;
    int m_id;
    QString m_userName;
    QImage m_img;
    QImage m_defaultImg;

    friend class RoomDialog;
    QTimer m_timer; //静音检测的定时器
    QTime m_lastTime;

};

#endif // USERWIDGET_H
