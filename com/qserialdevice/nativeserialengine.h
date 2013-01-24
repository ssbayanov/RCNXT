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

#ifndef NATIVESERIALENGINE_H
#define NATIVESERIALENGINE_H

#include <QtCore/QStringList>

#include "abstractserialengine.h"

class NativeSerialEnginePrivate;
class NativeSerialEngine : public AbstractSerialEngine
{
    Q_OBJECT
public:
    explicit NativeSerialEngine(QObject *parent = 0);
    virtual ~NativeSerialEngine();

    bool isValid() const;

    //my impl (device parameters and controls)
    bool setBaudRate(AbstractSerial::BaudRate baudRate);
    bool setDataBits(AbstractSerial::DataBits dataBits);
    bool setParity(AbstractSerial::Parity parity);
    bool setStopBits(AbstractSerial::StopBits stopBits);
    bool setFlowControl(AbstractSerial::Flow flow);
    bool setCharIntervalTimeout(int msecs);

    bool open(QIODevice::OpenMode mode);
    void close();
    bool flush();
    bool reset();

    bool setDtr(bool set);
    bool setRts(bool set);

    ulong lineStatus();

    qint64 bytesAvailable(bool wait);

    qint64 read(char *data, qint64 maxSize);
    qint64 write(const char *data, qint64 maxSize);

    bool waitForRead(int msecs = 3000, bool *timedOut = 0) const;
    bool waitForWrite(int msecs = 3000, bool *timedOut = 0) const;
    bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                            bool checkRead, bool checkWrite,
                            int msecs = 3000, bool *timedOut = 0) const;

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable);

protected:
    //add 05.11.2009
    enum ioQueue{ txQueue, rxQueue };
    qint64 currentTxQueue() const;
    qint64 currentRxQueue() const;

private:
    Q_DECLARE_PRIVATE(NativeSerialEngine)
    Q_DISABLE_COPY(NativeSerialEngine)
};

#ifdef Q_OS_WIN
#include "winserialnotifier.h"
#else
#include <QtCore/QSocketNotifier>
#include <termios.h>
#endif

class NativeSerialEnginePrivate : public AbstractSerialEnginePrivate
{
public:
    NativeSerialEnginePrivate();

#ifdef Q_OS_WIN
    HANDLE hd;
    WinSerialNotifier *readNotifier;
#else
    int fd;
    QSocketNotifier *readNotifier;
#endif

    bool isValid() const;

    bool nativeSetBaudRate(AbstractSerial::BaudRate baudRate);
    bool nativeSetDataBits(AbstractSerial::DataBits dataBits);
    bool nativeSetParity(AbstractSerial::Parity parity);
    bool nativeSetStopBits(AbstractSerial::StopBits stopBits);
    bool nativeSetFlowControl(AbstractSerial::Flow flow);
    bool nativeSetCharIntervalTimeout(int msecs);
    //
    bool nativeOpen(QIODevice::OpenMode mode);
    bool nativeClose();
    bool nativeFlush();
    bool nativeReset();

    bool nativeSetDtr(bool set);
    bool nativeSetRts(bool set);

    ulong nativeLineStatus();

    qint64 nativeBytesAvailable(bool wait);

    qint64 nativeRead(char *data, qint64 len);
    qint64 nativeWrite(const char *data, qint64 len);
    int nativeSelect(int timeout, bool selectForRead) const;
    int nativeSelect(int timeout, bool checkRead, bool checkWrite,
                bool *selectForRead, bool *selectForWrite) const;

    //add 05.11.2009
    qint64 nativeCurrentQueue(NativeSerialEngine::ioQueue queue) const;

private:

#ifdef Q_OS_WIN
    COMMCONFIG cc, oldcc;
    COMMTIMEOUTS ct;
#else
    struct termios tio;
    struct termios oldtio;
#endif

};

#endif // NativeSerialEngine_H
