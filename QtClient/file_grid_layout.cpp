#include "file_grid_layout.h"
#include<QWidget>

file_grid_layout::file_grid_layout(QWidget* parent) : QGridLayout (parent)
{
    this->m_max_column_count = 8;
}

void file_grid_layout::addFileItem(QWidget* item)
{
    item->setFixedSize(itemSize,itemSize+20);

    int current_row = 0;
    int current_column = 0;

    while( itemAtPosition(current_row, current_column) != nullptr )
    {
        if( current_column == (m_max_column_count-1) )
        {
            current_column = 0;
            ++current_row;
        }
        else
        {
            ++current_column;
        }
    }

    QGridLayout::addWidget(item, current_row, current_column);
}
