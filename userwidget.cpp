#include "userwidget.h"
#include "ui_userwidget.h"

userWidget::userWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userWidget)
{
    ui->setupUi(this);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT( slot_checkTimer()) );
    m_lastTime = QTime::currentTime();

    m_defaultImg.load(":/resource/images/groupChat 2.png");

    m_timer.start(1000);  //每秒检测一次
}

userWidget::~userWidget()
{
    delete ui;
}

void userWidget::slot_setInfo(int id, QString name)
{
    m_id = id;
    m_userName = name;

    //设置名字
    QString txt = QString("用户：%1").arg(m_userName);
    ui->lb_name->setText(txt);
}

void userWidget::paintEvent(QPaintEvent *event)
{
    //重绘
    QPainter painer(this);
    //黑色背景
    painer.setBrush( Qt::black );  //设置黑色画刷
    painer.drawRect(-1,0, ui->lb_name->width()+1, this->height() );

    //画视频帧
    if( m_img.size().height()<= 0 ) return;
    //等比例缩放
    m_img = m_img.scaled(this->width(), this->height()-ui->lb_name->height(),Qt::KeepAspectRatio);
    QPixmap pix = QPixmap::fromImage(m_img);
    int x = this->width() - pix.width();
    x = x/2;
    int y = this->height() - pix.height() - ui->lb_name->height();
    y = y/2 + ui->lb_name->height();
    painer.drawPixmap(QPoint(x,y),pix);
    //0, ui->lb_name->height(),t
    painer.end();
}

void userWidget::slot_setImage(QImage &img)
{
    //设置图片
    m_img = img;
    this->update(); //触发重绘
    //每次获得画面更新时间
    m_lastTime = QTime::currentTime();
}

void userWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    Q_EMIT SIG_itemClicked( m_id, m_userName );
}

void userWidget::slot_checkTimer()
{
    //定时检测 接收界面是否超时
    if( m_lastTime.secsTo( QTime::currentTime() ) > 5){
        //设为默认画面
        slot_setImage(m_defaultImg);
    }
}
