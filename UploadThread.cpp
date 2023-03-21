#include "UploadThread.h"
#include<fstream>
#include<unistd.h>

UploadThread::UploadThread(HizZ::File* file,int sock_fd,SqlTool* sql)
{
    dataQueue = new ThreadQueue<std::pair<int,char*>>();
    std::thread t(work,this,file,sock_fd,sql);
    t.detach();
}

UploadThread::~UploadThread()
{
    //dtor
    delete dataQueue;
}

void UploadThread::work(UploadThread* thread,HizZ::File* file,int sock_fd,SqlTool* sql)
{
    int fileSize = file->getSize();
    int recvSize = 0;
    std::ofstream ofs;
    ofs.open(file->getPath(),std::ios::out|std::ios::app|std::ios::binary);
    if(!ofs.is_open())
    {
        perror("open file error");
        return;
    }
    while(file->getOffset()!=fileSize)
    {
        if(!thread->dataQueue->empty())
        {
            std::pair<int,char*> data;
            thread->dataQueue->dequeue(data);
            int recvSize = data.first;
            ofs.write(data.second,recvSize);
            file->addOffset(recvSize);
        }
    }
    file->submitFile();
    std::string filedata ="";
    filedata += file->getName()+"\t";

    MYSQL_RES* result = sql->exeSql("select f_id from File where f_path = \""+file->getPath()+"\";");
    std::string fid(sql->getRows(result)[0][0]);
    filedata += fid+"\t";
    filedata += std::to_string(file->getParentID())+"\t";
    filedata += file->getUploadTime()+"\t";
    filedata += std::to_string(file->getSize());

    write(sock_fd,filedata.c_str(),filedata.size());
    ofs.close();
    delete file;
}
