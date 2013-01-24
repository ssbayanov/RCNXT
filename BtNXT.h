#ifndef BTNXT_H
#define BTNXT_H

#include <com/qserialdevice/abstractserial.h>

#ifdef Q_OS_WIN
#define PORT "COM%1"
#define WIN 1
#else
#define PORT "/dev/rfcomm%1"
#define WIN 0
#endif

#define A 1
#define B 7
#define C 10

#define Port_A 0x00
#define Port_B 0x01
#define Port_C 0x02
#define Port_ALL 0xFF

class powerMotor
{
public:
    void setPower(int port, int power = 100);
    int power(int port);
private:
    short ports[3];
};

class BtNXT : public QObject
{
    Q_OBJECT

protected:
    AbstractSerial *port;

private:
    QByteArray getMessage(int len, int start, char cntrl);

public:
    BtNXT();

    float getBatt(); //возращает значенеи баттареи в
    int getUSonic(int port);
    int getSensValue(int port);
    long getTacho(int port, bool relative);

    bool connected(); //возвращает 1 если подключено 0 если нет
    bool setCon(QString p); //устанавливает соединение к указанному порту

    bool setDiscon(); //разрывает соединение
    void resetmotor(int port, int relative); //сбросс счётчика оборотов

    void sendMessage(QByteArray msg);
    QString getName();

public slots:
    void setSensType(int type, short num);
    void run(short command = B+C, short power = 100); //включат указанные моторы на указанную мощность
    void motor(short num, short power); //отправляет команду на включение мотора
    void setName(QString name); //установить заданное имя
};

#endif
