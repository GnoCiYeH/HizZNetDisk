#ifndef FILEINDEXLIST_H
#define FILEINDEXLIST_H

#include <QDialog>
#include"fileitem.h"
#include<QVector>
#include<QTreeWidget>
namespace Ui {
class FileIndexList;
}

class FileIndexList : public QDialog
{
    Q_OBJECT

public:
    explicit FileIndexList(FileItem* root, QHash<int,FileItem*> dir,QWidget *parent = nullptr);
    void addList(FileItem* root,QHash<int,FileItem*> dir,QTreeWidgetItem* TreeRoot);
    ~FileIndexList();

    int getSelectId(){return selectedId;}

public slots:
    void item_clicked(QTreeWidgetItem*,int);

private:
    Ui::FileIndexList *ui;

    int selectedId = 0;
};

#endif // FILEINDEXLIST_H
