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


#include "nativeserialengine.h"


#define NATIVESERIALENGINE_DEBUG

#ifdef NATIVESERIALENGINE_DEBUG
#include <QtCore/QDebug>
#endif



//-----------------------------------------------------------------------------------------------//

NativeSerialEngine::NativeSerialEngine(QObject *parent)
    : AbstractSerialEngine(*new NativeSerialEnginePrivate(), parent)
{
}

NativeSerialEngine::~NativeSerialEngine()
{
    close();//?
}

bool NativeSerialEngine::isValid() const
{
    Q_D(const NativeSerialEngine);
    return d->isValid();
}

bool NativeSerialEngine::setBaudRate(AbstractSerial::BaudRate baudRate)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetBaudRate(baudRate);
}

bool NativeSerialEngine::setDataBits(AbstractSerial::DataBits dataBits)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetDataBits(dataBits);
}

bool NativeSerialEngine::setParity(AbstractSerial::Parity parity)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetParity(parity);
}

bool NativeSerialEngine::setStopBits(AbstractSerial::StopBits stopBits)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetStopBits(stopBits);
}

bool NativeSerialEngine::setFlowControl(AbstractSerial::Flow flow)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetFlowControl(flow);
}

bool NativeSerialEngine::setCharIntervalTimeout(int msecs)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetCharIntervalTimeout(msecs);
}

bool NativeSerialEngine::open(QIODevice::OpenMode mode)
{
    Q_D(NativeSerialEngine);
    if (!d->nativeOpen(mode)) {
        close(); return false;
    }
    return true;
}

void NativeSerialEngine::close()
{
    Q_D(NativeSerialEngine);

    if (d->readNotifier)
        d->readNotifier->setEnabled(false);

    d->nativeClose();

    if (d->readNotifier) {
        delete d->readNotifier;
        d->readNotifier = 0;
    }
}

bool NativeSerialEngine::flush()
{
    Q_D(NativeSerialEngine);
    return d->nativeFlush();
}

bool NativeSerialEngine::reset()
{
    Q_D(NativeSerialEngine);
    return d->nativeReset();
}

bool NativeSerialEngine::setDtr(bool set)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetDtr(set);
}

bool NativeSerialEngine::setRts(bool set)
{
    Q_D(NativeSerialEngine);
    return d->nativeSetRts(set);
}

ulong NativeSerialEngine::lineStatus()
{
    Q_D(NativeSerialEngine);
    return d->nativeLineStatus();
}

qint64 NativeSerialEngine::bytesAvailable(bool wait)
{
    Q_D(NativeSerialEngine);
    return d->nativeBytesAvailable(wait);
}

qint64 NativeSerialEngine::write(const char *data, qint64 maxSize)
{
    Q_D(NativeSerialEngine);

    static const int writeChunkSize = 512; //chunk size to write bytes ( in byte )
    qint64 bytesToSend = 0;
    qint64 bytesWritten = 0;
    qint64 ret = 0;

    for(;;) {
        bytesToSend = qMin<qint64>(writeChunkSize, maxSize - ret);

        bytesWritten = d->nativeWrite((const char*)(data + ret), bytesToSend);
        if (bytesWritten <= 0) return (qint64)-1;
        if (bytesWritten != bytesToSend) return (qint64)-1;

        ret += bytesWritten;
        if (ret == maxSize) break;
    }
    return ret;
}

qint64 NativeSerialEngine::read(char *data, qint64 maxSize)
{
    Q_D(NativeSerialEngine);

    qint64 bytesToRead = 0;
    qint64 bytesReaded = 0;
    qint64 bav = 0;
    qint64 ret = 0;

    for(;;) {
        bav = d->nativeBytesAvailable(true);
        if (bav < 0) return (qint64)-1;
        if (bav == 0) break;

        bytesToRead = qMin<qint64>(maxSize - ret, bav);

        bytesReaded = d->nativeRead((char*)(data + ret), bytesToRead);
        if (bytesReaded <= 0) return (qint64)-1;
        if (bytesReaded != bytesToRead) return (qint64)-1;

        ret += bytesReaded;
        if (ret == maxSize) break;
    }
    return ret;
}

bool NativeSerialEngine::waitForRead(int msecs, bool *timedOut) const
{
    Q_D(const NativeSerialEngine);

    int ret = d->nativeSelect(msecs, true);
    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        return false;
    }
    return ret > 0;
}

bool NativeSerialEngine::waitForWrite(int msecs, bool *timedOut) const
{
    Q_D(const NativeSerialEngine);

    if (timedOut)
        *timedOut = false;

    int ret = d->nativeSelect(msecs, false);
    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        return false;
    }
    return ret > 0;
}

bool NativeSerialEngine::waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                                            bool checkRead, bool checkWrite,
                                            int msecs, bool *timedOut) const
{
    Q_D(const NativeSerialEngine);

    int ret = d->nativeSelect(msecs, checkRead, checkWrite, readyToRead, readyToWrite);

    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        return false;
    }
    return ret > 0;
}

//add 05.11.2009
qint64 NativeSerialEngine::currentTxQueue() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeCurrentQueue(txQueue);
}

//add 05.11.2009
qint64 NativeSerialEngine::currentRxQueue() const
{
    Q_D(const NativeSerialEngine);
    return d->nativeCurrentQueue(rxQueue);
}
