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

#include <QtCore/QAbstractEventDispatcher>

#include <qt_windows.h>

#include "nativeserialengine.h"


#define NATIVESERIALENGINE_WIN_DEBUG

#ifdef NATIVESERIALENGINE_WIN_DEBUG
#include <QtCore/QDebug>
#endif


NativeSerialEnginePrivate::NativeSerialEnginePrivate()
    : hd(INVALID_HANDLE_VALUE), readNotifier(0)
{
}

bool NativeSerialEnginePrivate::isValid() const
{
    return (hd != INVALID_HANDLE_VALUE);
}

bool NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate)
{

    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    switch (baudRate) {
        case AbstractSerial::BaudRate50:
        case AbstractSerial::BaudRate75:
        case AbstractSerial::BaudRate134:
        case AbstractSerial::BaudRate150:
        case AbstractSerial::BaudRate200:
        case AbstractSerial::BaudRate1800:
        case AbstractSerial::BaudRate76800:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> windows does not support" << baudRate << "baud operation. Error! \n";
#endif
            return false;
        case AbstractSerial::BaudRate110: cc.dcb.BaudRate = CBR_110; break;
        case AbstractSerial::BaudRate300: cc.dcb.BaudRate = CBR_300; break;
        case AbstractSerial::BaudRate600: cc.dcb.BaudRate = CBR_600; break;
        case AbstractSerial::BaudRate1200: cc.dcb.BaudRate = CBR_1200; break;
        case AbstractSerial::BaudRate2400: cc.dcb.BaudRate = CBR_2400; break;
        case AbstractSerial::BaudRate4800: cc.dcb.BaudRate = CBR_4800; break;
        case AbstractSerial::BaudRate9600: cc.dcb.BaudRate = CBR_9600; break;
        case AbstractSerial::BaudRate14400: cc.dcb.BaudRate = CBR_14400; break;
        case AbstractSerial::BaudRate19200: cc.dcb.BaudRate = CBR_19200; break;
        case AbstractSerial::BaudRate38400: cc.dcb.BaudRate = CBR_38400; break;
        case AbstractSerial::BaudRate56000: cc.dcb.BaudRate = CBR_56000; break;
        case AbstractSerial::BaudRate57600: cc.dcb.BaudRate = CBR_57600; break;
        case AbstractSerial::BaudRate115200: cc.dcb.BaudRate = CBR_115200; break;
        case AbstractSerial::BaudRate128000: cc.dcb.BaudRate = CBR_128000; break;
        case AbstractSerial::BaudRate256000: cc.dcb.BaudRate = CBR_256000; break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> baudRate: " << baudRate << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::BaudRate. Error! \n";
#endif
                return false;
        }//switch
        if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetBaudRate(AbstractSerial::BaudRate baudRate) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
            return false;
        }
        m_baudRate = baudRate;
        return true;
}

bool NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    if ((dataBits == AbstractSerial::DataBits5) && (m_stopBits == AbstractSerial::StopBits2)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }
    if ((dataBits == AbstractSerial::DataBits6) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    if ((dataBits == AbstractSerial::DataBits7) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    if ((dataBits == AbstractSerial::DataBits8) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
            " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    switch (dataBits) {
        case AbstractSerial::DataBits5: cc.dcb.ByteSize = 5; break;
        case AbstractSerial::DataBits6: cc.dcb.ByteSize = 6; break;
        case AbstractSerial::DataBits7: cc.dcb.ByteSize = 7; break;
        case AbstractSerial::DataBits8: cc.dcb.ByteSize = 8; break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> dataBits: " << dataBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::DataBits. Error! \n";
#endif
            return false;
    }//switch dataBits
    if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetDataBits(AbstractSerial::DataBits dataBits) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
        return false;
    }
    m_dataBits = dataBits;
    return true;
}

bool NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    switch (parity) {
        case AbstractSerial::ParityNone: cc.dcb.Parity = NOPARITY; cc.dcb.fParity = false; break;
        case AbstractSerial::ParitySpace: cc.dcb.Parity = SPACEPARITY; cc.dcb.fParity = true; break;
        case AbstractSerial::ParityMark: cc.dcb.Parity = MARKPARITY; cc.dcb.fParity = true; break;
        case AbstractSerial::ParityEven: cc.dcb.Parity = EVENPARITY; cc.dcb.fParity = true; break;
        case AbstractSerial::ParityOdd: cc.dcb.Parity = ODDPARITY; cc.dcb.fParity = true; break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> parity: " << parity << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Parity. Error! \n";
#endif
            return false;
    }//switch parity
    if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetParity(AbstractSerial::Parity parity) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
        return false;
    }
    m_parity = parity;
    return true;
}

bool NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    if ( (m_dataBits == AbstractSerial::DataBits5) && (m_stopBits == AbstractSerial::StopBits2) ) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 5 data bits cannot be used with 2 stop bits. Error! \n");
#endif
        return false;
    }
    if ((m_dataBits == AbstractSerial::DataBits6) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 6 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    if ((m_dataBits == AbstractSerial::DataBits7) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 7 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    if ((m_dataBits == AbstractSerial::DataBits8) && (m_stopBits == AbstractSerial::StopBits1_5)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
            " -> 8 data bits cannot be used with 1.5 stop bits. Error! \n");
#endif
        return false;
    }
    switch (stopBits) {
        case AbstractSerial::StopBits1: cc.dcb.StopBits = ONESTOPBIT; break;
        case AbstractSerial::StopBits1_5: cc.dcb.StopBits = ONE5STOPBITS; break;
        case AbstractSerial::StopBits2: cc.dcb.StopBits = TWOSTOPBITS; break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> stopBits: " << stopBits << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::StopBits. Error! \n";
#endif
            return false;
    }//switch stopBits
    if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetStopBits(AbstractSerial::StopBits stopBits) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
        return false;
    }
    m_stopBits=stopBits;
    return true;
}

bool NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    switch (flow) {
        case AbstractSerial::FlowControlOff:
            cc.dcb.fOutxCtsFlow = false; cc.dcb.fRtsControl = RTS_CONTROL_DISABLE;
            cc.dcb.fInX = false; cc.dcb.fOutX = false; break;
        case AbstractSerial::FlowControlXonXoff:
            cc.dcb.fOutxCtsFlow = false; cc.dcb.fRtsControl = RTS_CONTROL_DISABLE;
            cc.dcb.fInX = true; cc.dcb.fOutX = true; break;
        case AbstractSerial::FlowControlHardware:
            cc.dcb.fOutxCtsFlow = true; cc.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
            cc.dcb.fInX = false; cc.dcb.fOutX = false; break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> flow: " << flow << " is not supported by the class NativeSerialEnginePrivate, \n"
                "see enum AbstractSerial::Flow. Error! \n";
#endif
            return false;
    }//switch flow
    if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetFlowControl(AbstractSerial::Flow flow) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
        return false;
    }
    m_flow = flow;
    return true;
}

bool NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    /* тут нужно задать настройки для АСИНХРОННОГО режима! нужно проверить!
    (here you need to specify the settings for the asynchronous mode! to check!) */
    ct.ReadIntervalTimeout = MAXDWORD;
    ct.ReadTotalTimeoutMultiplier = 0;
    ct.ReadTotalTimeoutConstant = 0;
    ct.WriteTotalTimeoutMultiplier = 0;
    ct.WriteTotalTimeoutConstant = 0;
    if (::SetCommTimeouts(hd, &ct) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSetCharIntervalTimeout(int msecs) \n"
                " -> function: ::SetCommTimeouts(hd, &ct) returned: 0. Error! \n";
#endif
        return false;
    }
    m_charIntervalTimeout = msecs;
    return true;
}

bool NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode)
{
    if (hd != INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> the device is already open or other error. Error!";
#endif
        return false;
    }

    DWORD access = 0;
    DWORD sharing = 0;
    DWORD confSize = sizeof(COMMCONFIG);
    ::ZeroMemory(&cc, confSize);
    ::ZeroMemory(&oldcc, confSize);
    cc.dwSize = confSize;
    oldcc.dwSize = confSize;

    switch (mode & (~QIODevice::Unbuffered)) {
        case QIODevice::ReadOnly:
            access = GENERIC_READ; sharing = FILE_SHARE_READ;
            break;
        case QIODevice::WriteOnly:
            access = GENERIC_WRITE; sharing = FILE_SHARE_WRITE;
            break;
        case QIODevice::ReadWrite:
            access = GENERIC_READ | GENERIC_WRITE; sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
            break;
        default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> mode: " << mode << " undefined. Error! \n";
#endif
            return false;
    }

    //открываем последовательное устройство (opened serial device)
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> trying to open device: " << m_deviceName << "\n";
#endif

    // Create the file handle.
    QString sDeviceName = "\\\\.\\" + m_deviceName;
    QByteArray nativeDeviceName;

    QT_WA({
        nativeDeviceName = QByteArray((const char *)sDeviceName.utf16(), sDeviceName.size() * 2 + 1);
    }, {
        nativeDeviceName.append(sDeviceName);
    });

    QT_WA({
        hd = ::CreateFileW((TCHAR *)nativeDeviceName.constData(),
                           access, sharing,
                           0, OPEN_EXISTING,
                           FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                           0);
    }, {
        hd = ::CreateFileA(nativeDeviceName.constData(),
                           access, sharing,
                           0, OPEN_EXISTING,
                           FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                           0);
    });

    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::CreateFile(...) returned: " << hd << ". Error! \n";
#endif
        return false;
    }

    if (!nativeReset()) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: nativeReset() returned: false. Error! \n";
#endif
        return false;
    }

    //сохраняем текущие установки устройства в oldtio (saved current parameters device)
    if (::GetCommConfig(hd, &oldcc, &confSize) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::GetCommConfig(hd, &oldcc, &confSize) returned: 0. Error! \n";
#endif
        return false;
    }

    //выставляем флаг того, что успешно сохранили старые настройки устройства до его конфигурирования
    m_oldSettingsIsSaved = true;// set flag "altered parameters is saved"

    //приступаем к конфигурированию открытого устройства (get configure)
    memcpy(&cc, &oldcc, sizeof(COMMCONFIG));

    cc.dcb.fBinary = true;
    cc.dcb.fInX = false;
    cc.dcb.fOutX = false;
    cc.dcb.fAbortOnError = false;
    cc.dcb.fNull = false;

    if ( !(nativeSetBaudRate(m_baudRate) &&
        nativeSetDataBits(m_dataBits) &&
        nativeSetParity(m_parity) &&
        nativeSetStopBits(m_stopBits) &&
        nativeSetFlowControl(m_flow) &&
        nativeSetCharIntervalTimeout(m_charIntervalTimeout)) ) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> set(initialise) by default parameters - fail. Error! \n";
#endif
        return false;
    }//if set parameters

    if (::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> function: ::SetCommConfig(hd, &cc, sizeof(COMMCONFIG)) returned: 0. Error! \n";
#endif
        return false;
    }////if set all param
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeOpen(QIODevice::OpenMode mode) \n"
                " -> opened device: " << m_deviceName << " in mode: " << mode << " succesfully. Ok! \n";
#endif
    return true;
}

bool NativeSerialEnginePrivate::nativeClose()
{
    if ( hd == INVALID_HANDLE_VALUE ) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeClose() \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    bool okClosed = true;
    //восстанавливаем старые сохраненные параметры устройства (repair altered parameters device)
    if (m_oldSettingsIsSaved)
        if (::SetCommConfig(hd, &oldcc, sizeof(COMMCONFIG)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeClose() \n"
            " -> function: ::SetCommConfig(hd, &oldcc, sizeof(COMMCONFIG)) returned: 0. Error! \n");
#endif
            okClosed = false;
        }
    //закрываем устройство (closed device)
    if (::CloseHandle(hd) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeClose() \n"
            " -> function: ::CloseHandle(hd) returned: 0. Error! \n");
#endif
        okClosed = false;
    }
    hd = INVALID_HANDLE_VALUE;
    return okClosed;
}

bool NativeSerialEnginePrivate::nativeFlush()
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeFlush() \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    bool ret = ::FlushFileBuffers(hd);
    if (!ret) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeFlush() \n"
            " -> function:: ::FlushFileBuffers(hd) returned: false. Error! \n");
#endif
    }
    return ret;
}

bool NativeSerialEnginePrivate::nativeReset()
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeReset() \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }

    bool ret = true;

    // cброс буферов порта. (reset device buffers)
    if (::PurgeComm(hd,(PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeReset() \n"
            " -> function: ::PurgeComm(...) returned: 0. Error! \n");
#endif
        ret = false;
    }
    // cброс регистров порта. (reset device registers)
    if (::ClearCommBreak(hd) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeReset() \n"
            " -> function: ::ClearCommBreak(hd) returned: 0. Error! \n");
#endif
        ret = false;
    }
    return ret;
}

bool NativeSerialEnginePrivate::nativeSetDtr(bool set)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }
    if (::EscapeCommFunction(hd, (set) ? SETDTR : CLRDTR) == -1) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetDtr(bool set) \n"
            " -> function: ::EscapeCommFunction(...) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

bool NativeSerialEnginePrivate::nativeSetRts(bool set)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return false;
    }
    if (::EscapeCommFunction(hd, (set) ? SETRTS : CLRRTS) == -1) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSetRts(bool set) \n"
            " -> function: ::EscapeCommFunction(...) returned: -1. Error! \n");
#endif
        return false;
    }
    return true;
}

ulong NativeSerialEnginePrivate::nativeLineStatus()
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeLineStatus() \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return 0;
    }

    ulong temp = 0, status = 0;
    if (::GetCommModemStatus(hd, &temp) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeLineStatus() \n"
            " -> function: ::GetCommModemStatus(...) returned: 0. Error! \n");
#endif
        return status;
    }
    if (temp&MS_CTS_ON) status |= AbstractSerial::lsCTS;
    if (temp&MS_DSR_ON) status |= AbstractSerial::lsDSR;
    if (temp&MS_RING_ON) status |= AbstractSerial::lsRI;
    if (temp&MS_RLSD_ON) status |= AbstractSerial::lsDCD;
    return status;
}

qint64 NativeSerialEnginePrivate::nativeBytesAvailable(bool wait)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return 0;
    }

    DWORD err = 0;
    DWORD bav = 0;
    COMSTAT cs; ::ZeroMemory(&cs, sizeof(cs));
    for(;;) {
        if (ClearCommError(hd, &err, &cs) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
            " -> function: ::ClearCommError(hd, &err, &cs) returned: 0. Error! \n");
#endif
            return (qint64)-1;
        }
        if (err != 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeBytesAvailable(bool wait) \n"
                " -> in function: ::ClearCommError(hd, &err, &cs), \n"
                " output parameter: err: " << err << ". Error! \n";
#endif
            return (qint64)-1;
        }
        bav = cs.cbInQue;
        if (!wait) break;
        if (bav > 0) break;
        if (!nativeSelect(m_charIntervalTimeout, true)) break;
    }
    return (qint64)bav;
}

qint64 NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return (qint64)-1;
    }

    OVERLAPPED ovl; ::ZeroMemory(&ovl, sizeof(ovl));
    DWORD bytesWritten = 0;
    bool fSucess = false;

    if (!::WriteFile(hd, (void*)data, (DWORD)len, &bytesWritten, &ovl)) {
        DWORD rc = ::GetLastError();
        if (rc == ERROR_IO_PENDING ) {
            if (!::GetOverlappedResult(hd, &ovl, &bytesWritten, true)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
            " -> function: ::GetOverlappedResult(hd, &ovl, &bytesWritten, true) returned: false. Error! \n");
#endif
            }
            else {
                fSucess = true;
            }
        }
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) \n"
                " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
        }
    }
    else {
        fSucess = true;
    }
    ////
    if (fSucess) {
        if (bytesWritten == len)
            return (quint64)bytesWritten;
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeWrite(const char *data, qint64 len) error: \n"
                "    - query for write(send) is : "  << len << " bytes \n"
                "    - actually write(send) is  : "  << bytesWritten << " bytes \n";
#endif
        }
    }
    return (qint64)-1;
}

qint64 NativeSerialEnginePrivate::nativeRead(char *data, qint64 len)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
            " -> hd == INVALID_HANDLE_VALUE. Error! \n");
#endif
        return (qint64)-1;
    }

    OVERLAPPED ovl; ::ZeroMemory(&ovl, sizeof(ovl));
    DWORD bytesReaded = 0;
    bool fSucess = false;

    if ( !::ReadFile(hd, (void*)data, (DWORD)len, &bytesReaded, &ovl) ) {
        DWORD rc = ::GetLastError();
        if (rc == ERROR_IO_PENDING ) {
            if (!::GetOverlappedResult(hd, &ovl, &bytesReaded, true)) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
            " -> function: ::GetOverlappedResult(hd, &ovl, &bytesReaded, true) returned: false. Error! \n");
#endif
            }
            else {
                fSucess = true;
            }
        } // ERROR_IO_PENDING
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) \n"
                " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
        }
    }
    else {
        fSucess = true;
    }
    ////
    if (fSucess) {
        if (bytesReaded == len)
            return (qint64)bytesReaded;
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeRead(char *data, qint64 len) error: \n"
                " - query for reading is : "  << len << " bytes \n"
                " - actually read is     : "  << bytesReaded << " bytes \n";
#endif
        }
    }
    return (qint64)-1;
}

int NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
    int ret = -1;
    DWORD eventMask = 0;

    //создаем событие (create event)
    OVERLAPPED ovl; ::ZeroMemory(&ovl, sizeof(ovl));

    QT_WA ({
        ovl.hEvent = ::CreateEventW(0, false, false, 0);
    }, {
        ovl.hEvent = ::CreateEventA(0, false, false, 0);
    });

    if (ovl.hEvent == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::CreateEvent(0, false, false, 0) returned: " << ovl.hEvent << ". Error! \n";
#endif
        return ret;
    }

    if(::GetCommMask(hd, &eventMask) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::GetCommMask(hd, &eventMask) returned: 0. Error! \n";
#endif
        return ret;
    }

    if (selectForRead)
        if ( (eventMask & EV_RXCHAR) == 0 )
            eventMask |= EV_RXCHAR;
    else
        if ( (eventMask & EV_TXEMPTY) == 0 )
            eventMask |= EV_TXEMPTY;

        //установили маску
    if(::SetCommMask(hd, eventMask) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::SetCommMask(hd, eventMask) returned: 0. Error! \n";
#endif
        return ret;
    }
    bool fSucess = false;
    if (::WaitCommEvent(hd, &eventMask, &ovl) == 0) {
        DWORD rc = ::GetLastError();
        if (rc == ERROR_IO_PENDING) {
            rc = ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout);
            switch (rc) {
                case WAIT_OBJECT_0:
                    fSucess = true; break;
                case WAIT_TIMEOUT:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout), \n"
                " returned: WAIT_TIMEOUT: " << rc << ". Warning! \n";
#endif
                    ret = 0; break;
                default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout), \n"
                " returned: " << rc << ". Error! \n";
#endif
                    ;
            }
        }
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
        }
    }
    else {
        fSucess = true;
    }

    if (fSucess) {
        fSucess = false;
        if (selectForRead)
            if ( (eventMask & EV_RXCHAR) == EV_RXCHAR ) {
                ret = 1;
                fSucess = true;
            }
        else
            if ( (eventMask & EV_TXEMPTY) == EV_TXEMPTY ) {
                ret = 1;
                fSucess = true;
            }

        if (!fSucess) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
                " -> in function: ::WaitCommEvent(hd, &eventMask, &ovl), \n"
                " undefined mask event eventMask: " << eventMask <<". Error! \n";
#endif
        }
    }

    if (::CloseHandle(ovl.hEvent) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSelect(int timeout, bool selectForRead) \n"
            " -> function: ::CloseHandle(ovl.hEvent) returned: 0. Error! \n");
#endif
        ret = -1;
    }
    return ret;
}

int NativeSerialEnginePrivate::nativeSelect(int timeout,
                                            bool checkRead, bool checkWrite,
                                            bool *selectForRead, bool *selectForWrite) const
{
    int ret = -1;
    DWORD eventMask = 0;

    //создаем событие (create event)
    OVERLAPPED ovl; ::ZeroMemory(&ovl, sizeof(ovl));

    QT_WA ({
        ovl.hEvent = ::CreateEventW(0, false, false, 0);
    }, {
        ovl.hEvent = ::CreateEventA(0, false, false, 0);
    });

    if (ovl.hEvent == INVALID_HANDLE_VALUE) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::CreateEvent(0, false, false, 0) returned: " << ovl.hEvent << ". Error! \n";
#endif
        return ret;
    }

    if(::GetCommMask(hd, &eventMask) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::GetCommMask(hd, &eventMask) returned: 0. Error! \n";
#endif
        return ret;
    }

    if (selectForRead)
        if ( (eventMask & EV_RXCHAR) == 0 )
            eventMask |= EV_RXCHAR;
    else
        if ( (eventMask & EV_TXEMPTY) == 0 )
            eventMask |= EV_TXEMPTY;

    //установили маску
    if(::SetCommMask(hd, eventMask) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::SetCommMask(hd, eventMask) returned: 0. Error! \n";
#endif
        return ret;
    }
    bool fSucess = false;
    if (::WaitCommEvent(hd, &eventMask, &ovl) == 0) {
        DWORD rc = ::GetLastError();
        if (rc == ERROR_IO_PENDING) {
            rc = ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout);
            switch (rc) {
                case WAIT_OBJECT_0:
                    fSucess = true; break;
                case WAIT_TIMEOUT:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout), \n"
                " returned: WAIT_TIMEOUT: " << rc << ". Warning! \n";
#endif
                    ret = 0; break;
                default:
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::WaitForSingleObject(ovl.hEvent, timeout < 0 ? 0 : timeout), \n"
                " returned: " << rc << ". Error! \n";
#endif
                    ;
            }
        }
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> function: ::GetLastError() returned: " << rc << ". Error! \n";
#endif
        }
    }
    else {
        fSucess = true;
    }

    if (fSucess) {
        fSucess = false;

        *selectForRead = (eventMask & EV_RXCHAR) ? true : false;
        *selectForWrite = (eventMask & EV_TXEMPTY) ? true : false;

        if ((*selectForRead) || (*selectForWrite))
            ret = 1;
        else {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
                " -> in function: ::WaitCommEvent(hd, &eventMask, &ovl), \n"
                " undefined mask event eventMask: " << eventMask <<". Error! \n";
#endif
        }
    }

    if (::CloseHandle(ovl.hEvent) == 0) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug("Windows: NativeSerialEnginePrivate::nativeSelect(...) \n"
            " -> function: ::CloseHandle(ovl.hEvent) returned: 0. Error! \n");
#endif
        ret = -1;
    }
    return ret;
}

//added 05.11.2009
qint64 NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) const
{
    COMMPROP commProp;
    ::ZeroMemory(&commProp,  sizeof(COMMPROP));

    if ( !(::GetCommProperties(hd, &commProp)) ) {
#if defined (NATIVESERIALENGINE_WIN_DEBUG)
    qDebug() << "Windows: NativeSerialEnginePrivate::nativeCurrentQueue(NativeSerialEngine::ioQueue Queue) \n"
                " -> function: ::GetCommProperties(hd, &commProp) returned: false. Error! \n";
#endif
        return (qint64)-1;
    }

    switch (Queue) {
        case NativeSerialEngine::txQueue : return (qint64)commProp.dwCurrentTxQueue;
        case NativeSerialEngine::rxQueue : return (qint64)commProp.dwCurrentRxQueue;
        default: return (qint64)-1;
    }
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
        d->readNotifier = new WinSerialNotifier(d->hd, WinSerialNotifier::Read, this);
        QObject::connect(d->readNotifier, SIGNAL(activated(HANDLE)),
                            this, SIGNAL(readNotification()));
        d->readNotifier->setEnabled(true);
    }
}
