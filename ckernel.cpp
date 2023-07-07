#include "ckernel.h"
#include<QCoreApplication>
#include<QFileInfo>
#include<QSettings> //打开可以看例子
#include"md5.h"

#define NETPACKMAP(a)  m_netPackMap[ a - _DEF_PACK_BASE ]
void CKernel::setNetPackMap()
{
    memset(m_netPackMap,0, sizeof(m_netPackMap));

    NETPACKMAP(_DEF_PACK_REGISTER_RS)     = &CKernel::slot_dealRegisterRs;
    NETPACKMAP(_DEF_PACK_LOGIN_RS)        = &CKernel::slot_dealLoginRs;
    NETPACKMAP(_DEF_PACK_CREATE_ROOM_RS)  = &CKernel::slot_dealCreateRoomRs;
    NETPACKMAP(_DEF_PACK_JOIN_ROOM_RS)    = &CKernel::slot_dealJoinRoomRs;
    NETPACKMAP(_DEF_PACK_ROOM_MEMBER_RQ)  = &CKernel::slot_dealRoomMemberRq;
    NETPACKMAP(_DEF_PACK_LEAVE_ROOM_RQ)   = &CKernel::slot_dealLeaveRoomRq;
    NETPACKMAP(_DEF_PACK_AUDIO_FRAME_RQ)  = &CKernel::slot_dealAudioFrameRq;
    NETPACKMAP(_DEF_PACK_VIDEO_FRAME_RQ)  = &CKernel::slot_dealVideoFrameRq;
}

CKernel::CKernel(QObject *parent) : QObject(parent)
{
    qDebug()<<"MainThread:"<<QThread::currentThread();
    setNetPackMap();
    //////////////////主界面
    m_pMainDialog = new MainDialog;

    connect(m_pMainDialog, SIGNAL(SIG_close()),
            this, SLOT(slot_destory()));
    connect(m_pMainDialog, SIGNAL(SIG_createRoom()),
            this, SLOT(slot_createRoom()));
    connect(m_pMainDialog, SIGNAL(SIG_joinRoom()),
            this, SLOT(slot_joinRoom()));

    //////////////////网络
    m_pNet = new TcpClientMediator;
    connect(m_pNet, SIGNAL(SIG_ReadyData(uint,char*,int)),
            this, SLOT(slot_DealData(uint,char*,int)));

    setNetIni();
    m_pNet->OpenNet(m_ip.toStdString().c_str(), m_port.toInt());

    //音频和视频
    for(int i = 0; i< 2; i++){
        m_pAVClient[i] = new TcpClientMediator;
        m_pAVClient[i]->OpenNet(m_ip.toStdString().c_str(), m_port.toInt());
        connect(m_pAVClient[i], SIGNAL(SIG_ReadyData(uint,char*,int)),
                this, SLOT(slot_DealData(uint,char*,int)));
    }

    ////////////////登录界面
    m_pLoginDialog = new LoginDialog;
    connect(m_pLoginDialog, SIGNAL(SIG_loginCommit(QString,QString)),
            this, SLOT(slot_loginCommit(QString,QString)) );
    connect(m_pLoginDialog, SIGNAL(SIG_registerCommit(QString,QString,QString)),
            this, SLOT(slot_registerCommit(QString,QString,QString)));
    m_pLoginDialog->show();

    ////////////////会议界面
    m_pRoomDialog = new RoomDialog;

    //退出会议
    connect(m_pRoomDialog, SIGNAL(SIG_quitRoom()),
            this, SLOT(slot_quitRoom()));
    //音频控制
    connect(m_pRoomDialog, SIGNAL(SIG_audioPause()),
            this, SLOT(slot_audioPause()));
    connect(m_pRoomDialog, SIGNAL(SIG_audioStart()),
            this, SLOT( slot_audioStart()));
    //视频控制
    connect( m_pRoomDialog, SIGNAL(SIG_videoStart()),
             this, SLOT(slot_videoStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_videoPause()),
            this, SLOT(slot_videoPause()));
    //共享屏幕控制
    connect(m_pRoomDialog, SIGNAL(SIG_screenStart()),
            this, SLOT(slot_screenStart()));
    connect(m_pRoomDialog, SIGNAL(SIG_screenPause()),
            this, SLOT(slot_screenPause()));

    //////////////音频
    m_pAudioRead = new Audio_Read; //音频指针
    connect( m_pAudioRead, SIGNAL(SIG_audioFrame(QByteArray)),
             this, SLOT(slot_audioFrame(QByteArray)));
    ///////////////////////
    /// 视频
    m_pVideoRead = new VideoRead;
    connect( m_pVideoRead, SIGNAL(SIG_sendVideoFrame(QImage)),
             this, SLOT(slot_sendVideoFrame(QImage)));

    m_pScreenRead = new ScreenRead;
    connect(m_pScreenRead, SIGNAL(SIG_getScreenFrame(QImage)),
            this, SLOT(slot_sendVideoFrame(QImage))); //暂时不区分 统一处理发送
    //萌拍设置滤镜
    connect(m_pRoomDialog, SIGNAL(SIG_setMoji(int)),
            m_pVideoRead, SLOT(slot_setMoji(int)) );
    ///多线程指针
    m_pSendVideoWork = QSharedPointer<SendVideoWorker>(new SendVideoWorker);
    connect( this, SIGNAL(SIG_SendVideo(char*,int)),
             m_pSendVideoWork.data(), SLOT(slot_sendVideo(char*,int)) );
}

void CKernel::setNetIni()
{
    //默认值
    m_ip = _DEF_SERVERIP;
    m_port = _DEF_PORT;
    //获取exe同级目录
    QString exePath = QCoreApplication::applicationDirPath() + "/" + "config.ini";
    //判断文件是否存在
    QFileInfo info(exePath);
    QSettings setting(exePath, QSettings::IniFormat ); // 有打开 没有自动创建
    //打开组
    setting.beginGroup( "net" );
    if( !info.exists() ){//不存在 创建文件 写入文件
        //设置键值对
        setting.setValue( "ip", m_ip );
        setting.setValue( "port", m_port );
    }else{ //存在 读出数据赋值
        //取键值对
        QVariant strIp = setting.value( "ip", "");  //参数二：默认值（取数有问题） 空  返回值：QT中的泛型
        QVariant strPort = setting.value( "port", "");  //QT中的泛型
        if( !strIp.toString().isEmpty() )   m_ip = strIp.toString();
        if( !strPort.toString().isEmpty() ) m_port = strPort.toString();

    }
    //关闭组
    setting.endGroup();
    qDebug()<<"ip:"<<m_ip<<"   port:"<<m_port;
}


//生成MD5 static函数 当前文件可用
//生成目标格式 password_1234
#define MD5_KEY "1234"
static std::string getMD5(QString val)
{
    QString str = QString("%1_%2").arg(val).arg(MD5_KEY);
    MD5 md( str.toStdString() );
    qDebug() << str << "md5:" << md.toString().c_str()<< endl;
    return md.toString();
}

#include<QTextCodec>

// QString -> char* gb2312
void Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8)
{
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    QByteArray ba = gb2312code->fromUnicode( utf8 );// Unicode -> 转码对象的字符集

    strcpy_s ( gbbuf , nlen , ba.data() );
}

// char* gb2312 --> QString utf8
QString GB2312ToUtf8( char* gbbuf )
{
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    return gb2312code->toUnicode( gbbuf );// 转码对象的字符集 -> Unicode
}



void CKernel::slot_dealRegisterRs(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug() <<__func__;
    STRU_REGISTER_RS *rs = (STRU_REGISTER_RS*)buf;

    switch (rs->result) {
    case tel_is_exist:
        QMessageBox::about(m_pLoginDialog, "提示", "用户已存在,注册失败");
        break;
    case name_is_exist:
        QMessageBox::about(m_pLoginDialog, "提示", "昵称重复,注册失败");
        break;
    case register_success:
        QMessageBox::about(m_pLoginDialog, "提示", "注册成功");
        break;
    default:
        break;
    }
}

void CKernel::slot_dealLoginRs(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    //拆包 判断登录结果
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;

    switch (rs->result) {
    case tel_not_exist:
        QMessageBox::about(m_pLoginDialog, "提示", "用户不存在,登录失败");
        break;
    case password_error:
        QMessageBox::about(m_pLoginDialog, "提示", "密码错误,登录失败");
        break;
    case login_success: //登陆成功

        //后台
        m_name = QString::fromStdString(rs->name);
        m_id = rs->userid;
        m_pMainDialog->setUserInfo(m_name);
        //界面切换
        m_pLoginDialog->hide();
        m_pMainDialog->show();
        printf("login success\n");

        //注册 音频和视频的id
        STRU_VIDEO_REGISTE_RQ rq_video;
        rq_video.userid = m_id;
        STRU_AUDIO_REGISTE_RQ rq_audio;
        rq_audio.userid = m_id;

        m_pAVClient[audio_client]->SendData(0, (char*)&rq_audio, sizeof(rq_audio));
        m_pAVClient[video_client]->SendData(0, (char*)&rq_video, sizeof(rq_video));
        break;
  //  default:
  //      break;
    }

}

void CKernel::slot_dealCreateRoomRs(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    //拆包 显示会议界面
    STRU_CRAETE_ROOM_RS* rs = (STRU_CRAETE_ROOM_RS*)buf;
    m_roomid = rs->roomid;
    m_pRoomDialog->showNormal();

    //把房间号显示到标题栏
    m_pRoomDialog->slot_setRoomid(QString::number(rs->roomid));
    //添加自己的用户信息
    userWidget* user = new userWidget;
    //绑定切换大图
    connect(user, SIGNAL(SIG_itemClicked(int,QString)),
            m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));

    user->slot_setInfo(m_id, m_name);
    m_pRoomDialog->slot_addUserShow(user);
    //  qDebug()<<rs->roomid;

    //初始化音频视频 不选中
    m_pRoomDialog->slot_setAudioCheck(false);
    m_pRoomDialog->slot_setVideoCheck(false);

}

void CKernel::slot_dealJoinRoomRs(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    STRU_JOIN_ROOM_RS *rs = (STRU_JOIN_ROOM_RS*)buf;
    //判断是否成功
    if(rs->result == join_room_fail){
        //失败 提示
        QMessageBox::about(m_pMainDialog, "提示","加入会议室失败,房间不存在");
    }
    if(rs->result == join_room_success){
        //成功，设置房间号 显示房间
        m_pRoomDialog->slot_setRoomid(QString::number(rs->roomid));
        m_roomid = rs->roomid;
        m_pRoomDialog->showNormal();
        //添加自己的用户信息 服务器先给自己发
    }
    //音视频初始化
    m_pRoomDialog->slot_setAudioCheck(false);
    m_pRoomDialog->slot_setVideoCheck(false);
}

void CKernel::slot_dealRoomMemberRq(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    STRU_ROOM_MEMBER_RQ* rq = (STRU_ROOM_MEMBER_RQ*)buf;
    //创建控件
    userWidget* user = new userWidget;
    user->slot_setInfo(rq->userid, QString::fromStdString(rq->name));
    m_pRoomDialog->slot_addUserShow(user);
    //绑定切换大图
    connect(user, SIGNAL(SIG_itemClicked(int,QString)),
            m_pRoomDialog, SLOT(slot_setBigImgId(int,QString)));


    //音频的内容：创建并加入加到map
    //播放：为每个人创建一个对象
    Audio_Write* aw = nullptr;
    if( m_mapRoomerIDToAudioWrite.count(rq->userid) == 0){
        aw = new Audio_Write;
        m_mapRoomerIDToAudioWrite[rq->userid] = aw;
    }


    //视频：已经创建了控件


}

void CKernel::slot_dealLeaveRoomRq(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    //拆包
    STRU_LEAVE_ROOM_RQ*rq = (STRU_LEAVE_ROOM_RQ*)buf;
    //从ui去掉
    if( rq->roomid == m_roomid){
        m_pRoomDialog->slot_removeUserShow(rq->userid);
    }
    //关闭对应音频
    if(m_mapRoomerIDToAudioWrite.count(rq->userid)>0){
        Audio_Write* aw = m_mapRoomerIDToAudioWrite[rq->userid];
        m_mapRoomerIDToAudioWrite.erase(rq->userid);
        delete aw;
    }
}

//音频帧处理
void CKernel::slot_dealAudioFrameRq(unsigned int lSendIp, char *buf, int nlen)
{
    qDebug()<<__func__;
    //    PackType type;
    //    int userid;
    //    int roomid;
    //    int min;
    //    int sec;
    //    int msec;
    //    QByteArray audioFrame;  //可变长度　使用柔性数组
    //反序列化

    char* tmp = buf;
    int userid;
    int roomid;

    tmp += sizeof(int);  //跳过类型
    userid = *(int*)tmp ;    tmp += sizeof(int);
    roomid = *(int*)tmp;    tmp += sizeof(int);
    //跳过时间
    tmp += sizeof(int);
    tmp += sizeof(int);
    tmp += sizeof(int);

    int nbufLen = nlen- 6*sizeof(int);
    QByteArray ba(tmp, nbufLen);

    if(m_roomid == roomid){ //用户和房间都存在 播放
        if(m_mapRoomerIDToAudioWrite.count(userid) > 0){
            Audio_Write* aw = m_mapRoomerIDToAudioWrite[userid];
            aw->slot_net_rx(ba);
        }
    }


}

void CKernel::slot_dealVideoFrameRq(unsigned int lSendIp, char *buf, int nlen)
{
    //    PackType type;
    //    int userid;
    //    int roomid;
    //    int min;
    //    int sec;
    //    int msec;
    //    QByteArray videoFrame;
    //拆包取得数据
    char* tmp= buf;
    tmp += sizeof(int);
    int uid = *(int*)tmp; tmp += sizeof(int);
    int roomid = *(int*)tmp; tmp += sizeof(int);
    tmp += sizeof(int);tmp += sizeof(int);tmp += sizeof(int); //偏移时间

    int datalen = nlen - 6*sizeof(int);
    QByteArray bt(tmp, datalen);
    QImage img;
    img.loadFromData(bt);  //根据地址和长度加载图片
    //显示到对应用户
    if(roomid == m_roomid) m_pRoomDialog->slot_refreshVideo(uid, img);

}

void CKernel::slot_loginCommit(QString tel, QString password)
{
    qDebug()<<__func__;
    STRU_LOGIN_RQ rq;
    std::string strTel = tel.toStdString();
    strcpy(rq.tel, strTel.c_str());
    strcpy(rq.password, getMD5(password).c_str());
    SendData( (char*)&rq, sizeof(rq));
}

void CKernel::slot_registerCommit(QString tel, QString password, QString name)
{
    qDebug() <<__func__;
    STRU_REGISTER_RQ rq;
    //名字兼容中文
    std::string strName = name.toStdString();
    strcpy ( rq.name, strName.c_str() );

    strcpy( rq.tel, tel.toStdString().c_str() );

    //MD5
    strcpy(rq.password, getMD5(password).c_str());
    SendData( (char*)&rq, sizeof(rq) );
}

void CKernel::slot_createRoom()
{
    qDebug()<<__func__;
    //判断是否在房间里  m_roomid；
    if(m_roomid != 0){
        QMessageBox::about(m_pMainDialog, "错误","已在一个会议中")    ;
        return;
    }
    //发送创建请求
    STRU_CRAETE_ROOM_RQ rq;
    rq.userid = m_id;

    SendData( (char*)&rq, sizeof(rq) );
}

#include<QInputDialog>
#include"QRegExp"
#include "ckernel.h"
void CKernel::slot_joinRoom()
{
    qDebug()<<__func__;
    //判断是否在房间里
    if(m_roomid != 0){
        QMessageBox::about(m_pMainDialog, "错误","已在一个会议中")    ;
        return;
    }
    //弹出窗口 输入房间号
    QString strRoom = QInputDialog::getText( m_pMainDialog, "加入房间", "输入房间号");
    //判断房间号
    QRegExp exp("^[0-9]\{1,8\}$");  //匹配8位0-9之间的
    if( !exp.exactMatch( strRoom ))
    {
        QMessageBox::about(m_pMainDialog, "错误","房间号输入非法")    ;
        return;
    }
    STRU_JOIN_ROOM_RQ rq;
    rq.userid = m_id;
    rq.roomid = strRoom.toInt();
    qDebug()<<"user:"<<m_id<<"room"<<rq.roomid;
    //发送命令
    SendData( (char*)&rq, sizeof(rq) );
}

void CKernel::slot_quitRoom()
{
    qDebug()<<__func__;
    STRU_LEAVE_ROOM_RQ rq;
    rq.userid = m_id;
    strcpy(rq.name, m_name.toStdString().c_str());
    rq.roomid = m_roomid;
    SendData((char*)&rq, sizeof(rq));
    //退出会议
    m_roomid = 0;
    //关闭音视频
    m_pAudioRead->pause();
    m_pVideoRead->slot_closeVideo();
    m_pScreenRead->slot_closeVideo();

    m_pRoomDialog->slot_setAudioCheck( false );
    m_pRoomDialog->slot_setVideoCheck( false );
    m_pRoomDialog->slot_setScreenCheck( false );
    //回收所有人的音频
    for( auto ite = m_mapRoomerIDToAudioWrite.begin(); ite != m_mapRoomerIDToAudioWrite.end(); ){
        Audio_Write* aw = ite->second;
        ite = m_mapRoomerIDToAudioWrite.erase(ite);
        delete aw;
    }
    //回收资源
    m_pRoomDialog->slot_clearUserShow();
}

void CKernel::slot_audioPause()
{
    qDebug()<<__func__;
    m_pAudioRead->pause();
}

void CKernel::slot_audioStart()
{
    qDebug()<<__func__;
    m_pAudioRead->start();
}

void CKernel::slot_audioFrame(QByteArray ba) //对象 可以跨线程
{
    qDebug()<<__func__;
    int nPackSize = 6*sizeof(int) + ba.size();
    char* buf = new char[nPackSize];
    char* tmp = buf;
    //序列化
    PackType type = _DEF_PACK_AUDIO_FRAME_RQ;
    int userid = m_id;
    int roomid = m_roomid;
    QTime tm = QTime::currentTime();
    int min = tm.minute();
    int sec = tm.second();
    int msec = tm.msec();
    //  QByteArray audioFrame;  //可变长度　使用柔性数组
    *(int*)tmp =  type;     tmp += sizeof(int);
    *(int*)tmp =  userid;    tmp += sizeof(int);
    *(int*)tmp =  roomid;    tmp += sizeof(int);
    *(int*)tmp =  min;    tmp += sizeof(int);
    *(int*)tmp =  sec;    tmp += sizeof(int);
    *(int*)tmp =  msec;    tmp += sizeof(int);
    memcpy(tmp, ba.data(), ba.size());

    //todo:多线程：把音频和视频的发送变成一个信号，放在另一个线程执行
    //    SendData(buf, nPackSize);
    m_pAVClient[audio_client]->SendData(0,buf, nPackSize);
    delete[] buf;

}

void CKernel::slot_sendVideoFrame(QImage img)
{
    qDebug()<<__func__;
    //显示
    slot_refreshVideo( m_id, img ); //根据id刷新
    //压缩
    //压缩图片从 RGB24 格式压缩到 JPEG 格式, 发送出去
    QByteArray ba;
    QBuffer qbuf(&ba); // QBuffer 与 QByteArray 字节数组联立联系
    img.save( &qbuf , "JPEG" , 50 ); //将图片的数据写入 ba
    //使用 ba 对象, 可以获取图片对应的缓冲区
    // 可以使用 ba.data() , ba.size()将缓冲区发送出去

    //写视频帧发送
    //    PackType type;
    //    int userid;
    //    int roomid;
    //    int min;
    //    int sec;
    //    int msec;
    //    QByteArray videoFrame;
    int nPackSize = 6*sizeof(int) + ba.size();
    char* buf = new char[nPackSize];
    char* tmp = buf;

    *(int*)tmp = _DEF_PACK_VIDEO_FRAME_RQ;
    tmp += sizeof(int);
    *(int*)tmp = m_id;
    tmp += sizeof(int);
    *(int*)tmp = m_roomid;
    tmp += sizeof(int);
    QTime tm = QTime::currentTime();
    *(int*)tmp = tm.minute();
    tmp += sizeof(int);
    *(int*)tmp = tm.second();
    tmp += sizeof(int);
    *(int*)tmp = tm.msec();
    tmp += sizeof(int);

    memcpy( tmp, ba.data(), ba.size());
    Q_EMIT SIG_SendVideo(buf, nPackSize);
    //  SendData(buf, nPackSize);
    //  delete[] buf;
}

//视频开启和关闭
void CKernel::slot_videoPause()
{
    qDebug()<<__func__;
    m_pVideoRead->slot_closeVideo();
}

void CKernel::slot_videoStart()
{
    qDebug()<<__func__;
    m_pVideoRead->slot_openVideo();
}

void CKernel::slot_refreshVideo(int id, QImage &img)
{
    m_pRoomDialog->slot_refreshVideo(id, img);
}

void CKernel::slot_screenStart()
{
    m_pScreenRead->slot_openVideo();
}

void CKernel::slot_screenPause()
{
    m_pScreenRead->slot_closeVideo();
}

//多线程发送视频
void CKernel::slot_SendVideo(char *buf, int nlen)
{
    //判断是否卡顿超时，丢包
    char* tmp = buf;
    tmp += sizeof(int)*4; //跳过 直接取时间
    int min = *(int*)tmp;  tmp += sizeof(int);
    int sec = *(int*)tmp;  tmp += sizeof(int);
    int msec = *(int*)tmp;  tmp += sizeof(int);

    QTime curtm = QTime::currentTime();
    QTime tm(curtm.hour(), min, sec, msec);

    if(tm.msecsTo(curtm) > 300){  //延迟超过300直接舍弃
        qDebug()<<"time beyound 300,drop data";
        delete[] buf;
        return;
    }

    //m_pNet->SendData(0, buf, nlen);
    m_pAVClient[video_client]->SendData(0, buf, nlen);
    delete[] buf;
}




void CKernel::slot_destory()
{
    qDebug()<<__func__;

    if(m_pMainDialog){
        m_pMainDialog->hide(); //注意不要close不然死循环
        delete m_pMainDialog;
        m_pMainDialog = nullptr;
    }
    if(m_pLoginDialog){
        m_pLoginDialog->hide(); //注意不要close不然死循环
        delete m_pLoginDialog;
        m_pLoginDialog = nullptr;
    }

    if(m_pAudioRead){
        m_pAudioRead->pause();
        delete m_pAudioRead;
        m_pAudioRead = nullptr;
    }
    if(m_pRoomDialog){
        m_pRoomDialog->hide(); //注意不要close不然死循环
        delete m_pRoomDialog;
        m_pRoomDialog = nullptr;
    }

    if(m_pNet){
        m_pNet->CloseNet();
        delete m_pNet;
        m_pNet = nullptr;
    }
    exit(0);
}

void CKernel::slot_DealData(uint lIp, char *buf, int nLen)
{
    //拆包
    int type = *(int*)buf;
    //判断协议头范围
    if( type >= _DEF_PACK_BASE && type < _DEF_PACK_BASE + _DEF_PACK_COUNT){
        //取协议头 获取函数指针 调用函数
        PFUN  pfun = NETPACKMAP(type);
        if(pfun)
        {
            (this->*pfun)(lIp, buf, nLen); //类成员函数指针使用this
        }
    }
    //回收
    delete[] buf;
    buf = nullptr;
}

void CKernel::SendData(char *buf, int nLen)
{
    //阻塞发送：如果服务器没有及时取走缓冲区数据
    //导致滑动窗口变小，send阻塞 影响响应
    m_pNet->SendData(0, buf, nLen);


}
