#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QActionGroup>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    uart = new Uart();
    QList<qint32> bauds = uart->getStandardBaudRates();
    if(!bauds.isEmpty())
        uart->setBaudRate(bauds.at(0));

    newAct = new QAction(tr("&Ports information"), this);
    newAct->setStatusTip(tr("Show connected COM ports"));
    connect(newAct, &QAction::triggered, this, &MainWindow::com_ports);
    fileMenu = menuBar()->addMenu(tr("&Menu"));
    fileMenu->addAction(newAct);
    subMenu = new QMenu("Ports");
    baudMenu = new QMenu("Baud rates");
    fileMenu->addMenu(subMenu);
    fileMenu->addMenu(baudMenu);

    QList<QString> coms = uart->allCOMPorts();
    foreach(QString port , coms){
        actionPorts.append(new QAction(port));
    }

    QActionGroup* portGroup = new QActionGroup(this);
    foreach (QAction* action, actionPorts) {
        portGroup->addAction(action);
        action->setCheckable(true);
        subMenu->addAction(action);
    }

    if(actionPorts.isEmpty()){
        QAction* emptyAction = new QAction("No ports available");
        subMenu->addAction(emptyAction);
    }

    QActionGroup* baudGroup = new QActionGroup(this);
    foreach(qint32 one , bauds)
    {
        QString stringBaud;
        stringBaud = QString::number(one);
        QAction *singleBaud = new QAction(stringBaud);
        baudGroup->addAction(singleBaud);
        singleBaud->setCheckable(true);
        baudMenu->addAction(singleBaud);
    }

    connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuSelection(QAction*)));
    connect(baudMenu, SIGNAL(triggered(QAction*)), this, SLOT(baudSelection(QAction*)));
    connect(ui->listWidget,SIGNAL(rightClick(QPoint*)),this,SLOT(onRightClick(QPoint*)));

    ui->listWidget->setDragDropMode(QAbstractItemView::InternalMove);

    ui->label_2->setStyleSheet("QLabel { background-color : darkMagenta; color : white; }");
}
void MainWindow::baudSelection(QAction* baud)
{
    uart->setBaudRate(baud->text().toInt());
    if(!uart->getPortName().isEmpty()) uart->reopenPort();
}
void MainWindow::menuSelection(QAction* item)
{
    uart->setPortName(item->text());
    uart->reopenPort();
}
void MainWindow::com_ports()
{
    QList<QString> list = uart->checkAvailablePorts();
    foreach(QString port , list){
        QMessageBox::information(this,"Ports",port,QMessageBox::Ok);
    }
    if(list.isEmpty()){
        QMessageBox::warning(this,"Ports","No COM ports detected");
    }
}
MainWindow::~MainWindow()
{
    delete uart;
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
    QListWidgetItem* item = new QListWidgetItem(ui->lineEdit->text(), ui->listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->lineEdit->clear();
}
void MainWindow::on_pushButton_2_clicked()
{
    char c = ' ';
    for(int i=0;i<ui->listWidget->count();i++){
        QListWidgetItem* item = ui->listWidget->item(i);
        if( item->checkState() ==  Qt::Checked) {
            if(item->backgroundColor() == Qt::darkMagenta)
            {
                uart->writeNumberTwoByteFromString(item->text());
            }
            else
                {
                    if( (!item->text().isEmpty()) && (!item->text().isNull()) ) {
                        for(int pos=0;pos<item->text().size();pos++)
                        {
                            c = item->text().at(pos).toLatin1();
                            uart->writeChar(c);
                        }
                    }
                }
        }
    }
}
void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_clicked();
}
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item->backgroundColor() == Qt::darkMagenta)
    {
        item->setTextColor(Qt::black);
        item->setBackgroundColor(Qt::transparent);
    }
    else
    {
        item->setTextColor(Qt::white);
        item->setBackgroundColor(Qt::darkMagenta);
    }
}
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.size() > 0)
    {
        //char c = arg1.at(0).toLatin1();
        //if( (c > 47) && (c < 58) ){
            //if(arg1.length() > 1) ui->lineEdit->setText(arg1.right(1));
        //}else
            //if(arg1.length() > 1) ui->lineEdit->setText(arg1.right(1));
    }
}
void MainWindow::onRightClick(QPoint *pos)
{
    QListWidgetItem* item = ui->listWidget->itemAt(*pos);
    item = ui->listWidget->takeItem(ui->listWidget->row(item));
    delete item;
}
