#ifndef NEWFOLDERDIALOG_H
#define NEWFOLDERDIALOG_H

#include <QDialog>
#include"fileitem.h"

namespace Ui {
class NewFolderDialog;
}

class NewFolderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFolderDialog(FileItem* root,QHash<int,FileItem*> items,QWidget *parent = nullptr);
    ~NewFolderDialog();
    int getParID(){return par_id;}
    QString getName(){return name;}
    bool isRight(){return isright;}

signals:
    void newFolderInfo(int,QString);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::NewFolderDialog *ui;
    bool isright = true;
    int par_id = 0;
    QString name;
    FileItem * root;
    QHash<int,FileItem*> f_items;
};

#endif // NEWFOLDERDIALOG_H
