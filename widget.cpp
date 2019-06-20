#include "widget.h"
#include "ui_widget.h"

#include <QDateTime>
#include <QDebug>

Widget::Widget(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);

    uName = name;

    this->port = 9999;

    //绑定端口号  绑定模式  共享地址，断线重连
    udpSocket->bind(this->port,QUdpSocket::ShareAddress |QUdpSocket::ReuseAddressHint);
    //发送新用户进入
    sndMsg(UsrEnter);

    //点击发送按钮发送消息
    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        sndMsg(Msg);
    });
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::ReceiveMessage);
    //点击退出按钮 实现关闭窗口
    connect(ui->exitBtn,&QPushButton::clicked,[=](){
        this->close();
    });





}

void Widget::ReceiveMessage()
{
    //拿到数据报文
    //获取长度
    qint64 size = udpSocket->pendingDatagramSize();
    QByteArray array = QByteArray(size,0);
    udpSocket->readDatagram(array.data(),size);

    //解析数据
    //第一段  类型  第二段 用户名 第三段 内容
    QDataStream stream (&array,QIODevice::ReadOnly);

    int msgType; //读取到类型
    QString usrName;
    QString msg;

    //获取当前时间
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    stream >>msgType;
    switch (msgType) {
    case Msg: //普通聊天
        stream >> usrName >> msg;
        //最加聊天记录
        ui->msgBrowser->setTextColor(Qt::blue);
        ui->msgBrowser->append("[" + usrName  + "]" + time);
        ui->msgBrowser->append(msg);

        break;
    case UsrEnter:
        stream >> usrName;
        usrEnter(usrName);

        break;
    case UsrLeft:
        stream >> usrName;
        usrLeft(usrName,time);
        break;
    default:
        break;
    }

}//接受UDP消息

void Widget::sndMsg(MsgType type)
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << type << getUsr() ; //第一段内容 添加到流中 第二段 用户名
    switch(type){
        case Msg:
            if(ui->msgTxtEdit->toPlainText() == "") //判断如果用户没有输入内容，不发任何消息
            {
                QMessageBox::warning(this,"警告","发送内容不能为空");
                return ;
            }
            //第三段数据，具体说的话
            stream <<getMsg();
            break;
        case UsrEnter:
            break;
        case UsrLeft:
            break;
        default:
            break;
    }
    //书写报文  广播发送
    udpSocket->writeDatagram(array,QHostAddress::Broadcast,port);
}

QString Widget::getUsr()
{
    return this->uName;
}

QString Widget::getMsg()
{
    QString str = ui->msgTxtEdit->toHtml();

    //清空输入框
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();

    return str;
}//获取聊天信息

void Widget::usrEnter(QString username)
{
    //更新右侧TableWidget


    bool isEmpty1 = ui->usrTblWidget->findItems(username,Qt::MatchExactly).isEmpty();
    if(isEmpty1)
    {
        QTableWidgetItem * usr = new QTableWidgetItem(username);

        //插入行
        ui->usrTblWidget->insertRow(0);
        ui->usrTblWidget->setItem(0,0,usr);

        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append( QString("%1 上线了").arg(username));
        //在线人数更新
        ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTblWidget->rowCount()));

        //把自身信息广播出去
        sndMsg(UsrEnter);
    }

}//处理新用户加入


void Widget::usrLeft(QString usrname,QString time)
{
    //更新右侧tableWidget
    bool isEmpty = ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(!isEmpty)
    {
        int row = ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).first()->row();
        ui->usrTblWidget->removeRow(row);

        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QString("%1 于 %2 离开").arg(usrname).arg(time));
        //在线人数更新
        ui->usrNumLbl->setText(QString("在线用户：%1人").arg(ui->usrTblWidget->rowCount()));

    }

}

void Widget::closeEvent(QCloseEvent *e)
{
    emit this->closeWidget();
    sndMsg(UsrLeft);
    //断开套接字
    udpSocket->close();
    udpSocket->destroyed();

    QWidget::closeEvent(e);
}

Widget::~Widget()
{
    delete ui;
}
