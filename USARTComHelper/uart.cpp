#include "uart.h"

Uart::Uart(QObject *parent) : QObject(parent)
{
    port = nullptr;
}
Uart::~Uart()
{
    if( (port != nullptr) && (port->isOpen()) ) port->close();

    delete port;
}
void Uart::writeChar(char char_arg)
{
    if( (port != nullptr) && port->isOpen())
    {
        QByteArray to_send;
        to_send.append(char_arg);

        port->write(to_send,1);
        port->waitForBytesWritten(-1);
        to_send.clear();
    }
}
bool Uart::openPort(QString port_name, int baud_rate)
{
    bool is_open = false;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.portName() == port_name)
        {
            port = new QSerialPort(this);
            port->setPort(serialPortInfo);
            port->setBaudRate(baud_rate);
            port->open(QIODevice::ReadWrite);

            is_open = true;
            break;
        }
    }
    return is_open;
}
QList<QString> Uart::checkAvailablePorts()
{
    QList<QString> ports_info;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
       QString port_info;
       port_info = "Port: " + serialPortInfo.portName();
       port_info += "\n Location: " + serialPortInfo.systemLocation();
       port_info += "\n Description: " + serialPortInfo.description();
       port_info += "\n Manufacturer: " + serialPortInfo.manufacturer();
       port_info += "\n Vendor Identifier: " + (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : QByteArray());
       port_info += "\n Product Identifier:" + (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : QByteArray());
       port_info += "\n Busy:" + (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));
       port_info += "\n";
       QSerialPort *port = new QSerialPort(serialPortInfo);
       if (port->open(QIODevice::ReadWrite))
       {
           port_info += "\n Baud rate: " + port->baudRate();
           port_info += "\n Data bits: " + port->dataBits();
           port_info += "\n Stop bits: " + port->stopBits();
           port_info += "\n Parity: " + port->parity();
           port_info += "\n Flow control: " + port->flowControl();
           port_info += "\n Read buffer size: " + port->readBufferSize();
           port->close();
       } else
           {
               port_info += "\nUnable to open port, error code " + port->error();
           }
       delete port;

       ports_info.append(port_info);
    }
    return ports_info;
}
void Uart::writeNumberTwoByteFromString(QString number_arg)
{
    quint16 number = number_arg.toInt(); //lets take the 16 bits representation of string
    quint8  number_a = 0xff & number; //clear first 8 bits with mask to be sure
    quint8  number_b = 0xff & (number>>8); //clear second 8 bits with mask to be sure

    char a = number_a; //char representation
    char b = number_b;

    this->writeChar(a);
    if(number>255) this->writeChar(b);
}
QList<QString> Uart::allCOMPorts()
{
    QList<QString> list;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
       list.append(serialPortInfo.portName());
    }
    return list;
}
QString Uart::getPortName()
{
    return portName;
}
void Uart::setPortName(QString portNameArg)
{
    portName = portNameArg;
}
qint32 Uart::getBaudRate()
{
    return baudRate;
}
void Uart::setBaudRate(qint32 baudRateArg)
{
    baudRate = baudRateArg;
}
bool Uart::reopenPort()
{
    if( (port !=nullptr) && (port->isOpen()) ) port->close();
    bool isOpen = false;
    isOpen = this->openPort(portName,baudRate);
    return isOpen;
}
QList<qint32> Uart::getStandardBaudRates()
{
    return QSerialPortInfo::standardBaudRates();
}

