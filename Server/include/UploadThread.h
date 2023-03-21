#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H
#include"threadPool.h"
#include"File.h"
#include"SqlTool.h"

//管理用户上传文件的数据队列以及对数据及时处理（数据的入队和处理分线程处理）
class UploadThread
{
    public:
        UploadThread(HizZ::File* file,int sock_fd,SqlTool* sql);
        virtual ~UploadThread();

        void addData(char*buf,int recvSize){
            std::pair<int,char*> data(recvSize,buf);
            dataQueue->enqueue(data);
            }

    private:
        ThreadQueue<std::pair<int,char*>>* dataQueue;

        static void work(UploadThread* thread,HizZ::File* file,int sock_fd,SqlTool* sql);
};

#endif // UPLOADTHREAD_H
