#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include"maindialog.h"
#include"TcpClientMediator.h"
#include<QDebug>
#include"packdef.h"
#include"logindialog.h"
#include<QMessageBox>
#include"roomdialog.h"
#include"audio_read.h"
#include"audio_write.h"
#include<QTime>
#include"videoread.h"
#include"screenread.h"
#include<QBuffer>  //缓冲区
#include"threadworker.h"

//协议映射表
class CKernel;
typedef void (CKernel::*/*类成员函数指针*/PFUN)(uint,char*,int);

class SendVideoWorker;

//单例
class CKernel : public QObject
{
    Q_OBJECT
private:
    explicit CKernel(QObject *parent = nullptr);
    CKernel(CKernel & Kernel){}
    ~CKernel(){}
    void setNetPackMap();
    void setNetIni();
public:
    static CKernel* GetInstance(){
        static CKernel kernel; //局限性 无法回收
        return &kernel;
    }
private slots:
    //////////////////网络处理////////////////////
    void slot_dealRegisterRs(unsigned int lSendIp, char* buf, int nlen);
    void slot_dealLoginRs(unsigned int lSendIp, char* buf, int nlen);
    void slot_dealCreateRoomRs(unsigned int lSendIp, char* buf, int nlen);  //处理创建房间回复
    void slot_dealJoinRoomRs(unsigned int lSendIp, char* buf, int nlen); //处理加入房间回复
    void slot_dealRoomMemberRq(unsigned int lSendIp, char* buf, int nlen);  //处理房间成员信息
    void slot_dealLeaveRoomRq(unsigned int lSendIp, char* buf, int nlen);
    void slot_dealAudioFrameRq(unsigned int lSendIp, char* buf, int nlen); //处理音频数据
    void slot_dealVideoFrameRq(unsigned int lSendIp, char* buf, int nlen); //处理视频数据
    /////////////////普通函数//////////////////////
    void slot_loginCommit(QString tel, QString password);
    void slot_registerCommit(QString tel, QString password, QString name);
    void slot_createRoom();
    void slot_joinRoom();
    void slot_quitRoom();
    //音频处理
    void slot_audioPause();
    void slot_audioStart();
    void slot_audioFrame(QByteArray ba); //发送音频帧 QByteArray ba) //对象 可以跨线程
    //视频处理
    void slot_sendVideoFrame(QImage img);
    void slot_videoPause();
    void slot_videoStart();
    void slot_refreshVideo(int id, QImage & img); //根据id刷新图片
    //共享屏幕
    void slot_screenStart();
    void slot_screenPause();
    //多线程处理视频
    void slot_SendVideo(char* buf, int nlen);


    void slot_destory();
    void slot_DealData(uint lIp,char* buf,int nLen);
    void SendData(char* buf,int nLen);
private:
    QString m_ip;
    QString m_port;
    INetMediator* m_pNet;
    enum client_type{audio_client = 0, video_client};
    INetMediator * m_pAVClient[2];

    PFUN m_netPackMap[_DEF_PACK_COUNT];

    MainDialog* m_pMainDialog;
    LoginDialog* m_pLoginDialog;
    RoomDialog* m_pRoomDialog;

    QString m_name;
    int m_id;
    int m_roomid;

    /////音频  1个采集 多个播放每一个房间成员1:1映射
    Audio_Read * m_pAudioRead;
    std::map<int, Audio_Write*> m_mapRoomerIDToAudioWrite;


    /////视频
    VideoRead* m_pVideoRead;
    ScreenRead* m_pScreenRead;

    //多线程指针
    QSharedPointer<SendVideoWorker> m_pSendVideoWork;
signals:
    //多线程发送视频
    void SIG_SendVideo(char* buf, int nlen);
friend class SendVideoWorker;
};


class SendVideoWorker : public ThreadWorker
{
    Q_OBJECT
public:
    //不用定义指针 kernel是单例
public slots:
    void slot_sendVideo(char* buf, int nlen){
        CKernel::GetInstance()->slot_SendVideo(buf, nlen);
    }
};

#endif // CKERNEL_H
