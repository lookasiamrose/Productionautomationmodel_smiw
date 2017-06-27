#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <uart.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void com_ports();

    void menuSelection(QAction*);
    void baudSelection(QAction*);
    void on_lineEdit_textChanged(const QString &arg1);

    void onRightClick(QPoint *pos);

private:
    Ui::MainWindow *ui;
    Uart* uart;
    QAction* newAct;
    QList<QAction*> actionPorts;
    QMenu *fileMenu;
    QMenu *subMenu;
    QMenu *baudMenu;
};

#endif // MAINWINDOW_H
