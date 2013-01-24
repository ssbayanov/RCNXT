/* Reader
*
* This application is part of the examples on the use of the library QSerialDevice.
*
* reader - a test console application to read data from the port using the method of expectations waitForReadyRead()
*
* Copyright (C) 2009  Denis Shienkov
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#include <QtCore>
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    AbstractSerial *MyDevice = new AbstractSerial();

    if (!MyDevice) {
        qDebug() << "Create object is fail!";
        return 0;
    }

    char dn[50]; //device name
    cout << "Please enter serial device name, specific by OS, \n example: in Windows -> COMn, in GNU/Linux -> /dev/ttyXYZn: ";
    cin >> dn;

    MyDevice->setDeviceName(dn);

    /*!
        \warning
        \~english To annex "correctly" to work - need to open the device with the flag Unbuffered!
        \~russian Чтобы приложение "корректно" работало - необходимо открывать устройство с флагом Unbuffered!
    */
    if (MyDevice->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        qDebug() << "Serial device " << MyDevice->deviceName() << " open in " << MyDevice->openMode();

        qDebug() << "= Defaults parameters =";
        qDebug() << "Device name            : " << MyDevice->deviceName();
        qDebug() << "Baud rate              : " << MyDevice->baudRate();
        qDebug() << "Data bits              : " << MyDevice->dataBits();
        qDebug() << "Parity                 : " << MyDevice->parity();
        qDebug() << "Stop bits              : " << MyDevice->stopBits();
        qDebug() << "Flow                   : " << MyDevice->flowControl();
        qDebug() << "Char timeout, msec     : " << MyDevice->charIntervalTimeout();

        int rrto = 0; //timeout for ready read
        cout << "Please enter wait timeout for ready read, msec : ";
        cin >> rrto;

        int len = 0; //len data for read
        cout << "Please enter len data for read, bytes : ";
        cin >> len;

        qDebug() << "Enter is " << rrto << " msecs, " << len << " bytes";

        qDebug() << "Starting waiting ready read in time : " << QTime::currentTime();

        QByteArray ba; //received data

        while (1) {
            if (MyDevice->waitForReadyRead(rrto)) {
                ba.clear();
                ba = MyDevice->read(len);

                cout << "Rx : \n";
                printDataToHex(ba);

                qDebug() << " \n Readed is : " << ba.size() << " bytes";
            }
            else {
                qDebug() << "Timeout read data in time : " << QTime::currentTime();
            }
        }//while
    }
    else {
        qDebug() << "Error opened serial device " << MyDevice->deviceName();
    }
    MyDevice->close();
    qDebug() << "Serial device " << MyDevice->deviceName() << " is closed";
    delete MyDevice;
    MyDevice = 0;
    return app.exec();
}
