#ifndef UPLOADFILETHREADPOOL_H
#define UPLOADFILETHREADPOOL_H

#include<QObject>
#include<QTcpSocket>
#include<QThread>
#include<QProgressBar>
#include<QQueue>
#include<QHash>
#include<QFile>
#include<QMutex>
#include<QWaitCondition>
#include<QRunnable>
#include<queue>

template<typename T>
class SafeQueue
{
public:
    SafeQueue(){}
    bool empty()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void enqueue(T& t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.emplace(t);
    }

    bool dequeue(T& t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_queue.empty())
            return false;
        t = std::move(m_queue.front());
        m_queue.pop();

        return true;
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }


private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
};


class UploadFileThread : public QThread, public QRunnable
{
    Q_OBJECT
public:
    UploadFileThread(QString path,QString u_id,int f_parent);
    ~UploadFileThread(){}
    int getId(){return t_id;}
private:
    void run()override;
    int t_id;
    QString path;
    QString u_id;
    int f_parent;
    QFile* file;
    bool isWork = true;
    QTcpSocket* socket;

signals:
    void itemUploadFinished(int,QString);
    void setProgressBarVal(int,int);
};

class DownloadFileThread : public QThread, public QRunnable
{
    Q_OBJECT
public:
    DownloadFileThread(int f_id,QString path);
    ~DownloadFileThread(){}
    int getId(){return t_id;}
    void closeWork(){
        file->close();
        socket->close();
        this->deleteLater();
    }
private:
    void run()override;
    int t_id;
    int f_id;
    QFile* file;
    bool isWork = true;
    QTcpSocket* socket;
    QString path;
    SafeQueue<QByteArray> dataQueue;
    qint64 fileSize = -1;

public:

signals:
    void finish(int);
    void setProgressBarVal(int,int);
};
#endif // UPLOADFILETHREADPOOL_H
