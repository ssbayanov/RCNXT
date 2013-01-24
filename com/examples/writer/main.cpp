/* Writer
*
* This application is part of the examples on the use of the library QSerialDevice.
*
* writer - a test console application to write data to the port
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
    quint8 n=0;
    for (int i=0;i<data.size();i++) {
        n=data.at(i);
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
    if (MyDevice->open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {

        if (!MyDevice->setBaudRate(AbstractSerial::BaudRate115200)) {
            qDebug() << "Set baud rate " <<  AbstractSerial::BaudRate115200 << " error.";
            return 0;
        };

        qDebug() << "Serial device " << MyDevice->deviceName() << " open in " << MyDevice->openMode();

        qDebug() << "= Defaults parameters =";
        qDebug() << "Device name            : " << MyDevice->deviceName();
        qDebug() << "Baud rate              : " << MyDevice->baudRate();
        qDebug() << "Data bits              : " << MyDevice->dataBits();
        qDebug() << "Parity                 : " << MyDevice->parity();
        qDebug() << "Stop bits              : " << MyDevice->stopBits();
        qDebug() << "Flow                   : " << MyDevice->flowControl();
        qDebug() << "Char timeout, msec     : " << MyDevice->charIntervalTimeout();

        QByteArray ba; //data to send
        qint64 bw = 0; //bytes really writed

        while (1) {
            bw = 0;
            cout << "Please enter count bytes for wtitten : ";
            cin >> bw;

            qDebug() << "Starting writting " << bw << " bytes in time : " << QTime::currentTime();

            ba.clear();
            ba.resize(bw);
            for (int i=0;i<bw;i++) { //filling data array
                ba[i] = i;
            }
            bw = MyDevice->write(ba);

            if (bw > 0) {
                cout << "Tx : \n";
                printDataToHex(ba);
            }
            qDebug() << "\n Writed is : " << bw << " bytes";
        }//
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
