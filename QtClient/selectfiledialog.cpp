#include "selectfiledialog.h"
#include "ui_selectfiledialog.h"
#include<QFileDialog>
#include"fileindexlist.h"

SelectFileDialog::SelectFileDialog(QHash<int,FileItem*> dir,FileItem* root,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectFileDialog)
{
    ui->setupUi(this);
    this->f_items = dir;
    this->root = root;
}

SelectFileDialog::~SelectFileDialog()
{
    delete ui;
}

void SelectFileDialog::on_toolButton_clicked()
{
    QFileDialog fileDialog(this);
    QString fileName = fileDialog.getOpenFileName();
    ui->lineEdit->setText(fileName);
}

void SelectFileDialog::on_toolButton_2_clicked()
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

    ui->lineEdit_2->setText(path);
    dirId = list.getSelectId();
}

void SelectFileDialog::on_buttonBox_accepted()
{
    emit uplodeInfo(ui->lineEdit->text(),dirId);
}
