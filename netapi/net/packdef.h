#pragma once
#include<memory.h>

#define _DEF_BUFFER         (4096)
#define _DEF_CONTENT_SIZE	(1024)
#define _MAX_SIZE           (40)
#define _MAX_PATH_SIZE      (260)
#define _DEF_PORT           8000
#define _DEF_SERVERIP       "192.168.159.133"

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)

//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ	(_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS	(_DEF_PACK_BASE + 3 )


//返回的结果
//注册请求的结果
#define tel_is_exist		(0)
#define name_is_exist		(1)
#define register_success	(2)
//登录请求的结果
#define tel_not_exist		(0)
#define password_error		(1)
#define login_success		(2)


typedef int PackType;

//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
	{
		memset( tel  , 0, sizeof(tel));
		memset( name  , 0, sizeof(name));
		memset( password , 0, sizeof(password) );
	}
	//需要手机号码 , 密码, 昵称
	PackType type;
	char tel[_MAX_SIZE];
	char name[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_REGISTER_RQ;

typedef struct STRU_REGISTER_RS
{
	//回复结果
	STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success)
	{
	}
	PackType type;
	int result;

}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
	//登录需要: 手机号 密码 
	STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
	{
		memset( tel , 0, sizeof(tel) );
		memset( password , 0, sizeof(password) );
	}
	PackType type;
	char tel[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_LOGIN_RQ;

typedef struct STRU_LOGIN_RS
{
	// 需要 结果 , 用户的id
	STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
	{
        memset( name , 0, sizeof(name) );
	}
	PackType type;
	int result;
	int userid;
    char name[_MAX_SIZE];
}STRU_LOGIN_RS;


/////////////////创建房间///////////////////
#define _DEF_PACK_CREATE_ROOM_RQ	(_DEF_PACK_BASE + 4 )
#define _DEF_PACK_CREATE_ROOM_RS	(_DEF_PACK_BASE + 5 )


#define create_room_success		(1)
#define create_room_fail		(0)

typedef struct STRU_CRAETE_ROOM_RQ
{
    STRU_CRAETE_ROOM_RQ(): type(_DEF_PACK_CREATE_ROOM_RQ) ,userid(0)
    {
    }
    PackType type;
    int userid;
}STRU_CRAETE_ROOM_RQ;

typedef struct STRU_CRAETE_ROOM_RS
{
    STRU_CRAETE_ROOM_RS(): type(_DEF_PACK_CREATE_ROOM_RS) ,result(0),roomid(0)
    {
    }
    PackType type;
    int result;
    int roomid;
}STRU_CRAETE_ROOM_RS;

/////////////////加入房间///////////////////
#define _DEF_PACK_JOIN_ROOM_RQ	(_DEF_PACK_BASE + 6 )
#define _DEF_PACK_JOIN_ROOM_RS	(_DEF_PACK_BASE + 7 )

#define join_room_success   (1)
#define join_room_fail		(0)

typedef struct STRU_JOIN_ROOM_RQ
{
    STRU_JOIN_ROOM_RQ(): type(_DEF_PACK_JOIN_ROOM_RQ) ,userid(0), roomid(0)
    {
    }
    PackType type;
    int userid;
    int roomid;
}STRU_JOIN_ROOM_RQ;

typedef struct STRU_JOIN_ROOM_RS
{
    STRU_JOIN_ROOM_RS(): type(_DEF_PACK_JOIN_ROOM_RS) ,result(0), roomid(0)
    {
    }
    PackType type;
    int result;
    int roomid;
}STRU_JOIN_ROOM_RS;

//房间用户信息//////////////////
#define _DEF_PACK_ROOM_MEMBER_RQ	(_DEF_PACK_BASE + 8 )

typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ(): type(_DEF_PACK_ROOM_MEMBER_RQ) ,userid(0)
    {
        memset( name , 0, sizeof(name) );
    }
    PackType type;
    int userid;
    char name[_MAX_SIZE];
}STRU_ROOM_MEMBER_RQ;


/////////////////退出房间///////////////////
#define _DEF_PACK_LEAVE_ROOM_RQ	(_DEF_PACK_BASE + 9 )

typedef struct STRU_LEAVE_ROOM_RQ
{
    STRU_LEAVE_ROOM_RQ(): type(_DEF_PACK_LEAVE_ROOM_RQ) ,userid(0),roomid(0)
    {
        memset( name , 0, sizeof(name) );
    }
    PackType type;
    int userid;
    int roomid;
    char name[_MAX_SIZE];
}STRU_LEAVE_ROOM_RQ;


/////////////////音频///////////////////
#define _DEF_PACK_AUDIO_FRAME_RQ	(_DEF_PACK_BASE + 10 )
//    PackType type;
//    int userid;
//    int roomid;
//    int min;
//    int sec;
//    int msec;
//    QByteArray audioFrame;  //可变长度　使用柔性数组


////////////////视频/////////////////////
#define _DEF_PACK_VIDEO_FRAME_RQ	(_DEF_PACK_BASE + 11 )
//    PackType type;
//    int userid;
//    int roomid;
//    int min;
//    int sec;
//    int msec;
//    QByteArray videoFrame;

///////////////////注册音频和视频
#define _DEF_PACK_VIDEO_REGISTER_RQ	(_DEF_PACK_BASE + 12 )
typedef struct STRU_VIDEO_REGISTE_RQ
{
    STRU_VIDEO_REGISTE_RQ(): type(_DEF_PACK_VIDEO_REGISTER_RQ) ,userid(0)
    {

    }
    PackType type;
    int userid;
}STRU_VIDEO_REGISTE_RQ;

#define _DEF_PACK_AUDIO_REGISTER_RQ	(_DEF_PACK_BASE + 13 )
typedef struct STRU_AUDIO_REGISTE_RQ
{
    STRU_AUDIO_REGISTE_RQ(): type(_DEF_PACK_AUDIO_REGISTER_RQ) ,userid(0)
    {

    }
    PackType type;
    int userid;
}STRU_AUDIO_REGISTE_RQ;
