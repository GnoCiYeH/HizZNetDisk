#ifndef FILEINDEXLABLE_H
#define FILEINDEXLABLE_H

#include <QObject>
#include<QLabel>
#include"fileitem.h"
#include<QHBoxLayout>
class FileIndexLable : public QLabel
{
    Q_OBJECT
public:
    FileIndexLable(FileItem* file_item,FileIndexLable* parLable,QWidget* parent = nullptr);
    FileIndexLable* getParentLable(){return parentLable;}
    FileItem* getItem(){return item;}
signals:
    void clicked(FileItem*);

private:
    FileItem* item;
    FileIndexLable* parentLable;
    void mouseReleaseEvent(QMouseEvent *)override;
    bool event(QEvent* event)override;
};

class FileIndexWidget : public QWidget
{
    Q_OBJECT
public:
    FileIndexWidget(QWidget* parent);

    void addLable(FileItem* item);

    void deleteLable();

public slots:
    void itemClicked(FileItem* item);
signals:
    void clicked(FileItem*);
private:
    QHBoxLayout* layout;
    FileIndexLable* last;
    int count = 0;
};

#endif // FILEINDEXLABLE_H
