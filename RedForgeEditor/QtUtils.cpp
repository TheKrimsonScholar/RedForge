#include "QtUtils.h"

void clearLayout(QLayout* layout)
{
    if(!layout || layout->isEmpty())
        return;
    
    while(QLayoutItem* item = layout->takeAt(0))
    {
        if(item->layout())
        {
            clearLayout(item->layout());
            delete item->layout();
        }
        if(item->widget())
            delete item->widget();

        delete item;
    }
}