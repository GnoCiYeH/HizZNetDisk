#include "fileitem.h"
#include "ui_fileitem.h"
#include<QDebug>
#include"attribute.h"
File::File(QString type,int id,QString name,QString uploadTime,int size,bool isLeaf,int parentId,QObject* par):QObject (par)
{
    if(type=="dir")
    {
        this->f_type = DIR;
    }
    else if(type=="txt")
    {
        this->f_type = TXT;
    }
    else if(type=="mp4")
    {
        this->f_type = MP4;
    }
    else
    {
        this->f_type = UNKNOWN;
    }
    this->fileId = id;
    this->f_name = name;
    this->f_uploadTime = uploadTime;
    this->f_size = size;
    this->f_isLeaf = isLeaf;
    this->f_parentId = parentId;
}

File::File(const File& file)
{
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
}

FileItem::FileItem(QString type,int id,QString name,QString uploadTime,int size,bool isLeaf,int parentId) :
    ui(new Ui::FileItem)
{

    file = new File(type,id,name,uploadTime,size,isLeaf,parentId,this);
    ui->setupUi(this);

    switch (file->f_type) {
    case FileType::DIR:
    {
        QIcon icon("://images/files/file_folder@2x.png");
        ui->toolButton->setIcon(icon);
        ui->toolButton->setIconSize(QSize(70,70));
        ui->toolButton->setFixedSize(70,70);
        ui->label->setText(name.left(name.lastIndexOf('.')));
        ui->toolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
        break;
    }
    case FileType::TXT:
    {
        QIcon icon("://images/files/file_text@2x.png");
        ui->toolButton->setIcon(icon);
        ui->toolButton->setIconSize(QSize(70,70));
        ui->toolButton->setFixedSize(70,70);
        ui->label->setText(name);
        break;
    }
    case FileType::MP4:
    {
        QIcon icon("://images/files/file_video@2x.png");
        ui->toolButton->setIcon(icon);
        ui->toolButton->setIconSize(QSize(70,70));
        ui->toolButton->setFixedSize(70,70);
        ui->label->setText(name);
        break;
    }
    default:
    {
        QIcon icon("://images/files/file_unknown@2x.png");
        ui->toolButton->setIcon(icon);
        ui->toolButton->setIconSize(QSize(70,70));
        ui->toolButton->setFixedSize(70,70);
        ui->label->setText(name);
        break;
    }
    }

    connect(ui->toolButton,&FileToolButton::doubleClicked,this,[=](){
        emit doubleClickedTrigger(this);
    });

    menu = new QMenu(this);
    download_action = new QAction(this);
    download_action->setIcon(QIcon("://images/svg/download-alt.svg"));
    download_action->setText("下载");
    attribute_action = new QAction(this);
    attribute_action->setIcon(QIcon("://images/svg/info-sign.svg"));
    attribute_action->setText("属性");
    delete_action = new QAction(this);
    delete_action->setIcon(QIcon("://images/remove-gray.png"));
    delete_action->setText("删除");
    if(this->getFile()->getType()==FileType::DIR)
        download_action->setDisabled(true);
    menu->addAction(download_action);
    menu->addAction(attribute_action);
    menu->addAction(delete_action);


    connect(attribute_action,SIGNAL(triggered()),this,SLOT(showAttribute()));
    connect(delete_action,&QAction::triggered,this,[=](){
        emit deleteSignal(this);
    });
    connect(download_action,&QAction::triggered,this,[=](){
        emit downloadSignal(this);
    });

    QString str = ui->label->text();
    QFontMetrics metrics(ui->label->font());
    if (metrics.width(str) > ui->label->width())
    {
        str = QFontMetrics(ui->label->font()).elidedText(str, Qt::ElideMiddle, ui->label->width());
    }
    ui->label->setText(str);

    if(type!=FileType::DIR)
    {
        this->setToolTip(name);
    }
}

int FileItem::getChildrenSize()
{
    if(file->getType()==FileType::DIR)
    {
        int size = 0;
        for(auto i:*this->items)
        {
            size+=i->getChildrenSize();
        }
        return size;
    }
    return file->getSize();
}

QString FileItem::getDirectory()
{
    FileItem* temp = this;
    QString str = this->getFile()->getName();
    while(temp->getParent())
    {
        temp = temp->getParent();
        str.push_front(temp->getFile()->getName()+">");
    }
    return str;
}

void FileItem::showAttribute()
{
    if(this->file->getType()==FileType::DIR)
    {
        this->file->setDirNum(this->getDirNum());
        this->file->setFileNum(this->getFileNum());
        this->file->setSize(getChildrenSize());
    }
    this->file->setDirectory(this->getDirectory());

    Attribute a(*file);
    a.exec();
}

void FileItem::contextMenuEvent(QContextMenuEvent* event)
{
 menu->exec(QCursor::pos());
 event->accept();
}

int FileItem::getFileNum()
{
 int num = 0;
 for(auto i : *this->items)
 {
     if(i->getFile()->getType()==FileType::DIR)
     {
         num+=i->getFileNum();
     }
     else {
         num+=1;
     }
 }
 return num;
}

int FileItem::getDirNum()
{
    int num = 0;
    for(auto i : *this->items)
    {
     if(i->getFile()->getType()==FileType::DIR)
     {
         num+=1;
         num+=i->getDirNum();
     }
    }
    return num;
}

FileItem::FileItem(FileItem& item)
{
    file->f_type = item.getFile()->getType();
    file->f_name = item.getFile()->getName();
    file->f_isLeaf = item.getFile()->getIsLeaf();
    file->fileId = item.getFile()->getFID();
    file->f_uploadTime = item.getFile()->getUploadTime();
    file->f_size = item.getFile()->getSize();
    file->f_parentId = item.getFile()->getParentId();
    items = item.getChildern();
    f_parentFile = item.getParent();
}

FileItem FileItem::operator=(FileItem& item)
{
    FileItem i(item);
    return i;
}

FileItem::~FileItem()
{
    delete ui;
    file->deleteLater();
}

//FileItem* FileItem::copy(FileItem* item)
//{
//    FileItem * newItem = new FileItem(item->getType(),item->getFID(),item->getName(),item->getUploadTime(),item->getSize(),item->getIsLeaf(),item->getParentId());
//    QVector<FileItem*>* items = newItem->getChildern();
//    for(auto i: *item->getChildern())
//    {
//        items->push_back(i);
//    }
//}

FileToolButton::FileToolButton(QWidget * parent) : QToolButton(parent)
{

}

FileToolButton::~FileToolButton()
{

}

//实现QToolButton左键双击事件
void FileToolButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    /* 也可在此写双击想要实现的事件*/

    //如果需要自定义控件，则发送左键双击信号
    //如果双击事件则发送双击事件信号
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
    }

}
