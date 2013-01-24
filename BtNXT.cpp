#include "BtNXT.h"

BtNXT::BtNXT()
{
    port = new AbstractSerial();
}

bool BtNXT::setCon(QString p)
{
    if(!port->isOpen()){
        port->setDeviceName(p);
        if (port->open(AbstractSerial::ReadWrite)) {
            port->setBaudRate(AbstractSerial::BaudRate19200);
            port->setDataBits(AbstractSerial::DataBits8);
            //port->setTotalReadConstantTimeout(5000);
            port->setCharIntervalTimeout(400);
            return 1;}}
    return port->isOpen();
}


bool BtNXT::setDiscon()
{
    if(port->isOpen()){
        port->close();}
    return (!port->isOpen());
}

QString BtNXT::getName()
{
    QByteArray name;
    name.resize(2);
    name[0] = 0x01;
    name[1] = 0x9B;
    sendMessage(name);
    QByteArray gname;
    gname.resize(15);
    for(int i = 0; i < 15; i++){
        gname[i] = 0x00;}
    gname = getMessage(15,3,(int)name[1]);
    if (gname.size() == 0)
        return 0;
    return gname.data();
}

void BtNXT::sendMessage(QByteArray msg)
{
    if(connected()){
        QByteArray msgLen;
        msgLen.resize(2);
        msgLen[0] = msg.size();
        msgLen[1] = 0x00;
        port->write(msgLen);
        port->write(msg);
        port->waitForBytesWritten(800);}
}

void BtNXT::motor(short num, short power)
{
    QByteArray motor;
    motor.resize(13);
    motor[0] = 0x80;
    motor[1] = 0x04;
    motor[2] = num;
    motor[3] = power;
    motor[4] = 0x01;
    motor[5] = 0x00;
    motor[6] = 0x64;
    motor[7] = 0x20;
    motor[8] = 0x00;
    motor[9] = 0x00;
    motor[10] = 0x00;
    motor[11] = 0x00;
    motor[12] = 0x00;
    sendMessage(motor);
}

void BtNXT::run(short command, short power)
{
    switch (command) {
    case A: motor(Port_A, power); break;
    case B: motor(Port_B, power); break;
    case C: motor(Port_C, power); break;
    case -A: motor(Port_A, power); break;
    case -B: motor(Port_B, power); break;
    case -C: motor(Port_C, power); break;
    case A+B: motor(Port_A, power); motor(Port_B, power); break;
    case C+A: motor(Port_C, power); motor(Port_A, power); break;
    case B+C: motor(Port_B, power); motor(Port_C, power); break;
    case A-B: motor(Port_A, power); motor(Port_B, -power); break;
    case B-A: motor(Port_A, -power); motor(Port_B, power); break;
    case C-A: motor(Port_C, power); motor(Port_A, -power); break;
    case A-C: motor(Port_C, -power); motor(Port_A, power); break;
    case B-C: motor(Port_B, power); motor(Port_C, -power); break;
    case C-B: motor(Port_B, -power); motor(Port_C, power); break;
    case -A-B-C: motor(Port_ALL, -power); break;
    case -A-B+C: motor(Port_B, -power); motor(Port_C, -power);motor(Port_C, power); break;
    case -A+B-C: motor(Port_B, -power); motor(Port_C, power);motor(Port_C, -power); break;
    case -A+B+C: motor(Port_B, -power); motor(Port_C, power);motor(Port_C, power); break;
    case +A-B-C: motor(Port_B, power); motor(Port_C, -power);motor(Port_C, -power); break;
    case +A-B+C: motor(Port_B, power); motor(Port_C, -power);motor(Port_C, power); break;
    case +A+B-C: motor(Port_B, power); motor(Port_C, power);motor(Port_C, -power); break;
    case +A+B+C: motor(Port_ALL, power); break;}
}

QByteArray BtNXT::getMessage(int len, int start, char cntrl)
{
    QByteArray buffer;
    buffer.resize(2);
    QByteArray ret;
    ret.resize(len);
    for(int i = 0; i < 2; i++)
        buffer[i] = 0x00;

    port->waitForReadyRead(5000);
    buffer = port->read(2);

    int leng = (int)buffer[0];
    buffer.resize(leng);

    buffer = port->read(leng);

    for(int i = 0; i < len; i++)
        ret[i] = 0x00;

    if(buffer[1] == cntrl){
        for(int i = start; i < len+start; i++){
            ret[i-start] = buffer[i];}

    }
    else {
        ret.resize(0);
    }
    return ret;
}

float BtNXT::getBatt()
{
    float ret = 0;
    if(connected()){
        QByteArray batt;
        batt.resize(2);
        batt[0] = 0x00;
        batt[1] = 0x0B;
        sendMessage(batt);
        batt = getMessage(2,3,(int) 0x0B);


        if (batt.size() != 0)
        {
            unsigned short result;
            memcpy(&result,batt.data(),batt.size());
            ret = result/1000.00;
        }}
    return ret;
}

void BtNXT::setName(QString name)
{
    if(connected()){
        QByteArray setname;
        setname.resize(18);
        setname[0] = 0x81;
        setname[1] = 0x98;
        uint i;

        for(i = 0; i < name.toStdString().length() && i < 15;i++)
            setname[i+2] = name.toStdString().c_str()[i];

        for(;i < 15; i++)
            setname[i+2] = 0x00;
        sendMessage(setname);
        getMessage(3,0,(int) 0x98);}
}


void BtNXT::resetmotor(int port,int relative = 1)
{
    QByteArray reset;
    reset.resize(4);
    reset[0] = 0x80;
    reset[1] = 0x0A;
    reset[2] = port;
    reset[3] = relative;
    sendMessage(reset);
}

long int BtNXT::getTacho(int port, bool relative = 1)
{
    QByteArray gett;
    gett.resize(3);
    gett[0] = 0x00;
    gett[1] = 0x06;
    gett[2] = port;
    sendMessage(gett);
    gett.resize(4);
    long int result;
    if (relative)
        gett = getMessage(4,21,(int) 0x06);
    else
        gett = getMessage(4,17,(int) 0x06);
    if (gett.size() != 0)
    {

        memcpy(&result,gett.data(),gett.size());

    }
    else
        return -1;
    return result;
}

int BtNXT::getSensValue(int port)
{
    unsigned short result = 0;
    QByteArray sensValue;
    sensValue.resize(3);
    sensValue[0] = 0x00;
    sensValue[1] = 0x07;
    sensValue[2] = port;

    sendMessage(sensValue);

    sensValue = getMessage(2,12,(int) 0x07);

    if (sensValue.size() != 0)
    {
        memcpy(&result,sensValue.data(),sensValue.size());
    }
    return result;
}

int BtNXT::getUSonic(int port)
{
    unsigned short result = 0;
    if(connected()){
        QByteArray us;
        /*       us.resize(2);
        us[0] = 0x81;
        us[1] = 0x88;
        sendMessage(us);*/

        us.resize(7);
        us[0] = 0x80;
        us[1] = 0x0F;
        us[2] = port;
        us[3] = 0x02;
        us[4] = 0x01;
        us[5] = 0x02;
        us[6] = 0x42;
        sendMessage(us);

        QByteArray sensValue;
        sensValue.resize(2);
        sensValue[0] = 0x00;
        sensValue[1] = 0x10;
        sensValue[2] = port;

        sendMessage(sensValue);

        sensValue = getMessage(2,3,0x10);

        if (sensValue.size() != 0)
        {
            memcpy(&result,sensValue.data(),sensValue.size());
        }
        else
            return -1;}
    return result;
}

bool BtNXT::connected()
{
    return port->isOpen();
}
void BtNXT::setSensType(int type, short num)
{
    if(connected()){
        QByteArray imputMode;
        imputMode.resize(5);
        imputMode[0] = 0x80;
        imputMode[1] = 0x05;
        imputMode[2] = num;


        switch(type)
        {
        case 0:
            imputMode[3] = 0x00;
            imputMode[4] = 0x00;
            sendMessage(imputMode);
            break;
        case 1:
            imputMode[3] = 0x01;
            imputMode[4] = 0x20;
            sendMessage(imputMode);
            break;
        case 2: break;
        case 3: break;
        case 4:
            /*    if(count>3)
    {
    type++;
    if(type == (int) 0x0D){
   type=0;
        mode++;}*/


            imputMode[3] = 0x0B;
            imputMode[4] = 0x00;
            sendMessage(imputMode);
            //    count=0;

            break;

        case 5: break;
        }}
}
