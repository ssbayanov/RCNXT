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

#ifndef SERIALDEVICEWATCHER_P_H
#define SERIALDEVICEWATCHER_P_H

#include "serialdevicewatcher.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
class QWinEventNotifier;
#else
struct udev;
struct udev_monitor;
class QSocketNotifier;
#endif

class SerialDeviceWatcherPrivate
{
    Q_DECLARE_PUBLIC(SerialDeviceWatcher)
public:
    SerialDeviceWatcherPrivate();
    virtual ~SerialDeviceWatcherPrivate();

    QStringList nativeAvailableDevices() const;
    void setEnabled(bool enable);
    bool isEnabled() const;

    SerialDeviceWatcher * q_ptr;

private:
#ifdef Q_OS_WIN
    HANDLE m_hEvent;
    HKEY m_hKey;
    QWinEventNotifier *availableNotifier;
#else
    int u_sock;
    struct udev *u_lib;
    struct udev_monitor *u_mon;
    QSocketNotifier *availableNotifier;
#endif
    QStringList m_devicesList;
    bool m_enabled;
    bool m_justCreated;
    void _q_processAvailableNotifier();
};

#endif // SERIALDEVICEWATCHER_P_H
