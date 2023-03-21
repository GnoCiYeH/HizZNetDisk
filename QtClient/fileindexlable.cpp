#include "fileindexlable.h"
#include<QHBoxLayout>

FileIndexLable::FileIndexLable(FileItem* file_item,FileIndexLable* parLable,QWidget* parent) : QLabel (parent)
{
    this->item = file_item;
    this->parentLable = parLable;

    QString str = item->getFile()->getName();
    QString temp = str.left(str.lastIndexOf('.'));
    this->setText(temp.append('>'));

    QPalette palette;
    palette.setColor(QPalette::Background,QColor(220,220,220));
    this->setAutoFillBackground(false);
    this->setPalette(palette);
}
void FileIndexLable::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(this->item);
}

bool FileIndexLable::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
    {
        this->setAutoFillBackground(true);
        break;
    }
    case QEvent::Leave:
    {
        QPalette palette;
        this->setAutoFillBackground(false);
        break;
    }
    default:break;
    }
    return QLabel::event(event);
}

FileIndexWidget::FileIndexWidget(QWidget* parent) : QWidget (parent)
{
    QLabel* lable = new QLabel(">",this);
    layout = new QHBoxLayout(this);
    layout->addWidget(lable);
    this->setLayout(layout);
}

void FileIndexWidget::addLable(FileItem* item)
{
    FileIndexLable* lable = new FileIndexLable(item,last,this);
    connect(lable,SIGNAL(clicked(FileItem*)),this,SLOT(itemClicked(FileItem*)));
    layout->addWidget(lable);
    last = lable;
    count++;
}

void FileIndexWidget::deleteLable()
{
    if(count==0)
        return;
    FileIndexLable* newLast = last->getParentLable();
    last->deleteLater();
    last = newLast;
    count--;
}

void FileIndexWidget::itemClicked(FileItem* item)
{
    emit clicked(item);
    while (this->last->getItem()!=item) {
        deleteLable();
    }
}
