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
#include <QtCore/QVariant>
#include <QtCore/private/qwineventnotifier_p.h>

#include "serialdevicewatcher.h"
#include "serialdevicewatcher_p.h"

#define SERIALDEVICEWATCHER_WIN_DEBUG

#ifdef SERIALDEVICEWATCHER_WIN_DEBUG
#include <QtCore/QDebug>
#endif

// --- Wrapper for the WinAPI registry functions read (modified wrapper from Qt4 "qsettings_win.cpp")
static HKEY openKey(HKEY parentHandle, LONG perms, const QString &subKey)
{
    HKEY resultHandle = 0;
    LONG res;
    
    QT_WA( {
        res = ::RegOpenKeyExW(parentHandle, reinterpret_cast<const wchar_t *>(subKey.utf16()),
                              0, perms, &resultHandle);
    } , {
        res = ::RegOpenKeyExA(parentHandle, subKey.toLocal8Bit(),
                              0, perms, &resultHandle);
    } );

    if (res == ERROR_SUCCESS)
        return resultHandle;
    return 0;
}

static void closeKey(HKEY *keyHandle)
{
    if (keyHandle != 0)
        ::RegCloseKey(*keyHandle);
    *keyHandle = 0;
}

static const int MAX_KEY_OR_VAR_LEN = 256; // TODO ?

static QStringList enumOnlySZKeysValues(HKEY keyHandle)
{
    QStringList result;
    LONG res = ERROR_SUCCESS;
    int i = 0;
    DWORD keyType = 0;
    
    QByteArray buffKeyName(MAX_KEY_OR_VAR_LEN, 0);
    QByteArray buffKeyVal(MAX_KEY_OR_VAR_LEN, 0);
    
    for (;;) {
        QString itemName;
        QString itemValue;

        QT_WA( {
            DWORD lenKeyName = buffKeyName.size(); // TODO ?
            DWORD lenKeyValue = buffKeyVal.size();
            
            res = ::RegEnumValueW(keyHandle, i++,
                                reinterpret_cast<wchar_t *>(buffKeyName.data()), &lenKeyName, 
                                0, &keyType, 
                                reinterpret_cast<unsigned char*>(buffKeyVal.data()), &lenKeyValue); 
            if ((res == ERROR_SUCCESS) && (keyType == REG_SZ)) {
                itemName = QString::fromUtf16(reinterpret_cast<ushort*>(buffKeyName.data()), lenKeyName);
                itemValue = QString::fromUtf16(((const ushort*)buffKeyVal.constData()));
            }
        }, {
            DWORD lenKeyName = buffKeyName.size();
            DWORD lenKeyValue = buffKeyVal.size();
            res = ::RegEnumValueA(keyHandle, i++,
                                buffKeyName.data(), &lenKeyName, 
                                0, &keyType, 
                                reinterpret_cast<unsigned char*>(buffKeyVal.data()), &lenKeyValue); 
                                
            if ((res == ERROR_SUCCESS) && (keyType == REG_SZ)) {
                itemName = QString::fromLocal8Bit(buffKeyName.data(), lenKeyName);
                itemValue = QString::fromLocal8Bit(buffKeyVal.constData());
            }
        } );

        if (res != ERROR_SUCCESS) {
            break;
        }
        
        if (itemValue.isEmpty())
            itemValue = QLatin1String(".");
        
        result.append(itemValue);
    }
    return result;
}
// --- End Wrapper


SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate()
    : m_hEvent(0),
      m_hKey(0),
      availableNotifier(0),
      m_enabled(false),
      m_justCreated(true)
{
    //1.
    m_hKey = openKey(HKEY_LOCAL_MACHINE,
                     KEY_NOTIFY | KEY_QUERY_VALUE,
                     "HARDWARE\\DEVICEMAP\\SERIALCOMM");
                     
    if (!m_hKey) {
#if defined (SERIALDEVICEWATCHER_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                " -> function: openKey() returned false. Error! \n";
#endif
        return;
    }
    //2.
    QT_WA ({
        m_hEvent = ::CreateEventW(0, false, false, 0);
    }, {
        m_hEvent = ::CreateEventA(0, false, false, 0);
    });
    
    if (!m_hEvent) {
#if defined (SERIALDEVICEWATCHER_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceWatcherPrivate::SerialDeviceWatcherPrivate() \n"
                " -> function: ::CreateEvent(0, false, false, 0) returned " << m_hEvent << ". Error! \n";
#endif
        closeKey(&m_hKey);
        return;
    }
    //3.
    m_devicesList = enumOnlySZKeysValues(m_hKey);
}

SerialDeviceWatcherPrivate::~SerialDeviceWatcherPrivate()
{
    setEnabled(false);

    if (m_hEvent != INVALID_HANDLE_VALUE) {
        if (!::CloseHandle(m_hEvent)) {
#if defined (SERIALDEVICEWATCHER_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceWatcherPrivate::~SerialDeviceWatcherPrivate() \n"
                " -> function: ::CloseHandle(m_hEvent) returned 0. Error! \n";
#endif
        }
        m_hEvent = 0;
    }

    if (m_hKey)
        closeKey(&m_hKey);
}

void SerialDeviceWatcherPrivate::setEnabled(bool enable)
{
    Q_Q(SerialDeviceWatcher);
    if (enable) {

        /* then if the object has just been created (m_justCreated) and never called the method "setEnabled()" -
        then "emit" the entire list of names for serial devices */
        if (m_justCreated) {
            m_justCreated = false;
            if (!m_devicesList.isEmpty())
                emit(q->hasChanged(m_devicesList));
        }

        if (!availableNotifier) {
            availableNotifier = new QWinEventNotifier(m_hEvent, q);
            q->connect(availableNotifier, SIGNAL(activated(HANDLE)), q, SLOT(_q_processAvailableNotifier()));
        }

        //set tracking
        LONG rc = ::RegNotifyChangeKeyValue(m_hKey, true, REG_NOTIFY_CHANGE_LAST_SET, m_hEvent, true);
        if (rc != ERROR_SUCCESS) {
#if defined (SERIALDEVICEWATCHER_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceWatcherPrivate::setEnabled(bool enable) \n"
                " -> function: ::RegNotifyChangeKeyValue() returned " << rc << ". Error! \n";
#endif
            return;
        }
    }
    availableNotifier->setEnabled(enable);
    m_enabled = enable;
}

void SerialDeviceWatcherPrivate::_q_processAvailableNotifier()
{
    Q_Q(SerialDeviceWatcher);

    QStringList list = enumOnlySZKeysValues(m_hKey);

    if (list != m_devicesList) {
        m_devicesList = list;
        emit(q->hasChanged(m_devicesList));
    }

    setEnabled(true);
}

#include "moc_serialdevicewatcher.cpp"








