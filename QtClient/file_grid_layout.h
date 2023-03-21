#ifndef FILE_GRID_LAYOUT_H
#define FILE_GRID_LAYOUT_H

#include<QGridLayout>

class file_grid_layout : public QGridLayout
{
    Q_OBJECT
public:
    file_grid_layout(QWidget* parent = nullptr);
    void addFileItem(QWidget* item);
private:
    int m_max_column_count;
    int itemSize = 70;
};

#endif // FILE_GRID_LAYOUT_H
