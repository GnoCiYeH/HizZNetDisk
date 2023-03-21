#include "logindialog.h"
#include "ui_logindialog.h"
#include"myhelper.h"
#include"tcpTool.h"
#include <QDebug>
#include"mainwindow.h"
#include<QTimer>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    myHelper::setStyle(":/qss/psblack.css");
    setWindowTitle(tr("HizZNetDisk"));
    setWindowIcon(QIcon(":/images/seafile.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->logo->setPixmap(QPixmap(":/images/seafile-32.png"));
    ui->label_serverError->setText("");
    ui->label_UserIdError->setText("");
    ui->label_pswError->setText("");

    const QRect screen = QApplication::desktop()->screenGeometry();
    move(screen.center() - this->rect().center());
}

void LoginDialog::on_btn_login_clicked()
{
    //ui->btn_login->setDisabled(true);
    QString server = ui->lineEdit_server->text();
    QString UserId = ui->lineEdit_UserId->text();
    QString psw = ui->lineEdit_psw->text();
    if(UserId.size()>32){
        ui->label_UserIdError->setText("id error");
        return;
    }
    if(psw.size()>32)
    {
        ui->label_pswError->setText("psw error");
        return;
    }
    ui->label_state->setText("connect to the server...");
    socket->connectToHost("192.168.239.129",8888);
    connect(socket,&QTcpSocket::connected,this,[=](){
        ui->label_state->setText("connect to the server succeeded!");

        QString data = UserId + "\n" + psw;
        Package package(data.toUtf8(),Package::PackageType::SIGNIN);
        socket->write(package.getPdata());
    });

    connect(socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray arr = socket->read(8);
        int type = Package::ByteArrToInt(arr,0);
        int size = Package::ByteArrToInt(arr,4);
        switch (type) {
        case Package::ALLOW:
        {
            QByteArray userName = socket->readAll();
            this->hide();
            MainWindow * m = new MainWindow(socket,QString(userName),UserId);
            m->show();
            break;
        }
        case Package::_ERROR:
        {
            socket->readAll();
            ui->label_state->setText("The account number or password is incorrect!");
            socket->close();
            ui->btn_login->setDisabled(false);
        }
        }
    });
    QTimer::singleShot(1000,this,[=](){
        if(socket->state()!=QAbstractSocket::ConnectedState)
        {
            socket->readAll();
            socket->close();
            ui->btn_login->setDisabled(false);
            ui->label_state->setText("connect to the server failed");
        }
    });
}

void LoginDialog::on_btn_cancel_clicked()
{
    exit(0);
}
