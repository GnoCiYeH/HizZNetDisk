#ifndef FILE_H
#define FILE_H
#include<string>
namespace HizZ{
class File
{
public:
    File(){}
    File(std::string uid,int size,std::string name,int parentId,std::string path);
    virtual ~File();
    int getOffset(){return f_offset;}
    int getSize(){return f_size;}
    std::string getName(){return f_name;}
    std::string getUploadTime(){return f_uploadTime;}
    std::string getUserId(){return u_id;}
    int getParentID(){return f_parent;}
    std::string getPath(){return f_path;}
    int getMd5(){return f_md5;}
    void addOffset(int i){this->f_offset+=i;}

    void submitFile();
private:
    int f_offset = 0;
    int f_size = 0;
    std::string f_name;
    std::string f_uploadTime;
    std::string u_id;
    int f_parent;
    std::string f_path;
    int f_md5;
};
}

#endif // FILE_H
