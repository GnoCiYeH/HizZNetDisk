#include "fileindexlist.h"
#include "ui_fileindexlist.h"

FileIndexList::FileIndexList(FileItem* root,QHash<int,FileItem*> dir,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileIndexList)
{
    ui->setupUi(this);

    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(item_clicked(QTreeWidgetItem*,int)));

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,root->getFile()->getName());
    ui->treeWidget->addTopLevelItem(item);
    addList(root,dir,item);
}

FileIndexList::~FileIndexList()
{
    delete ui;
}

void FileIndexList::item_clicked(QTreeWidgetItem* item,int n)
{
    QVariant var = item->data(n,Qt::UserRole+1);
    selectedId = var.value<int>();
}

void FileIndexList::addList(FileItem* root,QHash<int,FileItem*> dir,QTreeWidgetItem* treeRoot)
{
    for(auto i : *root->getChildern())
    {
        if(dir.contains(i->getFile()->getFID()))
        {
            QTreeWidgetItem * item = new QTreeWidgetItem(treeRoot);
            item->setText(0,i->getFile()->getName().left(i->getFile()->getName().lastIndexOf('.')));
            item->setData(0,Qt::UserRole+1,QVariant::fromValue(i->getFile()->getFID()));
            addList(i,dir,item);
        }
    }
}
