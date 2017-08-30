#include "glider_gui.h"
#include <QDebug>
#include <QFont>

glider_gui::glider_gui(QWidget *pwgt):QWidget(pwgt)
{
    manager = new QNetworkAccessManager(pwgt);
    QPushButton *qpb_dive = new QPushButton("&dive");
    qpb_dive->setStyleSheet("QPushButton{border: 2px solid #8f8f91;color:red; background-color: \
                            qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                            stop: 0 blue, stop: 1 lightblue); font: italic bold 30px;border-radius: 20px;\
                            padding: 6px;}QPushButton:pressed {background-color: \
                            qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 lightblue, stop: 1 blue);}");
    qpb_dive->setFocusPolicy(Qt::NoFocus);
    QPushButton *qpb_emersion = new QPushButton("&emersion");
    qpb_emersion->setStyleSheet("QPushButton{border: 2px solid #8f8f91;color:red; background-color: \
                                qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                stop: 0 lightgreen, stop: 1 yellow); font: italic bold 30px;border-radius: 20px;\
                                padding: 6px;}QPushButton:pressed {background-color: \
                                qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 yellow, stop: 1 lightgreen);}");
    qpb_emersion->setFocusPolicy(Qt::NoFocus);
    QPushButton *qpb_bubble = new QPushButton("&bubble");
    qpb_bubble->setStyleSheet("QPushButton{border: 2px solid #8f8f91;color:red; background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                              stop: 0 lightgreen, stop: 1 lightblue); font: italic bold 30px;border-radius: 20px;\
                              padding: 6px;}QPushButton:pressed {background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 lightblue, stop: 1 lightgreen);}");
    qpb_bubble->setFocusPolicy(Qt::NoFocus);
    QPushButton *qpb_batpos = new QPushButton("batpos");
    qpb_batpos->setStyleSheet("QPushButton{border: 2px solid #8f8f91;color:red; background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                              stop: 0 lightblue, stop: 1 blue); font: italic bold 30px;border-radius: 20px;\
                              padding: 6px;}QPushButton:pressed {background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 blue, stop: 1 lightblue);}");
    qpb_batpos->setFocusPolicy(Qt::NoFocus);
    //QPushButton *qpb_finpos = new QPushButton("finpos");
    //QPushButton *qpb_roll = new QPushButton("roll");
    //QPushButton *qpb_course = new QPushButton("course");
    QPushButton *qpb_status = new QPushButton("&status");
    qpb_status->setStyleSheet("QPushButton{border: 2px solid #8f8f91;color:red; background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                              stop: 0 lightblue, stop: 1 lightgreen); font: italic bold 30px;border-radius: 20px;\
                              padding: 6px;}QPushButton:pressed {background-color: \
                              qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 lightgreen, stop: 1 lightblue);}");
    qpb_status->setFocusPolicy(Qt::NoFocus);

    QCheckBox *qchbox = new QCheckBox();
    qchbox->setStyleSheet("QCheckBox{border:none}QCheckBox:hover{border:none}");
    flag = qchbox->isChecked();
    qle_bubble = new QLineEdit();
    qle_bubble->resize(50, 100);
    qle_batpos = new QLineEdit();

    QLabel *ql_title = new QLabel();
    ql_title->setText(tr("<h1><i><b><p align=\"center\"><font color=red face=Times>Underwater Glider</font></p></b></i></h1>"));
    ql_title->setStyleSheet("color:green; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                            stop: 0 yellow, stop: 1 lightgreen); font: italic bold 30px;border-radius: 10px;padding: 6px;");

    QLabel *ql_bubble = new QLabel("0-100, %");
    QLabel *ql_batpos = new QLabel("0-100, %");
    QLabel *ql_chbox = new QLabel("<b><i><font color=green>LAN(ON)/WiFi(OFF)</font></b></i>");

   QHBoxLayout *qvbl_bubble = new QHBoxLayout();
   qvbl_bubble->addWidget(qle_bubble);
   qvbl_bubble->addWidget(ql_bubble);
   QHBoxLayout *qhbl_bubble = new QHBoxLayout();
   qhbl_bubble->addWidget(qpb_bubble);
   qhbl_bubble->addLayout(qvbl_bubble);

   QHBoxLayout *qvbl_batpos = new QHBoxLayout();
   qvbl_batpos->addWidget(qle_batpos);
   qvbl_batpos->addWidget(ql_batpos);
   QHBoxLayout *qhbl_batpos = new QHBoxLayout();
   qhbl_batpos->addWidget(qpb_batpos);
   qhbl_batpos->addLayout(qvbl_batpos);

   QVBoxLayout *qvbl = new QVBoxLayout();
   qvbl->addLayout(qhbl_bubble);
   qvbl->addLayout(qhbl_batpos);
   qvbl->addWidget(qpb_dive);
   //qvbl->addWidget(qpb_course);
   qvbl->addWidget(qpb_status);
   //qvbl->addWidget(qpb_finpos);
   //qvbl->addWidget(qpb_roll);
   qvbl->addWidget(qpb_emersion);

   qte = new QTextEdit();
   QVBoxLayout *qvbl_qte_save = new QVBoxLayout();
   qvbl_qte_save->addWidget(qte);
   QHBoxLayout *qhbl_qchbox = new QHBoxLayout();
   qhbl_qchbox->addWidget(qchbox);
   qhbl_qchbox->addWidget(ql_chbox);
   qhbl_qchbox->setStretch(0,1);
   qhbl_qchbox->setStretch(1, 10);
   qvbl_qte_save->addLayout(qhbl_qchbox);

   QHBoxLayout *qhbl = new QHBoxLayout();
   qhbl->addLayout(qvbl);
   qhbl->addLayout(qvbl_qte_save);
   qhbl->setStretch(0, 1);
   qhbl->setStretch(1, 10);

   QVBoxLayout *qvbl_ = new QVBoxLayout();
   qvbl_->addWidget(ql_title);
   qvbl_->addLayout(qhbl);

   setLayout(qvbl_);
   qte->setReadOnly(true);

   connect(qpb_dive, SIGNAL(clicked()), SLOT(slotDIVE()));
   connect(qpb_emersion, SIGNAL(clicked()), SLOT(slotDIVE()));
   connect(qpb_bubble, SIGNAL(clicked()), SLOT(slotDIVE()));
   connect(qpb_batpos, SIGNAL(clicked()), SLOT(slotDIVE()));
   connect(qpb_status, SIGNAL(clicked()), SLOT(slotDIVE()));
   connect(qchbox, SIGNAL(clicked()), SLOT(isWIFI()));
   connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getResponse(QNetworkReply*)));
   isWIFI();
}

void glider_gui::slotDIVE()
{
    QString str = ((QPushButton*)sender())->text();
    if (str=="&dive")
    {
        QNetworkRequest request(QUrl("http://"+ip+"/arduino/"+str+"/"));
        manager->get(request);
        return;
    }
    else if(str=="&emersion")
    {
        QNetworkRequest request(QUrl("http://"+ip+"/arduino/"+str+"/"));
        manager->get(request);
        return;
    }
    else if(str=="&bubble")
    {
        QString percentage = qle_bubble->text();
        QNetworkRequest request(QUrl("http://"+ip+"/arduino/"+str+"/"+percentage));
        manager->get(request);
        return;
    }
    else if(str=="batpos")
    {
        QString percentage = qle_batpos->text();
        QNetworkRequest request(QUrl("http://"+ip+"/arduino/"+str+"/"+percentage));
        manager->get(request);
        return;
    }
    else if(str=="finpos")
    {
        return;
    }
    else if(str=="roll")
    {
        return;
    }
    else if(str=="course")
    {
        return;
    }
    else if(str=="&status")
    {
        //qDebug()<<"hello";
        QNetworkRequest request(QUrl("http://"+ip+"/arduino/"+str+"/"));
        manager->get(request);
        return;
    }
}
void glider_gui::getResponse(QNetworkReply *reply)
{
   QTime time = QTime::currentTime();
   qte->append(time.toString()+" "+ QString::fromUtf8(reply->readAll()));
}
void glider_gui::isWIFI()
{
    QTime time = QTime::currentTime();
    if(!flag)
    {
        ip = "192.168.240.1";
        qte->append(time.toString()+" "+QString::fromUtf8("Используется соединение по WiFi: ")+ip);
    }
    else
    {
        ip = "192.168.0.1";
        qte->append(time.toString()+" "+QString::fromUtf8("Используется соединение по LAN: ")+ip);
    }
    flag = !flag;
}
