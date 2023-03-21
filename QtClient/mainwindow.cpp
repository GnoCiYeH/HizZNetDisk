#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<myhelper.h>
#include"fileitem.h"
#include"package.h"
#include<QHash>
#include"fileindexlable.h"
#include"selectfiledialog.h"
#include"downloaditem.h"
#include"newfolderdialog.h"

#define PATH "D:\\QTproject\\HizZNetDisk\\testFIle\\"

MainWindow::MainWindow(QTcpSocket* sock,QString Username,QString UserId,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->socket = sock;
    socket->disconnect();

    this->UserId = UserId;

    root = new FileItem("dir",0,Username,nullptr,0,false,0);
    root->hide();

    curDir = root;

    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::dataProcess);

    ui->setupUi(this);
    myHelper::setStyle(":/qss/darkgray.css");
    FileIndexWidget* indexWidget = ui->fileIndex_widget;
    indexWidget->addLable(root);
    connect(indexWidget,SIGNAL(clicked(FileItem*)),this,SLOT(indexClicked(FileItem*)));

    ui->scrollArea_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->Download_Area->setLayout(new QVBoxLayout());
}

MainWindow::~MainWindow()
{
    delete ui;
    for(auto i : f_items)
    {
        delete i;
    }
}

void MainWindow::updateProgressVal(int id,int val)
{
    DownloadItem* item = downloadItemMap.find(id).value();
    item->getProgressBar()->setValue(val);
}

void MainWindow::indexClicked(FileItem* item)
{
    while(curDir!=item)
    {
        backBtn();
    }
}

void MainWindow::openFile(FileItem* item)
{
    if(item->getFile()->getType()==FileType::DIR)
    {
        curDir = item;
        for(auto i : showItemMap)
        {
            i->setHidden(true);
            HideItemMap.insert(i->getFile()->getFID(),i);
        }
        showItemMap.clear();
        for(auto i : *item->getChildern())
        {
            i->setHidden(false);
            showItemMap.insert(i->getFile()->getFID(),i);
        }
        ui->fileIndex_widget->addLable(item);
    }
}

void MainWindow::updateFile()
{
    myHelper::setStyle(":/qss/darkgray.css");
    dir.insert(0,root);
    for(auto item : f_items)
    {
        if(item->getFile()->getType()==FileType::DIR)
        {
            dir.insert(item->getFile()->getFID(),item);
        }
    }
    for(auto item : f_items)
    {
        int p_id = item->getFile()->getParentId();
        FileItem* parFile = dir.find(p_id).value();
        parFile->getChildern()->insert(item->getFile()->getFID(),item);
        item->setParent(parFile);
    }

    QWidget* widget = ui->file_widget;
    QGridLayout * layout = new QGridLayout (widget);
    filepage = new QWidget(widget);

    layout->addWidget(filepage,0,0);
    //layout->addItem(new QSpacerItem(0,50,QSizePolicy::Expanding,QSizePolicy::Fixed),0,1);
    //layout->addItem(new QSpacerItem(0,50,QSizePolicy::Fixed,QSizePolicy::Expanding),1,0);

    widget->setLayout(layout);

    file_grid_layout* file_layout = new file_grid_layout(filepage);
    for(auto item : f_items)
    {
        item->setHidden(true);
        HideItemMap.insert(item->getFile()->getFID(),item);
        file_layout->addFileItem(item);
        connect(item,SIGNAL(doubleClickedTrigger(FileItem*)),this,SLOT(openFile(FileItem*)));
        connect(item,SIGNAL(deleteSignal(FileItem*)),this,SLOT(deleteFile(FileItem*)));
        connect(item,SIGNAL(downloadSignal(FileItem*)),this,SLOT(downloadFile(FileItem*)));
    }
    for(auto item : *root->getChildern())
    {
        item->setHidden(false);
        showItemMap.insert(item->getFile()->getFID(),item);
        HideItemMap.remove(item->getFile()->getFID());
    }
    filepage->setLayout(file_layout);
}

void MainWindow::dataProcess()
{
    QByteArray buf = socket->read(8);
    int type = Package::ByteArrToInt(buf,0);
    QByteArray data = socket->readAll();
    switch (type) {
        case Package::SendType::UPDATEFILE:
        {
            QString buf(data);
            QStringList list = buf.split("\n"); //每一行数据
            for(int i = 0;i < list.size();i++)
            {
                QString temp = list.at(i);
                if(temp.size()==0)
                    continue;
                QStringList tempList = temp.split("\t");
                QString fileName = tempList.at(0);
                QString type = fileName.mid(fileName.lastIndexOf('.')+1);
                if(type=="dir")
                {
                    f_items.push_back(new FileItem(type,tempList.at(1).toInt(),fileName,nullptr,0,tempList.at(3).toInt(),tempList.at(2).toInt()));
                }
                else
                {
                    f_items.push_back(new FileItem(type,tempList.at(1).toInt(),fileName,tempList.at(3),tempList.at(4).toInt(),true,tempList.at(2).toInt()));
                }
            }

            updateFile();
            break;
        }
    case Package::SendType::NEWDIRINFO:
    {
        QString buf(data);
        QStringList list = buf.split("\n");
        int id = list[3].toInt();
        int parId = list[0].toInt();
        FileItem* item = new FileItem("dir",id,list[1],nullptr,0,list[2].toInt(),parId);
        connect(item,SIGNAL(doubleClickedTrigger(FileItem*)),this,SLOT(openFile(FileItem*)));
        connect(item,SIGNAL(deleteSignal(FileItem*)),this,SLOT(deleteFile(FileItem*)));
        connect(item,SIGNAL(downloadSignal(FileItem*)),this,SLOT(downloadFile(FileItem*)));
        FileItem* par = dir.find(parId).value();
        par->getChildern()->insert(id,item);
        item->setParent(par);
        f_items.push_back(item);
        dir.insert(id,item);
        static_cast<file_grid_layout*>(filepage->layout())->addFileItem(item);


        if(curDir==item->getParent())
        {
            item->setHidden(false);
            showItemMap.insert(id,item);
        }
        else {
            item->setHidden(true);
            HideItemMap.insert(id,item);
        }
        break;
    }
    }
}

void MainWindow::backBtn()
{
    if(curDir==root)
        return;

    FileItem* item = curDir->getParent();
    curDir = item;
    for(auto i : showItemMap)
    {
        i->setHidden(true);
        HideItemMap.insert(i->getFile()->getFID(),i);
    }
    showItemMap.clear();
    for(auto i : *item->getChildern())
    {
        i->setHidden(false);
        showItemMap.insert(i->getFile()->getFID(),i);
    }
    ui->fileIndex_widget->deleteLable();
}

void MainWindow::on_toolButton_10_clicked()
{
    backBtn();
}

void MainWindow::on_toolButton_3_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_toolButton_4_clicked()
{
    SelectFileDialog fileDialog(dir,root,this);

    connect(&fileDialog,SIGNAL(uplodeInfo(QString,int)),this,SLOT(uploadFile(QString ,int)));

    fileDialog.exec();
}

void MainWindow::deleteFile(FileItem* file)
{
    int fd = file->getFile()->getFID();
    if(file->getFile()->getType()==FileType::DIR)
    {
        QString str = QString::number(file->getFile()->getFID()) + "\n0";
        Package pck(str.toUtf8(),Package::DELETEFILE);
        socket->write(pck.getPdata());
        socket->flush();
        for(auto i : *file->getChildern())
        {
            deleteFile(i);
        }
        dir.find(fd).value()->deleteLater();
        dir.remove(fd);
    }
    //不是目录的情况
    else{
        QString str = QString::number(file->getFile()->getFID()) + "\n1";
        Package pck(str.toUtf8(),Package::DELETEFILE);
        socket->write(pck.getPdata());
        socket->flush();
    }

    file->getParent()->getChildern()->remove(fd);
    if(HideItemMap.contains(fd))
    {
        HideItemMap.find(fd).value()->deleteLater();
        HideItemMap.remove(fd);
    }
    if(showItemMap.contains(fd))
    {
        showItemMap.find(fd).value()->deleteLater();
        showItemMap.remove(fd);
    }
}

void MainWindow::downloadFile(FileItem* item)
{
    auto layout = ui->Download_Area->layout();
    DownloadItem* downloadItem = new DownloadItem(this);

    connect(item,SIGNAL(itemClear(int)),this,SLOT(deleteFromDownloadArea(int)));
    downloadItem->getProgressBar()->setRange(0,100);
    layout->addWidget(downloadItem);

    DownloadFileThread* thread = new DownloadFileThread(item->getFile()->getFID(),PATH);
    connect(thread,SIGNAL(setProgressBarVal(int,int)),this,SLOT(updateProgressVal(int,int)));
    connect(thread,SIGNAL(finish(int)),this,SLOT(transmitFinished(int)));

    int id = thread->getId();
    pool->start(thread);
    downloadItem->setId(id);
    downloadItemMap.insert (id,downloadItem);
    downloadThreadMap.insert(id,thread);
}

void MainWindow::uploadFile(QString filePath,int dirId)
{
    QString info = filePath + "\n" + QString::number(dirId);
    auto layout = ui->Download_Area->layout();
    DownloadItem* item = new DownloadItem(this);

    connect(item,SIGNAL(itemClear(int)),this,SLOT(deleteFromDownloadArea(int)));

    item->getProgressBar()->setRange(0,100);
    layout->addWidget(item);
    UploadFileThread* thread = new UploadFileThread(filePath,UserId,dirId);
    connect(thread,SIGNAL(setProgressBarVal(int,int)),this,SLOT(updateProgressVal(int,int)));
    connect(thread,SIGNAL(itemUploadFinished(int,QString)),this,SLOT(transmitFinished(int,QString)));

    int id = thread->getId();
    pool->start(thread);
    item->setId(id);
    downloadItemMap.insert (id,item);
}

void MainWindow::transmitFinished(int id)
{
    auto item = downloadItemMap.find(id).value();
    downloadItemMap.remove(id);
    auto thread = downloadThreadMap.find(id).value();
    thread->closeWork();
}

void MainWindow::transmitFinished(int id,QString filedata)
{
    DownloadItem* item = downloadItemMap.find(id).value();
    item->finished();

    QStringList tempList = filedata.split("\t");
    QString fileName = tempList.at(0);
    QString type = fileName.mid(fileName.lastIndexOf('.')+1);
    FileItem* fileitem;
    fileitem = new FileItem(type,tempList.at(1).toInt(),fileName,tempList.at(3),tempList.at(4).toInt(),true,tempList.at(2).toInt());
    f_items.push_back(fileitem);

    connect(fileitem,SIGNAL(doubleClickedTrigger(FileItem*)),this,SLOT(openFile(FileItem*)));
    connect(fileitem,SIGNAL(deleteSignal(FileItem*)),this,SLOT(deleteFile(FileItem*)));
    connect(fileitem,SIGNAL(downloadSignal(FileItem*)),this,SLOT(downloadFile(FileItem*)));

    FileItem* par = dir.find(tempList.at(2).toInt()).value();
    fileitem->setParent(par);
     int fd = fileitem->getFile()->getFID();
    par->getChildern()->insert(fd,fileitem);
    file_grid_layout* layout = static_cast<file_grid_layout*>(filepage->layout());
    if(curDir==fileitem->getParent())
    {
        item->setHidden(false);
        showItemMap.insert(fd,fileitem);
    }
    else {
        item->setHidden(true);
        HideItemMap.insert(fd,fileitem);
    }
    layout->addFileItem(fileitem);
    this->update();
    //添加进传输完成窗口中
    //
}

void MainWindow::deleteFromDownloadArea(int id)
{
    downloadItemMap.find(id).value()->deleteLater();
}

void MainWindow::on_toolButton_14_clicked()
{
    NewFolderDialog a(root,dir,this);
    a.exec();
    if(a.isRight())
    {
        QString data = UserId +"\n"+a.getName()+".dir"+"\n"+QString::number(a.getParID());
        Package pck(data.toUtf8(),Package::NEWFOLDER);
        socket->write(pck.getPdata());
        socket->flush();
    }
}
