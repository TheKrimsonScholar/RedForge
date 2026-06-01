#include "QtUtils.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

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