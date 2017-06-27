#ifndef ORDERLIST_H
#define ORDERLIST_H

#include <QListWidget>

class OrderList : public QListWidget
{
    Q_OBJECT
public:
    OrderList(QWidget *parent = 0);
    ~OrderList();
private:
    void mousePressEvent(QMouseEvent *event);
signals:
    void rightClick(QPoint* pos);
};

#endif // ORDERLIST_H
