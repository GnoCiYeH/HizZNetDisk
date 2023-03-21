#ifndef TCPTOOL_H
#define TCPTOOL_H

#include<QTcpSocket>
#include"package.h"

class TcpTool : public QObject
{
    Q_OBJECT
public:
    TcpTool();
    static void sendToServer(QTcpSocket* socket,Package package);
    static void reciveFromServer(QTcpSocket* socket);
    static void updateFile(QTcpSocket* socket,QString path);
    static void downloadFile(QTcpSocket* socket,QString path);
private:
};

#endif // TCPTOOL_H
