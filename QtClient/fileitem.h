#ifndef FILEITEM_H
#define FILEITEM_H

#include <QWidget>
#include<QVector>
#include<QMouseEvent>
#include<QToolButton>
#include<QMenu>
enum FileType{
    DIR,
    TXT,
    MP4,
    ZIP,
    PNG,
    UNKNOWN
};
class File : public QObject
{
    Q_OBJECT
public:
    friend class FileItem;
    File(){}
    File(const File& file);
    File(QString type,int id,QString name,QString uploadTime,int size,bool isLeaf,int parentId,QObject* parent = nullptr);
    int getParentId()const{return f_parentId;}
    int getFID()const{return fileId;}
    FileType getType()const{return f_type;}
    QString getUploadTime()const{return f_uploadTime;}
    bool getIsLeaf()const{return f_isLeaf;}
    int getSize()const{return f_size;}
    QString getName()const{return f_name;}
    int getFileNum()const{return f_fileNum;}
    int getDirNum()const{return f_dirNum;}
    QString getDirectory()const{return f_directory;}

    void setFileNum(int num){this->f_fileNum = num;}
    void setDirNum(int num){this->f_dirNum = num;}
    void setSize(int num){this->f_size=num;}
    void setDirectory(QString str){this->f_directory = str;}

    File& operator=(const File& file)
    {
        if(this==&file)
            return *this;

        this->f_name = file.getName();
        this->fileId = file.getFID();
        this->f_type = file.getType();
        this->f_size = file.getSize();
        this->f_dirNum = file.getDirNum();
        this->f_isLeaf = file.getIsLeaf();
        this->f_fileNum = file.getFileNum();
        this->f_parentId = file.getParentId();
        this->f_directory = file.getDirectory();
        this->f_uploadTime = file.getUploadTime();
        return *this;
    }

private:
    FileType f_type;
    QString f_name = "unknwon";
    bool f_isLeaf = true;
    int fileId;
    QString f_uploadTime = "unknown";
    int f_size = 0;
    int f_parentId = 0;
    int f_fileNum = 0;
    int f_dirNum = 0;
    QString f_directory;
};

//带双击事件的QToolButoon
class FileToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FileToolButton(QWidget * parent = nullptr);
    ~FileToolButton();

    //实现QToolButton左键双击事件
    virtual void mouseDoubleClickEvent(QMouseEvent *event)override;

signals:
    void doubleClicked();
};

namespace Ui {
class FileItem;
}

class FileItem : public QWidget
{
    Q_OBJECT
public:
    FileItem(){}
    explicit FileItem(QString type,int id,QString name,QString uploadTime,int size,bool isLeaf,int parentId);
    FileItem(FileItem& item);
    ~FileItem();
    QHash<int,FileItem *>* getChildern(){return items;}

    void setParent(FileItem* parent){this->f_parentFile = parent;}
    FileItem* getParent(){
        if(!f_parentFile)
            return nullptr;
        return f_parentFile;
    }
    FileItem operator=(FileItem& item);
    File* getFile(){return file;}
    int getFileNum();
    int getDirNum();
    int getChildrenSize();
    QString getDirectory();
public slots:
    void showAttribute();
signals:
    void doubleClickedTrigger(FileItem*);
    void deleteSignal(FileItem*);
    void downloadSignal(FileItem*);

protected:
    void contextMenuEvent(QContextMenuEvent* event)override;

private:
    File* file;
    Ui::FileItem *ui;
    QHash<int,FileItem *>* items = new QHash<int,FileItem*>();
    FileItem * f_parentFile = nullptr;
    QMenu* menu;
    QAction* download_action;
    QAction* attribute_action;
    QAction* delete_action;
};

#endif // FILEITEM_H
