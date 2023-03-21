#include "attribute.h"
#include "ui_attribute.h"

Attribute::Attribute(File mfile,QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Attribute)
{
    ui->setupUi(this);
    file = mfile;
    ui->directory->adjustSize();   //自适应大小
    ui->directory->setWordWrap(true); //自动换行

    if(file.getType()!=FileType::DIR)
    {
        ui->label_4->hide();
        ui->dir_incude->hide();
    }

    ui->lineEdit->setText(file.getName());

    switch (file.getType()) {
    case FileType::DIR:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_folder@2x.png"));
        QString str = QString::number(file.getFileNum()) + "个文件（" + QString::number(file.getDirNum()) + "个文件夹）";
        ui->dir_incude->setText(str);
        ui->file_type->setText("文件夹");
        break;
    }
    case FileType::TXT:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_text@2x.png"));
        ui->file_type->setText("文本文件");
        break;
    }
    case FileType::MP4:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_video@2x.png"));
        ui->file_type->setText("视频");
        break;
    }
    case FileType::PNG:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_image@2x.png"));
        ui->file_type->setText("图片");
        break;
    }
    case FileType::ZIP:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_zip@2x.png"));
        ui->file_type->setText("压缩文件");
        break;
    }
    case FileType::UNKNOWN:
    {
        ui->icon->setPixmap(QPixmap("://images/files/file_unknwon@2x.png"));
        ui->file_type->setText("未知文件");
        break;
    }
    }
    QString str;
    int size = file.getSize();
    if(size<1024*1024)
    {
        str = QString::number(size/1024.0) + " KB(" + QString::number(size) + " 字节)";
    }
    else {
        str = QString::number(size/(1024*1024.0)) + " MB(" + QString::number(size) + " 字节)";
    }
    ui->size->setText(str);
    ui->uploadTime->setText(file.getUploadTime());
    ui->directory->setText(file.getDirectory());
}

Attribute::~Attribute()
{
    delete ui;
}

void Attribute::on_pushButton_clicked()
{

}

void Attribute::on_pushButton_2_clicked()
{
    this->close();
}
