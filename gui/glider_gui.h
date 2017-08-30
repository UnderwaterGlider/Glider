#ifndef GLIDER_GUI_H
#define GLIDER_GUI_H

#include <QApplication>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextBlock>
#include <QLabel>
#include <QTextEdit>
#include <QTime>
#include <QNetworkAccessManager>
#include <QCheckBox>

class glider_gui: public QWidget
{
    Q_OBJECT
private:
    bool flag;
    QString ip;
    QTextEdit *qte;
    QLineEdit *qle_bubble;
    QLineEdit *qle_batpos;
    QNetworkAccessManager *manager;
public:
    glider_gui(QWidget *pwgt =0);
public slots:
    void slotDIVE();
    void getResponse(QNetworkReply *reply);
    void isWIFI();
};

#endif // GLIDER_GUI_H
