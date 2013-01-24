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
#include <QtCore/QVariant>
#include <QtCore/QRegExp>

#include <qt_windows.h>

#include "serialdeviceinfo_p.h"

#define SERIALDEVICEINFO_WIN_DEBUG

#ifdef SERIALDEVICEINFO_WIN_DEBUG
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

static bool readKey(HKEY keyHandle, const QString &keyName, QVariant *value)
{
    DWORD dataType;
    DWORD dataSize;
    LONG res;

    //1. First step -> get the size and type of the value (see examples in MSDN)
    QT_WA( {
        res = ::RegQueryValueExW(keyHandle, reinterpret_cast<const wchar_t *>(keyName.utf16()), 0, &dataType, 0, &dataSize);
    }, {
        res = ::RegQueryValueExA(keyHandle, keyName.toLocal8Bit(), 0, &dataType, 0, &dataSize);
    } );

    if (res != ERROR_SUCCESS) {
        //closeKey(&keyHandle);
        return false;
    }

    QByteArray data(dataSize, 0);

    //2. Second step -> get the value (see examples in MSDN)
    QT_WA( {
        res = ::RegQueryValueExW(keyHandle, reinterpret_cast<const wchar_t *>(keyName.utf16()), 0, 0,
                                 reinterpret_cast<unsigned char*>(data.data()), &dataSize);
    }, {
        res = ::RegQueryValueExA(keyHandle, keyName.toLocal8Bit(), 0, 0,
                                 reinterpret_cast<unsigned char*>(data.data()), &dataSize);
    } );

    if (res != ERROR_SUCCESS) {
        //closeKey(&keyHandle);
        return false;
    }

    switch (dataType) {
        case REG_EXPAND_SZ:
        case REG_SZ: {
            QString s;
            if (dataSize) {
                QT_WA( {
                    s = QString::fromUtf16(((const ushort*)data.constData()));
                }, {
                    s = QString::fromLocal8Bit(data.constData());
                } );
            }
            if (value != 0)
                //*value = stringToVariant(s);
                *value = QVariant(s);
            break;
        }

        case REG_MULTI_SZ: {
            QStringList l;
            if (dataSize) {
                int i = 0;
                for (;;) {
                    QString s;
                    QT_WA( {
                        s = QString::fromUtf16((const ushort*)data.constData() + i);
                    }, {
                        s = QString::fromLocal8Bit(data.constData() + i);
                    } );
                    i += s.length() + 1;

                    if (s.isEmpty())
                        break;
                    l.append(s);
                }
            }
            if (value != 0)
                //*value = stringListToVariantList(l);
                *value = QVariant(l);
            break;
        }

        case REG_NONE:
        case REG_BINARY: {
            QString s;
            if (dataSize) {
                QT_WA( {
                    s = QString::fromUtf16((const ushort*)data.constData(), data.size()/2);
                }, {
                    s = QString::fromLocal8Bit(data.constData(), data.size());
                } );
            }
            if (value != 0)
                //*value = stringToVariant(s);
                *value = QVariant(s);
            break;
        }

        case REG_DWORD_BIG_ENDIAN:
        case REG_DWORD: {
            Q_ASSERT(data.size() == sizeof(int));
            int i;
            ::memcpy((char*)&i, data.constData(), sizeof(int));
            if (value != 0)
                *value = i;
            break;
        }

        default:
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: bool readKey(HKEY keyHandle, const QString &keyName, QVariant *value) \n"
                " -> function: ::RegQueryValueEx(...), \n"
                "returned: unknown registry data type" << static_cast<int>(dataType) << ". Error! \n";
#endif
            if (value != 0)
                *value = QVariant();
            break;
    }
    return true;
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

static QString systemPathFromDevName(HKEY keyHandle, const QString &deviceName)
{
    LONG res = ERROR_SUCCESS;
    int i = 0;
    DWORD keyType = 0;

    QByteArray buffKeyName(MAX_KEY_OR_VAR_LEN, 0);
    QByteArray buffKeyVal(MAX_KEY_OR_VAR_LEN, 0);

    QString itemName;
    QString itemValue;
    
    for (;;) {
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

        if (itemValue == deviceName)
            break;
    }
    return itemName;
}
// --- End Wrapper

/*! Returns an instance of the device, which corresponds to the specified device name.
*/
static QString deviceInstanceIdPath(const QString &deviceName) {

    //1. Get key handle
    HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, "SYSTEM\\CurrentControlSet\\Services\\serenum\\Enum");

    if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
        return QString(0);
    }

    //2. Request from the key - count of serial devices available
    QVariant varValue;

    if (!readKey(hKey, "Count", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
        closeKey(&hKey);
        return QString(0);
    }

    bool ok = false;
    int countDevices = varValue.toInt(&ok);

    if ((countDevices == 0) || (!ok)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> Serial devices are not detected, countDevices: " << countDevices << ". Error! \n";
#endif
        closeKey(&hKey);
        return QString(0);
    }

    //3. Requests from the registry code copies the device and puts it in line with the name of the device.
    QStringList deviceInstanceIdList;

    for (int i = 0; i<countDevices; ++i) {

        if (!readKey(hKey, QString("%1").arg(i), &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }
        deviceInstanceIdList.append(varValue.toString());
    }
    closeKey(&hKey);
    //
    foreach (QString sDeviceInstanceId, deviceInstanceIdList) {

        hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE,
                       "SYSTEM\\CurrentControlSet\\Enum\\" + sDeviceInstanceId + "\\Device Parameters");

        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        if (!readKey(hKey, "PortName", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString deviceInstanceId(const QString &deviceName) \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        if (varValue.toString() == deviceName) {
            closeKey(&hKey);
            sDeviceInstanceId.prepend("SYSTEM\\CurrentControlSet\\Enum\\");
            return sDeviceInstanceId;
        }
    }
    closeKey(&hKey);
    return QString(0);
}

SerialDeviceInfoPrivate::SerialDeviceInfoPrivate()
{
    nativeClear();
}

SerialDeviceInfoPrivate::~SerialDeviceInfoPrivate()
{

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
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    
    HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_READ, "Hardware\\Devicemap\\Serialcomm");
    if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeSystemPath() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
        return QString(0);
    }
    
    QString systemPath = systemPathFromDevName(hKey, m_systemDeviceName);
    closeKey(&hKey);
    
    return systemPath;
}

QString SerialDeviceInfoPrivate::nativeLocationInfo()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }

    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }

    if (m_locationInfo.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        QVariant varValue;

        if (!readKey(hKey, "LocationInformation", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeLocationInfo() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        closeKey(&hKey);
        m_locationInfo = varValue.toString();
    }
    return m_locationInfo;
}

QString SerialDeviceInfoPrivate::nativeDriver()
{
    /* TODO: ???
    */
    return QString(0);
}

QString SerialDeviceInfoPrivate::nativeSubSystem()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }

    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }

    if (m_class.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        QVariant varValue;

        //TODO: This dilemma: either use the "Class" or "Service"!?
        if (!readKey(hKey, "Class", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeSubSystem() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        closeKey(&hKey);
        m_class = varValue.toString();
    }
    return m_class;
}

QString SerialDeviceInfoPrivate::nativeFriendlyName()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }

    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }

    if (m_deviceFriendlyName.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        QVariant varValue;

        if (!readKey(hKey, "FriendlyName", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeFriendlyName() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        closeKey(&hKey);
        m_deviceFriendlyName = varValue.toString();
    }
    return m_deviceFriendlyName;
}

QString SerialDeviceInfoPrivate::nativeDescription()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }
    if (m_deviceDescription.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        QVariant varValue;

        if (!readKey(hKey, "DeviceDesc", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeDescription() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        closeKey(&hKey);
        m_deviceDescription = varValue.toString();
    }
    return m_deviceDescription;
}

QStringList SerialDeviceInfoPrivate::nativeHardwareID()
{
    //TODO: need a public method or not!?
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QStringList(0);
    }
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QStringList(0);
    }
    if (m_hardwareID.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QStringList(0);
        }

        QVariant varValue;

        if (!readKey(hKey, "HardwareID", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeHardwareID() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QStringList(0);
        }

        closeKey(&hKey);
        m_hardwareID = varValue.toStringList();
    }
    return m_hardwareID;
}

QString SerialDeviceInfoPrivate::nativeVendorID()
{
    if (!m_vendor.isEmpty()) 
        return m_vendor;
        
    QRegExp re("Vid_(\\w+)&Pid_(\\w+)");
    QString str = nativeHardwareID().at(0);

    if (str.contains(re)) {
        m_vendor = re.cap(1);
        return m_vendor;
    }
    return QString(0);
}

QString SerialDeviceInfoPrivate::nativeProductID()
{
    if (!m_product.isEmpty()) 
        return m_product;
        
    QRegExp re("Vid_(\\w+)&Pid_(\\w+)");
    QString str = nativeHardwareID().at(0);

    if (str.contains(re)) {
        m_product = re.cap(2);
        return m_product;
    }
    return QString(0);
}

QString SerialDeviceInfoPrivate::nativeManufacturer()
{
    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return QString(0);
    }

    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return QString(0);
    }

    if (m_mfg.isEmpty()) {
        if (m_regKeyID.isEmpty())
            m_regKeyID = deviceInstanceIdPath(m_systemDeviceName);

        HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_QUERY_VALUE, m_regKeyID);
        if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
            return QString(0);
        }

        QVariant varValue;

        if (!readKey(hKey, "Mfg", &varValue)) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: QString SerialDeviceInfoPrivate::nativeManufacturer() \n"
                " -> function: readKey(...) returned false. Error! \n";
#endif
            closeKey(&hKey);
            return QString(0);
        }

        closeKey(&hKey);
        m_mfg = varValue.toString();
    }
    return m_mfg;
}

void SerialDeviceInfoPrivate::nativeClear()
{
    m_internalSystemName.clear();
    m_regKeyID.clear();

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
}

bool SerialDeviceInfoPrivate::nativeExists() const
{

    bool ret = false;

    if (m_systemDeviceName.isEmpty()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeExists() \n"
                " -> m_systemDeviceName is empty (Set name using the setName()). Error! \n";
#endif
        return ret;
    }

    HKEY hKey = openKey(HKEY_LOCAL_MACHINE, KEY_READ, "Hardware\\Devicemap\\Serialcomm");
    if (!hKey) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeExists() \n"
                " -> function: openKey(...) returned invalid handle registry key: 0. Error! \n";
#endif
        return ret;
    }

    QStringList list = enumOnlySZKeysValues(hKey);
    closeKey(&hKey);

    if (list.contains(m_systemDeviceName))
        ret = true;

    return ret;
}

bool SerialDeviceInfoPrivate::nativeBusy() const
{
    if (!nativeExists()) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeBusy() \n"
                " -> device: " << m_systemDeviceName << " is not exists. Error! \n";
#endif
        return false;
    }

    bool ret = false;
    // Create the file handle.
    QString sDeviceName = "\\\\.\\" + m_systemDeviceName;
    QByteArray nativeDeviceName;

    QT_WA({
        nativeDeviceName = QByteArray((const char *)sDeviceName.utf16(), sDeviceName.size() * 2 + 1);
    }, {
        nativeDeviceName.append(sDeviceName);
    });

    HANDLE hd;

    QT_WA({
        hd = ::CreateFileW((TCHAR *)nativeDeviceName.constData(),
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           0, OPEN_EXISTING,
                           FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                           0);
    }, {
        hd = ::CreateFileA(nativeDeviceName.constData(),
                           GENERIC_READ | GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           0, OPEN_EXISTING,
                           FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                           0);
    });

    if (hd == INVALID_HANDLE_VALUE) {
        ret = true;

        LONG err = ::GetLastError();
        switch (::GetLastError()) {
            case ERROR_ACCESS_DENIED:
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() << "Windows: SerialDeviceInfoPrivate::nativeBusy() \n"
                " -> function: ::GetLastError() returned " << err << " \n"
                " ie Access is denied. \n";
#endif
                break;
            default:
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug() <<"Windows: SerialDeviceInfoPrivate::nativeBusy() \n"
                " -> function: ::GetLastError() returned " << err << " \n"
                " ie unknown error. \n";
#endif
                ;
        }
    }
    else {
        if (::CloseHandle(hd) == 0) {
#if defined (SERIALDEVICEINFO_WIN_DEBUG)
    qDebug("Windows: SerialDeviceInfoPrivate::nativeBusy() \n"
            " -> function: ::CloseHandle(hd) returned 0. Error! \n");
#endif
        }
    }
    return ret;
}
