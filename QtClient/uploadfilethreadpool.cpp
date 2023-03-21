#include "uploadfilethreadpool.h"
#include"package.h"
#include<QFile>
#include"package.h"
#include<QDateTime>
#include<QTimer>


DownloadFileThread::DownloadFileThread(int f_id,QString path)
{
    this->setAutoDelete(false);
    this->f_id = f_id;
    this->path = path;
    QDateTime time = QDateTime::currentDateTime();
    this->t_id = time.toTime_t();

    socket = new QTcpSocket;
    socket->connectToHost("192.168.239.129",8888);
    bool isFirst = true;
    connect(socket,&QTcpSocket::connected,this,[=](){
        Package pck(QString::number(f_id).toUtf8(),Package::DOWNLOADFILE);
        socket->write(pck.getPdata());
    });
    connect(socket,&QTcpSocket::readyRead,this,[=]()mutable{
        if(isFirst)
        {
            QByteArray arr = socket->read(8);
            int size = Package::ByteArrToInt(arr,4);
            QByteArray data = socket->read(size);
            QString fileInfo(data);
            QStringList list = fileInfo.split("\n");
            path+=list[0];
            fileSize = list[1].toInt();
            file = new QFile(path);
            file->open(QIODevice::WriteOnly);
            isFirst=false;
        }
        else {
            QByteArray arr = socket->readAll();
            dataQueue.enqueue(arr);
        }
    });
}

void DownloadFileThread::run()
{
    qint64 recvSize = 0;
    while(recvSize!=fileSize)
    {
        if(!dataQueue.empty())
        {
            QByteArray buf;
            dataQueue.dequeue(buf);
            int ret = buf.size();
            file->write(buf,ret);
            recvSize+=ret;
            emit setProgressBarVal(this->t_id,static_cast<int>(recvSize*100/fileSize));
        }
    }
    emit setProgressBarVal(this->t_id,100);
    emit finish(this->t_id);
    this->exit(0);
    //this->exec();
}

UploadFileThread::UploadFileThread(QString path,QString u_id,int f_parent)
{
    this->path = path;
    this->u_id = u_id;
    this->f_parent = f_parent;
    QDateTime time = QDateTime::currentDateTime();
    this->t_id = time.toTime_t();
}

void UploadFileThread::run()
{
    socket = new QTcpSocket();
    socket->connectToHost("192.168.239.129",8888);
    connect(socket,&QTcpSocket::connected,this,[=](){
        file = new QFile(path);
        file->open(QIODevice::ReadOnly);
        qint64 fileSize = file->size();
        QString fileName = path.mid(path.lastIndexOf('/')+1);

        QString data = u_id + "\n" + QString::number(fileSize) + "\n" + fileName + "\n"+ QString::number(f_parent) + '\0';
        Package pck(data.toUtf8(),Package::FILEHEAD);
        socket->write(pck.getPdata());
        socket->flush();
        char buf[1024];
        int ret = 0;
        qint64 sended = 0;
        while((ret = file->read(buf,sizeof(buf)))!=0)
        {
            sended+=ret;
            socket->write(buf,ret);
            emit setProgressBarVal(this->t_id,static_cast<int>(sended*100/fileSize));
            socket->flush();
        }
        emit setProgressBarVal(this->t_id,100);
        file->close();
    },Qt::DirectConnection);

    connect(socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray arr = socket->readAll();
        QString returnData = QString(arr);
        socket->close();
        emit itemUploadFinished(this->t_id,returnData);
    },Qt::DirectConnection);

    this->exec();
}
