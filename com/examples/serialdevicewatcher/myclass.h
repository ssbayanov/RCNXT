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

#ifndef MYCLASS_H
#define MYCLASS_H

#include <QtCore>
#include <serialdevicewatcher.h>

class MyClass : public QObject
{
    Q_OBJECT
public:
    MyClass(QObject *parent = 0)
        : QObject(parent ){
        qDebug() << "Please remove or insert a serial device (eg USB/Serial controller) to see the notification.";
        sdw = new SerialDeviceWatcher(this);
        connect(sdw, SIGNAL(hasChanged(const QStringList &)), this, SLOT(showChange(const QStringList &)));
        sdw->setEnabled(true);
    }
    virtual ~MyClass() {}

private slots:
    void showChange(const QStringList &devices) {
        qDebug() << "Devices :" << devices;
    }

private:
    SerialDeviceWatcher *sdw;
};

#endif // WINSERIALNOTIFIER_H
