#ifndef PACKAGE_H
#define PACKAGE_H

#include<QByteArray>

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
        _ERROR,
        UPDATEFILE,
        NEWDIRINFO
    };
    Package(QByteArray data,PackageType type);
    ~Package();
public:
    QByteArray getPdata(){return p_data;}
    static QByteArray intToByteArr(int num);
    static int ByteArrToInt(QByteArray arr,int offset);
    int getSize(){return size;}
private:
    QByteArray p_data;
    PackageType p_type;
    int size;
};

#endif // PACKAGE_H
