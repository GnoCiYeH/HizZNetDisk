#ifndef SELECTFILEDIALOG_H
#define SELECTFILEDIALOG_H

#include <QDialog>
#include<QVector>
#include"fileitem.h"

namespace Ui {
class SelectFileDialog;
}

class SelectFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectFileDialog(QHash<int,FileItem*> dir,FileItem* root,QWidget *parent = nullptr);
    ~SelectFileDialog();

signals:
    void uplodeInfo(QString,int);

private slots:
    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SelectFileDialog *ui;
    QHash<int,FileItem*> f_items;
    FileItem* root;
    int dirId = 0;
};

#endif // SELECTFILEDIALOG_H
