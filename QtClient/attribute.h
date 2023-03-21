#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QDialog>
#include"fileitem.h"

namespace Ui {
class Attribute;
}

class Attribute : public QDialog
{
    Q_OBJECT

public:
    explicit Attribute(File file,QDialog *parent = nullptr);
    ~Attribute();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Attribute *ui;
    File file;
};

#endif // ATTRIBUTE_H
