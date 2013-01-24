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


#include <QtCore/QStringList>
#include <QtCore/QSocketNotifier>
#include <QtCore/QDir>

#include <libudev.h>

#include "serialdevicewatcher.h"
#include "serialdevicewatcher_p.h"

#define SERIALDEVICEWATCHER_UNIX_DEBUG

#ifdef SERIALDEVICEWATCHER_UNIX_DEBUG
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <ctype.h>
#endif

static QStringList devices_available()
{
    QDir devDir("/dev");

    QStringList filtersList; filtersList.clear();
    QStringList devicesList; devicesList.clear();

    if (!devDir.exists()) {
#if defined (SERIALDEVICEWATCHER_UNIX_DEBUG)
    qDebug() << "Linux: \n"
                " -> in function: static QStringList devices_available() \n"
                "directory " << devDir.canonicalPath() << " is not exists. Error! \n";
#endif
        return devicesList;
    }

#ifdef Q_OS_IRIX
    filtersList << "ttyf*";
#elif defined(Q_OS_HPUX)
    filtersList << "tty1p*";
#elif defined(Q_OS_SOLARIS)
    filtersList << "tty*";
#elif defined(Q_OS_FREEBSD)
    filtersList << "ttyd*";
#elif defined(Q_OS_LINUX)
    filtersList << "ttyS*";
#else
    filtersList << "ttyS*";
#endif

    filtersList << "ttyUSB*" << "ttyACM*";

    devDir.setNameFilters(filtersList);
    devDir.setFilter(QDir::Files | QDir::System);
    QFileInfoList devicesInfoList = devDir.entryInfoList();

    foreach(QFileInfo info, devicesInfoList) {
        if (!info.isDir()) {
                devicesList << info.canonicalFilePath();
        }
    }
    return devicesList;
}

SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate()
    : u_sock(-1),
      u_lib(0),
      u_mon(0),
      availableNotifier(0),
      m_enabled(false),
      m_justCreated(true)
{
    m_devicesList = devices_available();
    //
    u_lib = ::udev_new();
    if (!u_lib) {
#if defined (SERIALDEVICEWATCHER_UNIX_DEBUG)
    qDebug() << "Linux: when created new udev \n"
                " -> in SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                "function udev_new() returned 0. Error! \n";
#endif
        return;
    }

    u_mon = ::udev_monitor_new_from_netlink( u_lib, "udev" );
    if (!u_mon) {
#if defined (SERIALDEVICEWATCHER_UNIX_DEBUG)
    qDebug() << "Linux: when created new udev monitor \n"
                " -> in SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                "function udev_monitor_new_from_netlink() returned 0. Error! \n";
#endif
        return;
    }

    int err = ::udev_monitor_enable_receiving(u_mon);
    if (err) {
#if defined (SERIALDEVICEWATCHER_UNIX_DEBUG)
    qDebug() << "Linux: when enable monitor receiving \n"
                " -> in SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                "function udev_monitor_enable_receiving() returned " << err << ". Error! \n";
#endif
        return;
    }

    u_sock = ::udev_monitor_get_fd(u_mon);
    if (u_sock ==  -1) {
#if defined (SERIALDEVICEWATCHER_UNIX_DEBUG)
    qDebug() << "Linux: when get monitor socket descriptor \n"
                " -> in SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                "function udev_monitor_get_fd() returned -1. Error! \n";
#endif
        return;
    }
}

SerialDeviceWatcherPrivate::~SerialDeviceWatcherPrivate()
{
    setEnabled(false);

    if (u_sock != -1) {
        close(u_sock);
        u_sock = -1;
    }
    if (u_mon) {
        //::udev_monitor_unref(u_mon);
        free(u_mon);
        u_mon = 0;
    }
    if (u_lib) {
        //::udev_unref(u_lib);
        free(u_lib);
        u_lib = 0;
    }
}

void SerialDeviceWatcherPrivate::setEnabled(bool enable)
{
    Q_Q(SerialDeviceWatcher);
    if (enable) {

        /* then if the object has just been created (m_justCreated) and never called the method "setEnabled()" -
        then "emit" the entire list of names for serial devices */
        if (m_justCreated) {
            m_justCreated = false;
            if (!m_devicesList.isEmpty()) {
                emit(q->hasChanged(m_devicesList));
            }
        }

        if (!availableNotifier) {
            availableNotifier = new QSocketNotifier(u_sock, QSocketNotifier::Read, q);
            q->connect(availableNotifier, SIGNAL(activated(int)), q, SLOT(_q_processAvailableNotifier()));
        }
    }
    availableNotifier->setEnabled(enable);
    m_enabled = enable;
}

void SerialDeviceWatcherPrivate::_q_processAvailableNotifier()
{
    Q_Q(SerialDeviceWatcher);

    QStringList list = devices_available();

    if (list != m_devicesList) {
        m_devicesList = devices_available();
        emit(q->hasChanged(m_devicesList));
    }
    setEnabled(true);
}

#include "moc_serialdevicewatcher.cpp"








