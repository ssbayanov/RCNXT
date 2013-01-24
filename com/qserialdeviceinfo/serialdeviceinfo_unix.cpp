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


#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <libudev.h>

#include "serialdeviceinfo_p.h"

#define SERIALDEVICEINFO_UNIX_DEBUG

#ifdef SERIALDEVICEINFO_UNIX_DEBUG
#include <QtCore/QDebug>
#endif



SerialDeviceInfoPrivate::SerialDeviceInfoPrivate()
    : u_lib(0), u_dev(0)
{
    nativeClear();
    //тут создание udev_lib
    if (!m_createUdevLib()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::SerialDeviceInfoPrivate() \n"
                " -> function:  m_createUdevLib() returned: false. Error! \n";
#endif
        return;
    }
}

SerialDeviceInfoPrivate::~SerialDeviceInfoPrivate()
{
    nativeClear();
    if (u_lib) {
        //::udev_unref(u_lib);
        free(u_lib);
        u_lib = 0;
    }
}

QString SerialDeviceInfoPrivate::nativeName() const
{
    return m_systemDeviceName;
}

void SerialDeviceInfoPrivate::nativeSetName(const QString &name)
{
    nativeClear();
    m_systemDeviceName = name;
}


QString SerialDeviceInfoPrivate::nativeSystemPath()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_systemPath.isEmpty())
        return m_systemPath;
    
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_systemPath = QString(::udev_device_get_property_value(u_dev, "DEVPATH"));
}

QString SerialDeviceInfoPrivate::nativeLocationInfo()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_locationInfo.isEmpty())
        return m_locationInfo;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_locationInfo = QString(::udev_device_get_property_value(u_dev, "ID_MODEL"));
}

QString SerialDeviceInfoPrivate::nativeDriver()
{
/* TODO: 
Here I do not know how to use UDEV "pull" information about the driver.
ie I do not know what the key substitute in the function: udev_device_get_property_value(...).
ie need (for example) to get this:
# udevadm info -a --name /dev/ttyUSB0
...
...
DRIVERS = "pl2303" <- this is ( etc. )
...
*/
    return QString(0);
}

QString SerialDeviceInfoPrivate::nativeSubSystem()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_class.isEmpty())
        return m_class;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_class = QString(::udev_device_get_property_value(u_dev, "SUBSYSTEM"));
}

QString SerialDeviceInfoPrivate::nativeFriendlyName()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_deviceFriendlyName.isEmpty())
        return m_deviceFriendlyName;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
   return m_deviceFriendlyName = nativeDescription() + " (" + m_systemDeviceName + ")"; 
}

QString SerialDeviceInfoPrivate::nativeDescription()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_deviceDescription.isEmpty())
        return m_deviceDescription;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_deviceDescription = QString(::udev_device_get_property_value(u_dev, "ID_MODEL_FROM_DATABASE"));
}

QStringList SerialDeviceInfoPrivate::nativeHardwareID()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QStringList(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QStringList(0);
    }
    
    if (!m_hardwareID.isEmpty())
        return m_hardwareID;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QStringList(0);
        }
    }
    return m_hardwareID << QString(::udev_device_get_property_value(u_dev, "ID_VENDOR_ENC")) << QString(::udev_device_get_property_value(u_dev, "ID_MODEL_ENC"));
}

QString SerialDeviceInfoPrivate::nativeVendorID()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeVendorID() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeVendorID() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_vendor.isEmpty())
        return m_vendor;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeVendorID() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_vendor = QString(::udev_device_get_property_value(u_dev, "ID_VENDOR_ID"));
}

QString SerialDeviceInfoPrivate::nativeProductID()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeProductID() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeProductID() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_product.isEmpty())
        return m_product;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeProductID() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_product = QString(::udev_device_get_property_value(u_dev, "ID_MODEL_ID"));
}

QString SerialDeviceInfoPrivate::nativeManufacturer()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    if (!m_mfg.isEmpty())
        return m_mfg;
        
    if (!u_dev) {
        if (!m_createUdevDev()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> function:  m_createUdevDev() returned: false. Error! \n";
#endif
            return QString(0);
        }
    }
    return m_mfg = QString(::udev_device_get_property_value(u_dev, "ID_VENDOR_FROM_DATABASE"));
}

void SerialDeviceInfoPrivate::nativeClear()
{
    m_systemDeviceName.clear();
    m_systemPath.clear();
    m_class.clear();
    m_locationInfo.clear();
    m_driver.clear();
    m_deviceFriendlyName.clear();
    m_deviceDescription.clear();
    m_hardwareID.clear();
    m_vendor.clear();
    m_product.clear();
    m_mfg.clear();
    //тут добавить очистку udev
    if (u_dev) {
        //::udev_device_unref(u_dev);
        free(u_dev);
        u_dev = 0;
    }
}

bool SerialDeviceInfoPrivate::nativeExists() const
{
    QFile device(m_systemDeviceName);
    return device.exists();
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

bool SerialDeviceInfoPrivate::nativeBusy() const
{
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_UNIX_DEBUG)
    qDebug() << "Linux: SerialDeviceInfoPrivate::nativeBusy() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return false;
    }

    QString lockFileName = getLockFileName(m_systemDeviceName);
    if (checkDeviceLocked(lockFileName))
        return true;
    return false;
}

bool SerialDeviceInfoPrivate::m_createUdevLib()
{
    if (!u_lib) {
        if (!(u_lib = ::udev_new()))
             return false;
    }
    return true;
}

bool SerialDeviceInfoPrivate::m_createUdevDev()
{
    bool ret = false;
    struct udev_list_entry *c = 0;
    const char *syspath = 0;
    struct udev_enumerate *ue = 0;

    if (!u_lib)
        return ret;

    ue = udev_enumerate_new(u_lib);
    udev_enumerate_add_match_property(ue, "DEVNAME", m_systemDeviceName.toAscii());

    if (::udev_enumerate_scan_devices(ue))
        goto end_create;

    c = ::udev_enumerate_get_list_entry(ue);

    if (!(syspath = ::udev_list_entry_get_name(c)))
        goto end_create;

    if (!(u_dev = ::udev_device_new_from_syspath(u_lib, syspath)))
        goto end_create;

    ret = true;

end_create:

    if (c) {
        free(c); c = 0;
    }

    if (ue) {
        free(ue);
        //::udev_enumerate_unref(ue);
        ue = 0;
    }
    return ret;
}
