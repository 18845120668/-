#include "roomdialog.h"
#include "ui_roomdialog.h"

RoomDialog::RoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDialog)
{
    ui->setupUi(this);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(5);

    //设置一个垂直布局的画布
    ui->wdg_list->setLayout( m_mainLayout );

//    //测试
//    for(int i = 0; i<6; i++){
//        userWidget* user = new userWidget;
//        user->slot_setInfo( i+1, QString("测试%1").arg(i+1) );
//        slot_addUserShow( user );
//    }
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::slot_setRoomid(QString roomid)
{
    QString title = QString("会议号:%1").arg(roomid);
    setWindowTitle(title);
    ui->lb_title->setText(title);
}

void RoomDialog::slot_addUserShow(userWidget *user)
{
    //向层里添加
    m_mainLayout->addWidget( user );
    //管理当前用户
    m_mapUserIDtoUserWidget[user->m_id] = user;
}

void RoomDialog::slot_removeUserShow(userWidget *user)
{
    user->hide();
    m_mainLayout->removeWidget(user);
}

void RoomDialog::slot_removeUserShow(int userid)
{
    //根据id删除一个用户的ui
    if( m_mapUserIDtoUserWidget.count(userid) > 0){
        userWidget* user = m_mapUserIDtoUserWidget[userid];
        user->hide();
        m_mainLayout->removeWidget(user);

    }
}

void RoomDialog::slot_clearUserShow()
{
    for( auto ite = m_mapUserIDtoUserWidget.begin(); ite!= m_mapUserIDtoUserWidget.end(); ite++){
        slot_removeUserShow( ite->second );
    }
}

void RoomDialog::slot_setAudioCheck(bool check)
{
    //
    ui->cb_audio->setChecked(check);
}

void RoomDialog::slot_setVideoCheck(bool check)
{
    ui->cb_video->setChecked(check);
}

void RoomDialog::slot_setScreenCheck(bool check)
{
    ui->cb_desk->setCheckable(check);
}

void RoomDialog::slot_refreshVideo(int id, QImage &img)
{
    if(ui->wdg_userShowBig->m_id == id) //预览图和要刷新图的id一致，也要显示
    {
        ui->wdg_userShowBig->slot_setImage(img);
    }

    if(m_mapUserIDtoUserWidget.count(id) == 0 ) return;
    userWidget* user = m_mapUserIDtoUserWidget[id];
    user->slot_setImage(img);
}

void RoomDialog::slot_setBigImgId(int id, QString name)
{
    ui->wdg_userShowBig->slot_setInfo(id, name);

}

void RoomDialog::on_pb_close_clicked()
{
    //退出房间
    ui->cb_audio->setChecked(false);
    this->close();

}


void RoomDialog::on_pb_quit_clicked()
{
    ui->cb_audio->setChecked(false);
    this->close();
}

void RoomDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::Yes == QMessageBox::question(this, "退出提示","是否确认退出会议?")){

        Q_EMIT SIG_quitRoom();
        event->accept();
        return;
    }
    event->ignore();
}


void RoomDialog::on_cb_audio_clicked()
{
    //检查状态 开启或关闭音频
    if(ui->cb_audio->isChecked() ){
   //     ui->cb_audio->setChecked(false);

        Q_EMIT SIG_audioStart();

    }else{
      //  ui->cb_audio->setChecked(true);
        Q_EMIT SIG_audioPause();

    }
}


void RoomDialog::on_cb_video_clicked()
{
    //开启关闭视频
    if(ui->cb_video->isChecked()){
        //停止屏幕共享，开始视频
        ui->cb_desk->setCheckable(false);
        Q_EMIT SIG_screenPause();
        Q_EMIT SIG_videoStart();
    }else{
        Q_EMIT SIG_videoPause();
    }

}

//共享桌面
void RoomDialog::on_cb_desk_clicked()
{
    if( ui->cb_desk->isChecked()){
        ui->cb_video->setCheckable(false);
        Q_EMIT SIG_videoPause();
        Q_EMIT SIG_screenStart();
    }else{
        Q_EMIT SIG_screenPause();
    }
}


void RoomDialog::on_cb_moji_face_currentIndexChanged(int index)
{
    //发送信号
    Q_EMIT SIG_setMoji(index);
}

