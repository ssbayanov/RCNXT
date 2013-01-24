#ifndef SREADER_H
#define SREADER_H

#include <QtCore>
#include <QDebug>
#include <QByteArray>

#include <abstractserial.h>
#include <iostream>

using namespace std;

static void printDataToHex(const QByteArray &data)
{
    QByteArray baTmp;
    baTmp.clear();
#if QT_VERSION >= 0x040300
    baTmp = (data.toHex()).toUpper();
#else
    quint8 n = 0;
    for (int i=0;i<data.size();i++) {
        n = data.at(i);
        if ((n >= 0) && (n <= 15)) baTmp.append(QByteArray::number(0, 16).toUpper());
        baTmp.append(QByteArray::number(n, 16).toUpper());
    }
#endif

    for (int i=0;i<baTmp.size();i+=2) {
        cout << "[" << baTmp.at(i) << baTmp.at(i + 1) << "]";
    }
    cout << endl;
}

class Reader : public QObject
{
    Q_OBJECT

public:
    Reader(QObject *parent = 0)
        : QObject(parent) {
        MyDevice = new AbstractSerial();
        connect(MyDevice, SIGNAL(readyRead()), this, SLOT(slotRead()));

        if (!MyDevice->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
            qDebug() << "Serial device by default: " << MyDevice->deviceName() << " open fail.";
            return;
        }

        if (!MyDevice->setBaudRate(AbstractSerial::BaudRate115200)) {
            qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate115200 << " error.";
            return;
        };

        readBytes = 0;
        cout << "Please enter len data for read, bytes : ";
        cin >> readBytes;

        qDebug() << "= Defaults parameters =";
        qDebug() << "Device name            : " << MyDevice->deviceName();
        qDebug() << "Baud rate              : " << MyDevice->baudRate();
        qDebug() << "Data bits              : " << MyDevice->dataBits();
        qDebug() << "Parity                 : " << MyDevice->parity();
        qDebug() << "Stop bits              : " << MyDevice->stopBits();
        qDebug() << "Flow                   : " << MyDevice->flowControl();
        qDebug() << "Char timeout, msec     : " << MyDevice->charIntervalTimeout();
    }
    ~Reader() {
        MyDevice->close();
        delete MyDevice;
        MyDevice = 0;
    }
private slots:
    void slotRead() {
        QByteArray ba = MyDevice->read(readBytes);
        cout << "Rx : \n";
        printDataToHex(ba);
        qDebug() << " \n Readed is : " << ba.size() << " bytes";
    }
private:
    AbstractSerial *MyDevice;
    int readBytes;
};

#endif
