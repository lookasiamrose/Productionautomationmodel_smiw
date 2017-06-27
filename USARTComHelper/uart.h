#ifndef UART_H
#define UART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class Uart : public QObject
{
    Q_OBJECT
private:
    QSerialPort *port;
    QString portName;
    qint32 baudRate;
public:
    explicit Uart(QObject *parent = 0);
    ~Uart();
    void writeChar(char char_arg);
    void writeNumberTwoByteFromString(QString number_arg);
    bool openPort(QString port_name, int baud_rate);
    QList<qint32> getStandardBaudRates();
    QList<QString> checkAvailablePorts();
    QList<QString> allCOMPorts();

    QString getPortName();
    void setPortName(QString portNameArg);
    qint32 getBaudRate();
    void setBaudRate(qint32 baudRateArg);
    bool reopenPort();
signals:

public slots:
};

#endif // UART_H
