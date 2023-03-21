
#include"logindialog.h"
#include"mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog w;
    w.show();
//    MainWindow w(new QTcpSocket," ");
//    w.show();

    return a.exec();
}
