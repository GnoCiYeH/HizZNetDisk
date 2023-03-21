#include "tcpTool.h"

TcpTool::TcpTool()
{

}

void TcpTool::sendToServer(QTcpSocket *socket, Package package)
{
    socket->write(package.getPdata());
}
