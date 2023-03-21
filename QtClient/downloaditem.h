#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QWidget>
#include<QProgressBar>

namespace Ui {
class DownloadItem;
}

class DownloadItem : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadItem(QWidget *parent = nullptr);
    ~DownloadItem();

    QProgressBar * getProgressBar();
    void setId(int id){this->id = id;}
    void finished();
    bool isFinished(){return m_finished;}

signals:
    void itemClear(int);
private slots:
    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

private:
    Ui::DownloadItem *ui;
    bool m_finished = false;
    bool m_pause = false;
    int id;
};

#endif // DOWNLOADITEM_H
