#include "orderlist.h"
#include <QMouseEvent>

OrderList::OrderList(QWidget *parent) : QListWidget(parent)
{

}

OrderList::~OrderList()
{

}
void OrderList::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        emit rightClick(&event->pos());
    } else {
        QListWidget::mousePressEvent(event);
    }
}
