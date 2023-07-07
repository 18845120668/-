#ifndef ROOMDIALOG_H
#define ROOMDIALOG_H

#include <QDialog>
//#include"usershow.h"
#include"userwidget.h"
#include<QVBoxLayout>
#include<QCloseEvent>
#include<QMessageBox>

namespace Ui {
class RoomDialog;
}

class RoomDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_quitRoom();
    void SIG_audioPause();
    void SIG_audioStart();

    //视频
    void SIG_videoPause();
    void SIG_videoStart();
    //共享屏幕
    void SIG_screenStart();
    void SIG_screenPause();
    //萌拍
    void SIG_setMoji(int index);
public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();
public slots:
    void slot_setRoomid(QString roomid);

    void slot_addUserShow(userWidget* userData);
    void slot_removeUserShow(userWidget* user);
    void slot_removeUserShow(int userid);
    void slot_clearUserShow();
    //设置勾选状态
    void slot_setAudioCheck(bool check);
    void slot_setVideoCheck(bool check);
    void slot_setScreenCheck(bool check );

    void slot_refreshVideo(int id, QImage& img);  //根据id刷新用户视频帧

    void slot_setBigImgId(int id, QString name);
private slots:
    void on_pb_close_clicked();

    void on_pb_quit_clicked();

    void closeEvent(QCloseEvent* event);

    void on_cb_audio_clicked();

    void on_cb_video_clicked();

    void on_cb_desk_clicked();



    void on_cb_moji_face_currentIndexChanged(int index);

private:
    Ui::RoomDialog *ui;
    QVBoxLayout* m_mainLayout;

    //会议用户和对应控件的管理
    std::map<int, userWidget*> m_mapUserIDtoUserWidget;
};

#endif // ROOMDIALOG_H
