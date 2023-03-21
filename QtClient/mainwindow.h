#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPainter>
#include<QStyleOption>
#include<QTcpSocket>
#include"file_grid_layout.h"
#include"fileitem.h"
#include"uploadfilethreadpool.h"
#include"downloaditem.h"
#include<QThreadPool>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QTcpSocket* socket,QString userName,QString UserId,QWidget *parent = nullptr);
    ~MainWindow();

    void updateFile();

public slots:
    void openFile(FileItem* item);
    void dataProcess();
    void uploadFile(QString,int);
private slots:
    void on_toolButton_10_clicked();
    void indexClicked(FileItem*);
    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();

    void updateProgressVal(int,int);
    void deleteFromDownloadArea(int);
    void transmitFinished(int,QString);
    void transmitFinished(int);

    void deleteFile(FileItem*);
    void downloadFile(FileItem*);

    void on_toolButton_14_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    FileItem* root;

    QVector<FileItem*> f_items;
    QHash<int,FileItem*> dir;
    QHash<int,FileItem*> showItemMap;
    QHash<int,FileItem*> HideItemMap;
    QHash<int,DownloadItem*> downloadItemMap;

    QHash<int,DownloadFileThread*> downloadThreadMap;

    QThreadPool* pool = QThreadPool::globalInstance();

    FileItem* curDir;
    void backBtn();

    QString UserId;
    QWidget* filepage;
};

#endif // MAINWINDOW_H
