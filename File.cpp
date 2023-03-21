#include "File.h"
#include"SqlTool.h"
using namespace HizZ;
File::File(std::string uid,int size,std::string name,int parentId,std::string path)
{
    u_id = uid;
    f_size = size;
    f_name = name;
    f_parent = parentId;
    f_path = path;
}

File::~File()
{
    //dtor
}

void File::submitFile()
{
    SqlTool tool("192.168.239.129","root","191230","HizZNetDisk");
    time_t timep;
    std::time(&timep);
    char tmp[256];
    std::strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    std::string uploadTime(tmp);
    this->f_uploadTime = uploadTime;
    tool.exeSql("update File set f_uploadtime = \""+uploadTime+"\", f_offset = "+std::to_string(this->f_offset)+" where f_path = \""+this->f_path+"\";");
}
