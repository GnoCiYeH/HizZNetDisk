#include "newfolderdialog.h"
#include "ui_newfolderdialog.h"
#include"fileindexlist.h"

NewFolderDialog::NewFolderDialog(FileItem* root,QHash<int,FileItem*> items,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFolderDialog)
{
    ui->setupUi(this);
    this->root = root;
    this->f_items=items;
}

NewFolderDialog::~NewFolderDialog()
{
    delete ui;
}

void NewFolderDialog::on_pushButton_clicked()
{
    FileIndexList list(root,f_items,this);
    list.exec();

    QString path = "";
    int fd = list.getSelectId();
    FileItem* item = f_items.value(fd);
    path.append(">"+item->getFile()->getName().left(item->getFile()->getName().lastIndexOf('.')));
    while(item!=root)
    {
        QString str = item->getParent()->getFile()->getName();
        path.push_front(">"+str.left(str.lastIndexOf('.')));
        item = item->getParent();
    }

    ui->path->setText(path);
    par_id = list.getSelectId();
}

void NewFolderDialog::on_pushButton_2_clicked()
{
    name = ui->lineEdit->text();
    if(name.size()==0)
    {
        ui->lineEdit->setPlaceholderText("请输入新建文件名");
        isright = false;
        return;
    }
    isright = true;
    this->close();
}

void NewFolderDialog::on_pushButton_3_clicked()
{
    isright = false;
    this->close();
}
