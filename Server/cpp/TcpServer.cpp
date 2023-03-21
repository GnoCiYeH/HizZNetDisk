#include "TcpServer.h"
#include<fstream>
#include<sys/timeb.h>
#include<errno.h>
#include"UploadThread.h"

#define MAXCONNECTION 64
#define EVENT_SIZE 20
#define FILEPATH "/home/heyicong/C++/HizZNetDisk/UserFiles/"
SqlTool* TcpServer::sql = new SqlTool(SqlIP,"root","191230","HizZNetDisk");
std::unordered_map<int,HizZ::File*> TcpServer::fileMap;
TcpServer::TcpServer(const char* ip,uint32_t port,uint32_t fileport,Log::Logger logger)
{
    m_logger = logger;
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(ip);
    m_server_addr.sin_port = htons(port);

    m_file_server_addr.sin_family = AF_INET;
    m_file_server_addr.sin_addr.s_addr = inet_addr(ip);
    m_file_server_addr.sin_port = htons(fileport);

    pool = new ThreadPool(1024);
    pool->init();
}

TcpServer::~TcpServer()
{
    close(m_sock_fd);
    close(m_epfd);
    delete sql;
    delete pool;
}

void TcpServer::recvFile(int fd,int offset)
{
    HizZ::File* file = fileMap.find(fd)->second;
    int recvSize = 0;
    int fileSize = file->getSize();
    UploadThread thread(file,fd,sql);
    while(offset!=fileSize)
    {
        char* data = new char[4096];
        memset(data,0,sizeof(data));
        recvSize = read(fd,data,sizeof(data));
        if(recvSize==0)
        {
            return;
        }
        thread.addData(data,recvSize);
    }
    fileMap.erase(fd);
}

void TcpServer::tcpStart()
{
    int ret;
    m_sock_fd = socket(AF_INET,SOCK_STREAM,0);
    FATAL_CHECK(m_sock_fd,"Create socket error",m_logger);
    int reuse = 1;
    if (setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
    }
    ret = bind(m_sock_fd,(sockaddr*)&m_server_addr,sizeof(m_server_addr));
    FATAL_CHECK(ret,"TcpServer bind ERROR!",m_logger);
    ret = listen(m_sock_fd,MAXCONNECTION);
    FATAL_CHECK(ret,"TcpServer listen ERROR",m_logger);
    m_epfd = epoll_create(MAXCONNECTION);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = m_sock_fd;

    epoll_ctl(m_epfd,EPOLL_CTL_ADD,m_sock_fd,&ev);
    epoll_event events[EVENT_SIZE];

    while(true)
    {
        int eNum = epoll_wait(m_epfd,events,EVENT_SIZE,-1);
        FATAL_CHECK(eNum,"epoll_wait error",m_logger);
        char buf[8];

        for(int i = 0 ; i < eNum ; i++)
        {
            if(events[i].data.fd == m_sock_fd)
            {
                ret = accept(m_sock_fd,NULL,NULL);
                ERROR_CHECK(ret,"TcpServer accept error",m_logger);
                ev.data.fd = ret;
                ev.events = EPOLLIN;
                epoll_ctl(m_epfd,EPOLL_CTL_ADD,ret,&ev);
                INFO_LOG(m_logger,"connect");
            }
            else if(events[i].events&EPOLLIN)
            {
                memset(buf,0,sizeof(buf));
                int sock_fd = events[i].data.fd;
                if((ret=read(sock_fd,buf,sizeof(buf)))==0)
                {
                    INFO_LOG(m_logger,std::string("client_fd:")+(m_logger,std::to_string(sock_fd))+" disconnected");
                    epoll_ctl(m_epfd,EPOLL_CTL_DEL,sock_fd,NULL);
                }
                else
                {
                    dataProcess(sock_fd,buf,sizeof(buf));
                }
            }
        }
    }
}

void TcpServer::dataProcess(int sock_fd,char* buf,int size)
{
    int type = bytesToInt(buf,0,sizeof(buf));
    int packageSize = bytesToInt(buf,4,sizeof(buf));

    switch(type)
    {
    case Package::PackageType::REGISTE:
        break;
    case Package::PackageType::SIGNIN:
    {
        char data[packageSize+1];
        data[packageSize] = '\0';
        read(sock_fd,data,packageSize);
        std::string str(data);
        std::vector<std::string> res;
        stringSplit(str,"\n",res);
        int n = res.size();
        MYSQL_RES* result = sql->exeSql("select* from User where id = \""+res[0]+"\" and password =md5(\""+res[1]+"\");");
        std::vector<MYSQL_ROW> rows = sql->getRows(result);
        if(rows.size() == 0)
        {
            Package pck(NULL,Package::ERROR,0);
            write(sock_fd,pck.getPdata(),pck.getSize());
        }
        else
        {
            std::string* userId = new std::string(rows[0][1]);
            Package pck(rows[0][1],Package::ALLOW,sizeof(rows[0][1]));
            write(sock_fd,pck.getPdata(),pck.getSize());

            std::thread* t = new std::thread(sendDir,sock_fd,userId);
            t->detach();
        }
        mysql_free_result(result);
        break;
    }
    case Package::PackageType::FILEHEAD:
    {
        char data[packageSize+1];
        data[packageSize] = '\0';
        read(sock_fd,data,packageSize);
        std::string fileInfo(data);
        std::vector<std::string> res;
        stringSplit(fileInfo,"\n",res);

        std::string fpath = FILEPATH;
        timeb t;
        ftime(&t);
        int time = t.time*1000+t.millitm;
        fpath.append(std::to_string(time)+"-");
        fpath.append(res[2]);

        std::string uid = res[0];
        int fsize =std::stoi(res[1]);
        std::string fname = res[2];
        int fpid = std::stoi(res[3]);

        time_t timep;
        std::time(&timep);
        char tmp[256];
        std::strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
        std::string uploadTime(tmp);

        HizZ::File* file = new HizZ::File(HizZ::File(uid,fsize,fname,fpid,fpath));
        std::pair<int, HizZ::File*> pair(sock_fd,file);
        fileMap.insert(pair);
        sql->exeSql("insert into File(u_id,f_name,f_parent,f_uploadtime,f_size,f_offset,f_path)values(\""
                   +uid+"\",\""+fname+"\","+std::to_string(fpid)+",\""+uploadTime+"\","+std::to_string(fsize)+","+std::to_string(0)+",\""+fpath+"\")");
        pool->submit(recvFile,sock_fd,0);
        epoll_ctl(m_epfd,EPOLL_CTL_DEL,sock_fd,NULL);
        break;
    }
    case Package::PackageType::DELETEFILE:
    {
        char data[packageSize+1];
        data[packageSize] = '\0';
        read(sock_fd,data,packageSize);
        std::string info(data);
        std::vector<std::string> res;
        stringSplit(info,"\n",res);
        if(res[1]=="0")
        {
            sql->exeSql("delete from Directory where dirId = " + res[0] + ";");
        }
        else if(res[1]=="1")
        {
            MYSQL_RES* result = sql->exeSql("select f_path from File where f_id = "+ res[0] + ";");
            std::vector<MYSQL_ROW> vec = sql->getRows(result);
            remove(vec[0][0]);
            sql->exeSql("delete from File where f_id = " + res[0] + ";");
        }
        break;
    }
    case Package::PackageType::NEWFOLDER:
    {
        char data[packageSize+1];
        data[packageSize] = '\0';
        read(sock_fd,data,packageSize);
        std::string info(data);
        std::vector<std::string> res;
        stringSplit(info,"\n",res);
        sql->exeSql("insert into Directory (userId,parent,name) values (\""+ res[0] + "\","+res[2]+",\""+res[1]+"\");");
        MYSQL_RES* result = sql->exeSql("select parent,name,isLeaf,dirId from Directory where userId = \""+res[0]+"\" and parent = "+res[2]+" and name =\""+res[1]+"\";");
        std::vector<MYSQL_ROW> vec = sql->getRows(result);
        std::string DirData = std::string(vec[0][0]) + "\n" + std::string(vec[0][1])+"\n"+std::string(vec[0][2])+"\n"+std::string(vec[0][3]);
        Package dirPck(DirData.c_str(),Package::NEWDIRINFO,DirData.size());
        write(sock_fd,dirPck.getPdata(),dirPck.getSize());
        mysql_free_result(result);
    }
    case Package::PackageType::DOWNLOADFILE:
    {
        char data[packageSize+1];
        data[packageSize] = '\0';
        read(sock_fd,data,packageSize);
        std::string info(data);
        MYSQL_RES* result = sql->exeSql("select f_name, f_size ,f_path from File where f_id = "+info+";");
        std::vector<MYSQL_ROW> vec = sql->getRows(result);

        std::string path = std::string(vec[0][2]);
        int fileSize = std::stoi(vec[0][1]);
        std::string buf = std::string(vec[0][0]) + "\n" +std::string(vec[0][1]);

        Package pck(buf.c_str(),Package::DATA,buf.size());
        write(sock_fd,pck.getPdata(),pck.getSize());

        pool->submit(sendFile,sock_fd,path,fileSize);
        mysql_free_result(result);
        break;
    }
    default:
        INFO_LOG(m_logger,"UNKNOW PACKAGETYPE");
    break;
    }
}

void TcpServer::sendFile(int sock_fd,std::string path,int fileSize)
{
    int sendSize = 0;
    int ret = 0;
    char buf[1024];
    std::ifstream ifs;
    ifs.open(path,std::ios::in|std::ios::binary);
    if(!ifs.is_open())
    {
        std::cout<<"send file open error";
    }
    while(sendSize<fileSize)
    {
        ret = ifs.readsome(buf,sizeof(buf));
        sendSize+=ret;
        write(sock_fd,buf,ret);
    }
    ifs.close();
}

void TcpServer::sendDir(int sock_fd,std::string* UserId)
{
    std::string data ="";
    MYSQL_RES* file = sql->exeSql("select f_name,f_id,f_parent,f_uploadtime,f_size from File where u_id = \""+*UserId+"\";");
    int field_num = mysql_num_fields(file);
    std::vector<MYSQL_ROW> file_rows = sql->getRows(file);
    for(auto item:file_rows)
    {
        for(int i = 0; i < field_num; i++)
        {
            if(item[i]!=nullptr)
            {
                std::string temp(item[i]);
                data = data.append(temp);
            }
            else
            {
                std::string temp = "null";
                data = data.append(temp);
            }
            data+="\t";
        }
        data.append("\n");
    }
    mysql_free_result(file);
    MYSQL_RES* dir = sql->exeSql("select name,dirId,parent,isLeaf from Directory where userId = \""+ *UserId +"\";");
    field_num = mysql_num_fields(dir);
    std::vector<MYSQL_ROW> dir_rows = sql->getRows(dir);
    for(auto item:dir_rows)
    {
        for(int i = 0; i < field_num; i++)
        {
            if(item[i]!=nullptr)
            {
                std::string temp(item[i]);
                data = data.append(temp);
            }
            else
            {
                std::string temp = "null";
                data = data.append(temp);
            }
            data+="\t";
        }
        data.append("\n");
    }
    mysql_free_result(dir);
    Package pck(data.c_str(),Package::UPDATEFILE,data.size());
    write(sock_fd,pck.getPdata(),pck.getSize());
}
