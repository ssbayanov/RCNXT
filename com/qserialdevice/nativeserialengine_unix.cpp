/*
* This file is part of QSerialDevice, an open-source cross-platform library
* Copyright (C) 2009  Denis Shienkov
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#include <QtCore/QDir>
#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QCoreApplication>

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>          //is defined open and close
#include <unistd.h>         //is defined read and write
#include <sys/types.h>
#include <signal.h>

#include "nativeserialengine.h"

#define NATIVESERIALENGINE_UNIX_DEBUG

#ifdef NATIVESERIALENGINE_UNIX_DEBUG
#include <QtCore/QDebug>
#endif


NativeSerialEnginePrivate::NativeSerialEnginePrivate()
    : fd(-1), readNotifier(0)
{
}

bool NativeSerialEnginePrivate::isValid() const
{
    return (fd != -1);
}

bool NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate)
{
    if (fd == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    speed_t POSIXbaudRate = 0;
    switch (baudRate) {
        case AbstractSerial::BaudRate14400:
        case AbstractSerial::BaudRate56000:
        case AbstractSerial::BaudRate76800:
        case AbstractSerial::BaudRate128000:
        case AbstractSerial::BaudRate256000:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> POSIX does not support baudRate: " << baudRate << " baud operation. Error! \n";
#endif
            return false;
        case AbstractSerial::BaudRate50: POSIXbaudRate = B50; break;
        case AbstractSerial::BaudRate75: POSIXbaudRate = B75; break;
        case AbstractSerial::BaudRate110: POSIXbaudRate = B110; break;
        case AbstractSerial::BaudRate134: POSIXbaudRate = B134; break;
        case AbstractSerial::BaudRate150: POSIXbaudRate = B150; break;
        case AbstractSerial::BaudRate200: POSIXbaudRate = B200; break;
        case AbstractSerial::BaudRate300: POSIXbaudRate = B300; break;
        case AbstractSerial::BaudRate600: POSIXbaudRate = B600; break;
        case AbstractSerial::BaudRate1200: POSIXbaudRate = B1200; break;
        case AbstractSerial::BaudRate1800: POSIXbaudRate = B1800; break;
        case AbstractSerial::BaudRate2400: POSIXbaudRate = B2400; break;
        case AbstractSerial::BaudRate4800: POSIXbaudRate = B4800; break;
        case AbstractSerial::BaudRate9600: POSIXbaudRate = B9600; break;
        case AbstractSerial::BaudRate19200: POSIXbaudRate = B19200; break;
        case AbstractSerial::BaudRate38400: POSIXbaudRate = B38400; break;
        case AbstractSerial::BaudRate57600: POSIXbaudRate = B57600; break;
        case AbstractSerial::BaudRate115200: POSIXbaudRate = B115200; break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> baudRate: " << baudRate << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::BaudRate. Error! \n";
#endif
            return false;
    }//switch baudrate
    if ((::cfsetispeed(&tio, POSIXbaudRate) == -1) || (::cfsetospeed(&tio, POSIXbaudRate) == -1) )  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> function: ::cfsetispeed(...) or function: ::cfsetospeed(...) returned: -1. Error! \n";
#endif
        return false;
    }
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
       return false;
    }
    m_baudRate = baudRate; return true;
}

bool NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits)
{
    if (fd == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    if ((dataBits == AbstractSerial::DataBits5) && (m_stopBits == AbstractSerial::StopBits2)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }
    if ((dataBits == AbstractSerial::DataBits6) && (m_stopBits == AbstractSerial::AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
        if ((dataBits == AbstractSerial::DataBits7) && (m_stopBits == AbstractSerial::AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    if ((dataBits == AbstractSerial::DataBits8) && (m_stopBits == AbstractSerial::AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }

    switch (dataBits) {
        case AbstractSerial::DataBits5:
            tio.c_cflag &= (~CSIZE);
            tio.c_cflag |= CS5;
            break;
        case AbstractSerial::DataBits6:
            tio.c_cflag &= (~CSIZE);
            tio.c_cflag |= CS6;
            break;
        case AbstractSerial::DataBits7:
            tio.c_cflag &= (~CSIZE);
            tio.c_cflag |= CS7;
            break;
        case AbstractSerial::DataBits8:
            tio.c_cflag &= (~CSIZE);
            tio.c_cflag |= CS8;
            break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> dataBits: " << dataBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::DataBits. Error! \n";
#endif
            return false;
    }//switch dataBits
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
       return false;
    }
    m_dataBits = dataBits; return true;
}

bool NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    switch (parity) {
        case AbstractSerial::ParitySpace:
            tio.c_cflag &= (~PARODD);
            tio.c_cflag |= CMSPAR;
            break;
        case AbstractSerial::ParityMark:
            tio.c_cflag |= (CMSPAR | PARODD);
            break;
        case AbstractSerial::ParityNone:
            tio.c_cflag &= (~PARENB);
            break;
        case AbstractSerial::ParityEven:
            tio.c_cflag &= (~PARODD);
            tio.c_cflag |= PARENB;
            break;
        case AbstractSerial::ParityOdd:
            tio.c_cflag |= (PARENB | PARODD);
            break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Parity. Error! \n";
#endif
            return false;
    }//switch parity
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
        return false;
    }
    m_parity = parity; return true;
}

bool NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    if ( (m_dataBits == AbstractSerial::DataBits5) && (m_stopBits == AbstractSerial::StopBits2) ) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }
    if (m_stopBits == AbstractSerial::StopBits1_5) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 1.5 stop bit operation is not supported by POSIX. Error! \n");
#endif
        return false;
    }
    switch (stopBits) {
        case AbstractSerial::StopBits1: tio.c_cflag &= (~CSTOPB); break;
        case AbstractSerial::StopBits2: tio.c_cflag |= CSTOPB; break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> stopBits: " << stopBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::StopBits. Error! \n";
#endif
            return false;
    }//switch
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
        return false;
    }
    m_stopBits = stopBits; return true;
}

bool NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    switch (flow) {
        case AbstractSerial::FlowControlOff:
            tio.c_cflag &= (~CRTSCTS);
            tio.c_iflag &= ( ~(IXON | IXOFF |IXANY ) );
            break;
        case AbstractSerial::FlowControlXonXoff:
            tio.c_cflag &= (~CRTSCTS);
            tio.c_iflag |= (IXON | IXOFF | IXANY);
            break;
        case AbstractSerial::FlowControlHardware:
            tio.c_cflag |= CRTSCTS;
            tio.c_iflag &= ( ~(IXON | IXOFF | IXANY) );
            break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> flow: " << flow << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Flow. Error! \n";
#endif
            return false;
    }//switch
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
        return false;
    }
    m_flow = flow; return true;
}

bool NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    if (::tcgetattr(fd, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs) \n"
                " -> function: ::tcgetattr(fd, &tio) returned: -1. Error! \n";
#endif
        return false;
    }
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 0;
    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
        return false;
    }
    m_charIntervalTimeout = msecs; return true;
}


/* Returned lock file name from serial device name
*/
static QString getLockFileName(const QString &deviceName)
{
    if (deviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when get lockFileName(const QString &deviceName) \n"
                " -> input deviceName is empty. Error! \n";
#endif
        return QString(0);
    }

    return QDir::cleanPath(deviceName).section( QDir::separator() , -1 ).prepend("/var/lock/LCK..");
}

/*
*/
static bool checkDeviceLocked(const QString &lockFileName)
{
    QFile lockFile(lockFileName);

    if (lockFile.exists()) {
        if (!lockFile.open(QIODevice::ReadOnly)) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when check lock device  \n"
                " -> in function: static bool checkDeviceLocked(const QString &lockFileName) \n"
                "lockFile: " << lockFileName << " unable to open. Error! \n";
#endif
            return true;
        }

        QString lockFileContent(lockFile.readAll());
        lockFile.close();

        int pid = lockFileContent.section( ' ', 0, 0, QString::SectionSkipEmpty ).toInt();

        if ((::kill(pid_t(pid), 0) == -1) && (errno == ESRCH))
            return true;
    }
    return false;
}

/*
*/
static bool unlockDevice(const QString &lockFileName)
{
    if (checkDeviceLocked(lockFileName)) {
        QFile lockFile(lockFileName);
        if (!lockFile.remove()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when unlock device \n"
                " -> in function: static bool unlockDevice(const QString &lockFileName) \n"
                "lockFile: " << lockFileName << " is not removed. Error! \n";
#endif
            return false;
        }
    }
    return true;
}

/*
*/
static bool lockDevice(const QString &lockFileName)
{
    if (!unlockDevice(lockFileName)) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when lock device \n"
                " -> in function: static bool lockDevice(const QString &lockFileName) \n"
                "function: unlockDevice(lockFileName) returned: false. Error! \n";
#endif
        return false;
    }

    int fd_lock_file = ::open(lockFileName.toAscii(),
                                       O_WRONLY | O_CREAT | O_EXCL,
                                       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );

    if (fd_lock_file == -1)  {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when lock device \n"
                " -> in function: static bool lockDevice(const QString &lockFileName) \n"
                "when open lock file function: ::open(...) returned -1. Error! \n";
#endif
        return false;
    }

    QString writeStr = "     %1 %2 %3\x0A";
    writeStr = writeStr.arg(::getpid()).arg(QCoreApplication::applicationName()).arg(::getuid()); //TODO: QCoreApplication::applicationName() ?

    if (::write(fd_lock_file, writeStr.toAscii(), writeStr.length()) == -1) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: when unlock device \n"
                " -> in function: static bool lockDevice(const QString &lockFileName) \n"
                "function: ::write(...) returned -1. Error! \n";
#endif
            return false;
    }
    ::close (fd_lock_file);
    return (fd_lock_file != -1);
}

bool NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode)
{
    QString lockFileName = getLockFileName(m_deviceName);

    if (!lockDevice(lockFileName)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: lockDevice(lockFileName) returned: false. Error! \n";
#endif
        return false;
    }

    if (fd != -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> The device is already open or other error. Error! \n";
#endif
        return false;
    }

    int flags = 0;
    ::memset(&oldtio, 0,  sizeof(oldtio));
    ::memset(&tio, 0,  sizeof(tio));

    switch (mode & (~QIODevice::Unbuffered)) {
        case QIODevice::ReadOnly:
            flags = (O_RDONLY | O_NOCTTY | O_NDELAY);
            break;
        case QIODevice::WriteOnly:
            flags = (O_WRONLY | O_NOCTTY | O_NDELAY);
            break;
        case QIODevice::ReadWrite:
            flags = (O_RDWR | O_NOCTTY | O_NDELAY);
            break;
        default:
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> mode: " << mode << " undefined. Error! \n";
#endif
            return false;
    }//switch
    //открываем последовательное устройство (opened serial device)
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> trying to open device: " << m_deviceName << " \n";
#endif
    fd = ::open((const char *)m_deviceName.toLatin1(), flags);
    if (fd == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::open(...) returned: " << fd << ". Error! \n";
#endif
        return false;
    }
    if (!nativeReset()) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: nativeReset() returned: false. Error! \n";
#endif
        return false;
    }
    //сохраняем текущие установки устройства в oldtio
    if (::tcgetattr(fd, &oldtio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::tcgetattr(fd, &oldtio) returned: -1. Error! \n";
#endif
        return false;
    }

    m_oldSettingsIsSaved = true;

    memcpy(&tio, &oldtio, sizeof(oldtio));

    tio.c_line = 0;
    //режимы управления
/*

       CBR  (not in POSIX) Baud speed mask (4+1 bits).  [requires _BSD_SOURCE or _SVID_SOURCE]
       CBREX
              (not in POSIX) Extra baud speed mask (1 bit), included in CBR.  [requires _BSD_SOURCE or _SVID_SOURCE]
              (POSIX says that the baud speed is stored in the termios structure without specifying where precisely, and  provides  cfgetispeed()
              and  cfsetispeed()  for  getting at it.  Some systems use bits selected by CBR in c_cflag, other systems use separate fields, for
              example, sg_ispeed and sg_ospeed.)
       CSIZE  Character size mask.  Values are CS5, CS6, CS7, or CS8.
       CSTOPB Set two stop bits, rather than one.
       CREAD  Enable receiver.
       PARENB Enable parity generation on output and parity checking for input.
       PARODD If set, then parity for input and output is odd; otherwise even parity is used.
       HUPCL  Lower modem control lines after last process closes the device (hang up).
       CLOCAL Ignore modem control lines.
       LOBLK  (not in POSIX) Block output from a non-current shell layer.  For use by shl (shell layers).  (Not implemented on Linux.)
       CIBR (not in POSIX) Mask for input speeds.  The values for the CIBR bits are the same as the values for the CBR bits,  shifted  left
              IBSHIFT bits.  [requires _BSD_SOURCE or _SVID_SOURCE] (Not implemented on Linux.)
       CMSPAR (not in POSIX) Use "stick" (mark/space) parity (supported on certain serial devices): if PARODD is set, the parity bit is always 1;
              if PARODD is not set, then the parity bit is always 0).  [requires _BSD_SOURCE or _SVID_SOURCE]
       CRTSCTS
              (not in POSIX) Enable RTS/CTS (hardware) flow control.  [requires _BSD_SOURCE or _SVID_SOURCE]
*/
    tio.c_cflag |= (CREAD|CLOCAL);
    tio.c_cflag |= (~HUPCL);
    //локальные режимы
/*

       ISIG   When any of the characters INTR, QUIT, SUSP, or DSUSP are received, generate the corresponding signal.
       ICANON Enable canonical mode (described below).
       XCASE  (not in POSIX; not supported under Linux) If ICANON is also set, terminal is uppercase only.   Input  is  converted  to  lowercase,
              except  for  characters preceded by \.  On output, uppercase characters are preceded by \ and lowercase characters are converted to
              uppercase.  [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
       ECHO   Echo input characters.
       ECHOE  If ICANON is also set, the ERASE character erases the preceding input character, and WERASE erases the preceding word.
       ECHOK  If ICANON is also set, the KILL character erases the current line.
       ECHONL If ICANON is also set, echo the NL character even if ECHO is not set.
       ECHOCTL
              (not in POSIX) If ECHO is also set, ASCII control signals other than TAB, NL, START, and STOP are echoed as  ^X,  where  X  is  the
              character  with  ASCII  code  0x40  greater  than the control signal.  For example, character 0x08 (BS) is echoed as ^H.  [requires
              _BSD_SOURCE or _SVID_SOURCE]
       ECHOPRT
              (not in POSIX) If ICANON and IECHO are also set, characters are printed  as  they  are  being  erased.   [requires  _BSD_SOURCE  or
              _SVID_SOURCE]
       ECHOKE (not  in  POSIX)  If  ICANON  is also set, KILL is echoed by erasing each character on the line, as specified by ECHOE and ECHOPRT.
              [requires _BSD_SOURCE or _SVID_SOURCE]
       DEFECHO
              (not in POSIX) Echo only when a process is reading.  (Not implemented on Linux.)
       FLUSHO (not in POSIX; not supported under Linux) Output is being  flushed.   This  flag  is  toggled  by  typing  the  DISCARD  character.
              [requires _BSD_SOURCE or _SVID_SOURCE]
       NOFLSH Disable flushing the input and output queues when generating the SIGINT, SIGQUIT, and SIGSUSP signals.
       TOSTOP Send the SIGTTOU signal to the process group of a background process which tries to write to its controlling terminal.
       PENDIN (not  in  POSIX;  not  supported  under  Linux)  All  characters  in the input queue are reprinted when the next character is read.
              (bash(1) handles typeahead this way.)  [requires _BSD_SOURCE or _SVID_SOURCE]
       IEXTEN Enable implementation-defined input processing.  This flag, as well as ICANON must be enabled for   the  special  characters  EOL2,
              LNEXT, REPRINT, WERASE to be interpreted, and for the IUCLC flag to be effective.
*/
    tio.c_lflag &= ( ~(ICANON | ECHO | ISIG) );
    tio.c_lflag |= (IEXTEN);
        //входные режимы
/*

       IGNBRK Ignore BREAK condition on input.
       BRKINT If IGNBRK is set, a BREAK is ignored.  If it is not set but BRKINT is set, then a BREAK causes the input and output  queues  to  be
              flushed,  and  if the terminal is the controlling terminal of a foreground process group, it will cause a SIGINT to be sent to this
              foreground process group.  When neither IGNBRK nor BRKINT are set, a BREAK reads as a 0 byte ('\0'), except when PARMRK is  set,
              in which case it reads as the sequence \377 \0 \0.
       IGNPAR Ignore framing errors and parity errors.
       PARMRK If  IGNPAR  is not set, prefix a character with a parity error or framing error with \377 \0.  If neither IGNPAR nor PARMRK is set,
              read a character with a parity error or framing error as \0.
       INPCK  Enable input parity checking.
       ISTRIP Strip off eighth bit.
       INLCR  Translate NL to CR on input.
       IGNCR  Ignore carriage return on input.
       ICRNL  Translate carriage return to newline on input (unless IGNCR is set).
       IUCLC  (not in POSIX) Map uppercase characters to lowercase on input.
       IXON   Enable XON/XOFF flow control on output.
       IXANY  (XSI) Typing any character will restart stopped output.  (The default is to allow just the START character to restart output.)
       IXOFF  Enable XON/XOFF flow control on input.
*/
    tio.c_iflag &= ( ~(IGNPAR | PARMRK | ISTRIP | ICRNL | IGNCR | INLCR) );
    tio.c_iflag |= (IGNBRK | INPCK | IXANY);
    //выходные режимы
/*

       OPOST  Enable implementation-defined output processing.
       The remaining c_oflag flag constants are defined in POSIX.1-2001, unless marked otherwise.
       OLCUC  (not in POSIX) Map lowercase characters to uppercase on output.
       ONLCR  (XSI) Map NL to CR-NL on output.
       OCRNL  Map CR to NL on output.
       ONOCR  Don't output CR at column 0.
       ONLRET Don't output CR.
       OFILL  Send fill characters for a delay, rather than using a timed delay.
       OFDEL  (not in POSIX) Fill character is ASCII DEL (0177).  If unset, fill character is ASCII NUL ('\0').  (Not implemented on Linux.)
       NLDLY  Newline delay mask.  Values are NL0 and NL1.  [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
       CRDLY  Carriage return delay mask.  Values are CR0, CR1, CR2, or CR3.  [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
       TABDLY Horizontal tab delay mask.  Values are TAB0, TAB1, TAB2, TAB3 (or XTABS).  A value of TAB3, that is, XTABS, expands tabs to  spaces
              (with tab stops every eight columns).  [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
       BSDLY  Backspace  delay  mask.   Values  are  BS0  or  BS1.   (Has  never  been  implemented.)   [requires  _BSD_SOURCE or _SVID_SOURCE or
              _XOPEN_SOURCE]
       VTDLY  Vertical tab delay mask.  Values are VT0 or VT1.
       FFDLY  Form feed delay mask.  Values are FF0 or FF1.  [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
*/
    tio.c_oflag &= ( ~(ONLCR | OCRNL | ONLRET | OFILL) );
    tio.c_oflag |= (OPOST | ONOCR);
        //управляющие символы
/*

       VINTR  (003,  ETX,  Ctrl-C,  or also 0177, DEL, rubout) Interrupt character.  Send a SIGINT signal.  Recognized when ISIG is set, and then
              not passed as input.
       VQUIT  (034, FS, Ctrl-\) Quit character.  Send SIGQUIT signal.  Recognized when ISIG is set, and then not passed as input.
       VERASE (0177, DEL, rubout, or 010, BS, Ctrl-H, or also #) Erase character.  This erases the previous not-yet-erased  character,  but  does
              not erase past EOF or beginning-of-line.  Recognized when ICANON is set, and then not passed as input.
       VKILL  (025,  NAK,  Ctrl-U,  or Ctrl-X, or also @) Kill character.  This erases the input since the last EOF or beginning-of-line.  Recog‐
              nized when ICANON is set, and then not passed as input.
       VEOF   (004, EOT, Ctrl-D) End-of-file character.  More precisely: this character causes the pending tty buffer to be sent to  the  waiting
              user program without waiting for end-of-line.  If it is the first character of the line, the read(2) in the user program returns 0,
              which signifies end-of-file.  Recognized when ICANON is set, and then not passed as input.
       VMIN   Minimum number of characters for non-canonical read.
       VEOL   (0, NUL) Additional end-of-line character.  Recognized when ICANON is set.
       VTIME  Timeout in deciseconds for non-canonical read.
       VEOL2  (not in POSIX; 0, NUL) Yet another end-of-line character.  Recognized when ICANON is set.
       VSWTCH (not in POSIX; not supported under Linux; 0, NUL) Switch character.  (Used by shl only.)
       VSTART (021, DC1, Ctrl-Q) Start character.  Restarts output stopped by the Stop character.  Recognized when IXON  is  set,  and  then  not
              passed as input.
       VSTOP  (023,  DC3,  Ctrl-S) Stop character.  Stop output until Start character typed.  Recognized when IXON is set, and then not passed as
              input.
       VSUSP  (032, SUB, Ctrl-Z) Suspend character.  Send SIGTSTP signal.  Recognized when ISIG is set, and then not passed as input.
       VDSUSP (not in POSIX; not supported under Linux; 031, EM, Ctrl-Y) Delayed suspend character: send SIGTSTP signal  when  the  character  is
              read  by  the  user  program.  Recognized when IEXTEN and ISIG are set, and the system supports job control, and then not passed as
              input.
       VLNEXT (not in POSIX; 026, SYN, Ctrl-V) Literal next.  Quotes the next input character, depriving it of a possible special meaning.   Rec‐
              ognized when IEXTEN is set, and then not passed as input.
       VWERASE
              (not in POSIX; 027, ETB, Ctrl-W) Word erase.  Recognized when ICANON and IEXTEN are set, and then not passed as input.
       VREPRINT
              (not  in  POSIX;  022,  DC2,  Ctrl-R) Reprint unread characters.  Recognized when ICANON and IEXTEN are set, and then not passed as
              input.
       VDISCARD
              (not in POSIX; not supported under Linux; 017, SI, Ctrl-O) Toggle: start/stop discarding pending output.  Recognized when IEXTEN is
              set, and then not passed as input.
       VSTATUS
              (not in POSIX; not supported under Linux; status request: 024, DC4, Ctrl-T).
*/
        /*
        tio.c_cc[VINTR]=_POSIX_VDISABLE;
        tio.c_cc[VQUIT]=_POSIX_VDISABLE;
        tio.c_cc[VSTART]=_POSIX_VDISABLE;
        tio.c_cc[VSTOP]=_POSIX_VDISABLE;
        tio.c_cc[VSUSP]=_POSIX_VDISABLE;
        */
    if ( !(nativeSetBaudRate(m_baudRate) &&
        nativeSetDataBits(m_dataBits) &&
        nativeSetParity(m_parity) &&
        nativeSetStopBits(m_stopBits) &&
        nativeSetFlowControl(m_flow) &&
        nativeSetCharIntervalTimeout(m_charIntervalTimeout)) ) {

#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> set default parameters - fail. Error! \n";
#endif
        return false;
    }//if set parameters

    if (::tcsetattr(fd, TCSANOW, &tio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::tcsetattr(fd, TCSANOW, &tio) returned: -1. Error! \n";
#endif
        return false;
    }//if set all param
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> opened device: " << m_deviceName << " in mode: " << mode << " succesfully. Ok! \n";
#endif
    return true;
}

bool NativeSerialEnginePrivate::nativeClose()
{
    QString lockFileName = getLockFileName(m_deviceName);

    if (!unlockDevice(lockFileName)) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeClose() \n"
                " -> function: unlockDevice(lockFileName) returned: false. Error! \n";
#endif
    }

    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeClose() \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    bool okClosed = true;
    //восстанавливаем старые сохраненные параметры устройства
    if (m_oldSettingsIsSaved)
        if (::tcsetattr(fd, TCSANOW, &oldtio) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeClose() \n"
            " -> function: ::tcsetattr(fd, TCSANOW, &oldtio) returned: -1. Error! \n");
#endif
            okClosed = false;
        }
    if (::close(fd) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeClose() \n"
            " -> function: ::close(fd) returned: -1. Error! \n");
#endif
        okClosed = false;
    }
    fd = -1;
    return okClosed;
}

bool NativeSerialEnginePrivate::nativeFlush()
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeFlush() \n"
            "-> fd == -1. Error! \n");
#endif
        return false;
    }
    if (::tcdrain(fd) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeFlush() \n"
            " -> function: ::tcdrain(fd) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeReset()
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeReset() \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }
    if (::tcflush(fd, TCIOFLUSH) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeReset() \n"
            " -> function: ::tcflush(fd, TCIOFLUSH) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeSetDtr(bool set)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    int status = 0;
    if (::ioctl(fd, TIOCMGET, &status) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
            " -> function: ::ioctl(fd, TIOCMGET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    (set) ? status |= TIOCM_DTR : status &= (~TIOCM_DTR);
    if (::ioctl(fd, TIOCMSET, &status) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
            " -> function: ::ioctl(fd, TIOCMSET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeSetRts(bool set)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
            " -> fd == -1. Error! \n");
#endif
        return false;
    }

    int status = 0;
    if (::ioctl(fd, TIOCMGET, &status) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
            " -> function: ::ioctl(fd, TIOCMGET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    (set) ? status |= TIOCM_RTS : status &= (~TIOCM_RTS);
    if (::ioctl(fd, TIOCMSET, &status) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
            " -> function: ::ioctl(fd, TIOCMSET, &status) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

ulong NativeSerialEnginePrivate::nativeLineStatus()
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeLineStatus() \n"
            " -> fd == -1. Error! \n");
#endif
        return 0;
    }

    ulong status = 0, temp = 0;
    if (::ioctl(fd, TIOCMGET, &temp) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeLineStatus() \n"
            " -> function: ::ioctl(fd, TIOCMGET, &temp) returned: -1. Error! \n");
#endif
        return status;
    }
    if (temp&TIOCM_CTS) status |= AbstractSerial::lsCTS;
    if (temp&TIOCM_DSR) status |= AbstractSerial::lsDSR;
    if (temp&TIOCM_RI) status |= AbstractSerial::lsRI;
    if (temp&TIOCM_CD) status |= AbstractSerial::lsDCD;
    if (temp&TIOCM_DTR) status |= AbstractSerial::lsDTR;
    if (temp&TIOCM_RTS) status |= AbstractSerial::lsRTS;
    if (temp&TIOCM_ST) status |= AbstractSerial::lsST;
    if (temp&TIOCM_SR) status |= AbstractSerial::lsSR;
    return status;
}

qint64 NativeSerialEnginePrivate::nativeBytesAvailable(bool wait)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
            " -> fd == -1. Error! \n");
#endif
        return 0;
    }

    size_t nbytes = 0;

    for(;;) {
        if (::ioctl(fd, FIONREAD, &nbytes) == -1) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
            " -> function: ::ioctl(fd, FIONREAD, &nbytes) returned: -1. Error! \n");
#endif
            return (qint64)-1;
        }
        if (!wait) break;
        if (nbytes > 0) break;
        if (!nativeSelect(m_charIntervalTimeout, true)) break;
    }
    return (qint64)nbytes;
}

qint64 NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
            " -> fd == -1. Error! \n");
#endif
        return (qint64)-1;
    }

    tcflush(fd,TCOFLUSH);
    ssize_t bytesWritten = ::write(fd, (const char *)data, (size_t)len);
    tcdrain(fd);

    if (bytesWritten < 0) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) write fail: \n"
                " - bytes written, bytesWritten: " << bytesWritten << " bytes \n"
                " - error code,           errno: " << errno << " \n. Error! \n";
#endif
        return (qint64)-1;
    }

    if (bytesWritten == len)
        return (quint64)bytesWritten;
    else {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) error: \n"
                " - query for write(send) is : "  << len << " bytes \n"
                " - actually write(send) is  : "  << bytesWritten << " bytes \n";
#endif
    }
    return (qint64)-1;
}

qint64 NativeSerialEnginePrivate::nativeRead(char *data, qint64 len)
{
    if (fd == -1)  {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug("Linux: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
            " -> fd == -1. Error! \n");
#endif
        return (qint64)-1;
    }

    ssize_t bytesReaded = ::read(fd, (void*)data, (size_t)len);

    if (bytesReaded < 0) {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n -> read fail: \n"
                " - bytes readed, bytesReaded: " << bytesReaded << " bytes \n"
                " - error code,         errno: " << errno << " \n. Error! \n";
#endif
        return (qint64)-1;
    }

    if (bytesReaded == len)
        return (quint64)bytesReaded;
    else {
#if defined (NATIVESERIALENGINE_UNIX_DEBUG)
    qDebug() << "Linux: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) error: \n"
                " - query for reading is : "  << len << " bytes \n"
                " - actually read is     : "  << bytesReaded << " bytes \n";
#endif
    }
    return (qint64)-1;
}

int NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    struct timeval tv;
    tv.tv_sec = (timeout / 1000);
    tv.tv_usec = (timeout % 1000) * 1000;

    if (selectForRead)
        return ::select(fd + 1, &fds, 0, 0, (timeout < 0) ? 0 : &tv);
    else
        return ::select(fd + 1, 0, &fds, 0, (timeout < 0) ? 0 : &tv);
}

int NativeSerialEnginePrivate::nativeSelect(int timeout,
                                            bool checkRead, bool checkWrite,
                                            bool *selectForRead, bool *selectForWrite) const
{
    fd_set fdread;
    FD_ZERO(&fdread);
    if (checkRead)
        FD_SET(fd, &fdread);

    fd_set fdwrite;
    FD_ZERO(&fdwrite);
    if (checkWrite)
        FD_SET(fd, &fdwrite);

    struct timeval tv;
    tv.tv_sec = (timeout / 1000);
    tv.tv_usec = (timeout % 1000) * 1000;

    int ret = ::select(fd + 1, &fdread, &fdwrite, 0, (timeout < 0) ? 0 : &tv);
    //int ret = ::select(fd + 1, &fdread, 0, 0, timeout < 0 ? 0 : &tv);

    if (ret <= 0)
        return ret;

    *selectForRead = FD_ISSET(fd, &fdread);
    *selectForWrite = FD_ISSET(fd, &fdwrite);

    return ret;
}

//added 06.11.2009
qint64 NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) const
{
    Q_UNUSED(Queue)
    /* not supported */
    return 0;
}

//------------------------------------------------------------------------------------------//

bool NativeSerialEngine::isReadNotificationEnabled() const
{
    Q_D(const NativeSerialEngine);
    return d->readNotifier && d->readNotifier->isEnabled();
}

void NativeSerialEngine::setReadNotificationEnabled(bool enable)
{
    Q_D(NativeSerialEngine);
    if (d->readNotifier) {
        d->readNotifier->setEnabled(enable);
    } else if (enable && QAbstractEventDispatcher::instance(thread())) {
        d->readNotifier = new QSocketNotifier(d->fd, QSocketNotifier::Read, this);
        QObject::connect(d->readNotifier, SIGNAL(activated(int)),
                            this, SIGNAL(readNotification()));

        d->readNotifier->setEnabled(true);
    }
}
