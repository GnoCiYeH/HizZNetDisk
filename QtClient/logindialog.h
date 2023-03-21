#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include"tcpTool.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

private slots:
    void on_btn_login_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::LoginDialog *ui;
    QString username_;
    QString password_;
    QString computer_name_;
    bool is_remember_device_;

    QTcpSocket* socket = new QTcpSocket;
};

#endif // LOGINDIALOG_H
