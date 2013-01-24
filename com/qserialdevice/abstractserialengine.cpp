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


AbstractSerialEnginePrivate::AbstractSerialEnginePrivate()
    :
    m_deviceName(AbstractSerial::defaultDeviceName),
    m_baudRate(AbstractSerial::BaudRate9600),
    m_dataBits(AbstractSerial::DataBits8),
    m_parity(AbstractSerial::ParityNone),
    m_stopBits(AbstractSerial::StopBits1),
    m_flow(AbstractSerial::FlowControlOff),
    m_charIntervalTimeout(10),
    m_status(AbstractSerial::ENone),
    m_oldSettingsIsSaved(false)
{
}

AbstractSerialEnginePrivate::~AbstractSerialEnginePrivate()
{
}

//---------------------------------------------------------------------------//

AbstractSerialEngine::AbstractSerialEngine(QObject *parent)
    : QObject(parent), d_ptr(new AbstractSerialEnginePrivate())
{
}

AbstractSerialEngine::AbstractSerialEngine(AbstractSerialEnginePrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd)
{
}

AbstractSerialEngine::~AbstractSerialEngine()
{
    delete d_ptr;
}

void AbstractSerialEngine::setDeviceName(const QString &deviceName)
{
    d_func()->m_deviceName = deviceName;
}

AbstractSerialEngine *AbstractSerialEngine::createSerialEngine(QObject *parent)
{
    return new NativeSerialEngine(parent);
}

QString AbstractSerialEngine::deviceName() const
{
    return d_func()->m_deviceName;
}

AbstractSerial::BaudRate AbstractSerialEngine::baudRate() const
{
    return d_func()->m_baudRate;
}

AbstractSerial::DataBits AbstractSerialEngine::dataBits() const
{
    return d_func()->m_dataBits;
}

AbstractSerial::Parity AbstractSerialEngine::parity() const
{
    return d_func()->m_parity;
}

AbstractSerial::StopBits AbstractSerialEngine::stopBits() const
{
    return d_func()->m_stopBits;
}

AbstractSerial::Flow AbstractSerialEngine::flow() const
{
    return d_func()->m_flow;
}

int AbstractSerialEngine::charIntervalTimeout() const
{
    return d_func()->m_charIntervalTimeout;
}

AbstractSerial::Status AbstractSerialEngine::status() const
{
    return d_func()->m_status;
}
