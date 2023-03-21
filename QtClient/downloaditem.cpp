#include "downloaditem.h"
#include "ui_downloaditem.h"

DownloadItem::DownloadItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadItem)
{
    ui->setupUi(this);

    QPixmap *pixmap = new QPixmap("://images/svg/ok.svg");
    pixmap->scaled(ui->oklabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->oklabel->setScaledContents(true);
    ui->oklabel->setPixmap(*pixmap);
    ui->oklabel->setHidden(true);

}

DownloadItem::~DownloadItem()
{
    delete ui;
}

QProgressBar* DownloadItem::getProgressBar()
{
    return ui->progressBar;
}

void DownloadItem::finished()
{
    ui->oklabel->setHidden(false);
    ui->toolButton->setHidden(true);
    m_finished = true;
}

void DownloadItem::on_toolButton_clicked()
{
    switch (m_pause) {
    case false:
    {
        m_pause=true;
        ui->toolButton->setIcon(QIcon("://images/svg/play.svg"));
        break;
    }
    case true:
    {
        m_pause=false;
        ui->toolButton->setIcon(QIcon("://images/svg/pause.svg"));
        break;
    }
    }
}

void DownloadItem::on_toolButton_2_clicked()
{
    emit itemClear(id);
}
