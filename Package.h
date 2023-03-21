#ifndef PACKAGE_H
#define PACKAGE_H


class Package
{
public:
    enum PackageType{
        REGISTE = 0,
        SIGNIN,
        FILEHEAD,
        DATA,
        DELETEFILE,
        DOWNLOADFILE,
        NEWFOLDER
    };
    enum SendType{
        EXISTENT = 0,
        ALLOW,
        ERROR,
        UPDATEFILE,
        NEWDIRINFO
    };
    Package(const char* data,int type,int size);
    ~Package();
public:
    char* getPdata(){return p_data;}
    int getSize(){return p_size;}
private:
    char* p_data;
    int p_type;
    int p_size;
};

#endif // PACKAGE_H
