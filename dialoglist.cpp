#include "dialoglist.h"
#include "ui_dialoglist.h"
#include "widget.h"

#include <QToolButton>
#include <QMessageBox>

DialogList::DialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    setWindowTitle("MySelfQQ 2017");
    setWindowIcon(QPixmap(":/images/qq.png"));
    QList<QString> nameList;
    nameList << "斧头帮主" << "忆梦如澜" <<"北京出版人"<<"Cherry"<<"淡然"
             <<"娇娇girl"<<"落水无痕"<<"青墨暖暖"<<"无语";
    QStringList iconNameList; //图标资源列表
    iconNameList << "ftbz"<< "ymrl" <<"qq" <<"Cherry"<< "dr"
                 <<"jj"<<"lswh"<<"qmnn"<<"wy";


    QVector <QToolButton *> VToolBtn;

    for(int i = 0; i < 9; i++){
        QToolButton *btn = new QToolButton;
        btn->setText(nameList[i]);
        QString str = QString(":/images/%1.png").arg(iconNameList.at(i));
        btn->setIcon(QPixmap(str));
        btn->setIconSize(QPixmap(str).size());
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->vLayout->addWidget(btn);
        VToolBtn.push_back(btn);
        isShow.push_back(false);
    }
    for(int i = 0; i < VToolBtn.size(); i++){
        connect(VToolBtn[i], &QToolButton::clicked, [=](){

            if(isShow[i]){
                QString str = QString("%1窗口已经被打开了").arg(VToolBtn[i]->text());
                QMessageBox::warning(this, "警告", str);
                return;
            }
            isShow[i] = true;

            Widget *widget = new Widget(0, VToolBtn[i]->text());
            widget->setWindowTitle(VToolBtn[i]->text());
            widget->setWindowIcon(VToolBtn[i]->icon());
            widget->show();
            connect(widget, &Widget::closeWidget, [=](){
                isShow[i] = false;
            });
        });
    }
}

DialogList::~DialogList()
{
    delete ui;
}
