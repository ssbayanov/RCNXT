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


/*! \class SerialDeviceWatcher

    \brief \~russian Класс SerialDeviceWatcher следит за добавлением/удалением последовательных устройств в системе.

    \~russian Этот класс является частью библиотеки QSerialDevice и может использоваться совместно с классом AbstractSerial. \n

    Данный класс предоставляет следующие возможности:
    - получать список имен всех имеющихся в системе последовательных устройств.
    - сообщать об удалении/добавлении последовательного устройства.

    \note В текущей версии - работа класса тестировалась (тестировалась автором) только в операционных системах:
    - MS Windows XP Pro SP3
    - ArchLinux x86_64
    
    Принцип работы класса в разных операционных системах имеет существенные отличия:
    - в MS Windows XP - информация о наличии последовательных устройств, а также события при добавлении/удалении
    последовательных устройств, добывается путем чтения системного реестра по соответствующим путям и контроля 
    изменений некоторых ключей.
    - в Linux - для получении информации используется \b UDEV в совокупности с анализом содержимого директории "/dev/".

    Начало работы с классом необходимо начинать с создания экземпляра объекта SerialDeviceWatcher.\n
    Пример:
    \code
        ...
        SerialDeviceWatcher *sdw = new SerialDeviceWatcher(this);
        connect(sdw, SIGNAL(hasChanged(const QStringList &)), this, SLOT(showChange(const QStringList &)));
        ...
    \endcode

    По умолчанию новый экземпляр класса создается с отключенным контролем/слежением за наличием/отсутствием последовательных устройств. \n
    Код:
    \code
        ...
        connect(sdw, SIGNAL(hasChanged(const QStringList &)), this, SLOT(showChange(const QStringList &)));
        ...
    \endcode
    можно использовать, если Вы хотите получать и обрабатывать сигналы/уведомления о добавлении/удалении последовательных устройств в
    системе (см. ниже описание сигнала hasChanged() ).
    \note Делать \b connect необходимо до вызова метода: \b setEnabled(true).

    Для включения/отключения контроля/слежения за последовательными устройствами используется метод:
    - void SerialDeviceWatcher::setEnabled(bool enable). \n

    Для получения состояния класса (включен или отключен) используется метод:
    - bool SerialDeviceWatcher::isEnabled() const. \n
    
    Для получения списка имен всех имеющихся в системе последовательных устройств используется метод:
    - QStringList SerialDeviceWatcher::devicesAvailable() const. \n
    
    Класс SerialDeviceWatcher реализует следующие сигналы:
    - void SerialDeviceWatcher::hasChanged(const QStringList &list) - излучается \b КАЖДЫЙ раз при добавлении/удалении хотябы
    одного последовательного устройства.
    \note При первом вызове метода \b setEnabled(true) - сигнал вызовется автоматически с передачей в переменной \b list
    списка имен всех имеющихся в текущее время последовательных устройств. При последующих вызовах метода setEnabled() -
    сигнал автоматически будет излучаться только: при включенном состоянии класса \b И при добавлении/удалении устройства. \n

    \n
    \n
    \n
    
    \warning По умолчанию класс сконфигурирован таким образом, чтобы записывать в консоль отладочные сообщения.
    Для отключения вывода этих сообщений в консоль - необходимо закомментировать в *.cpp файлах строчки вида:
    \code
        ...
        #define NATIVESERIALENGINE_DEBUG
        ...
    \endcode
    и заново пересобрать библиотеку.

    \brief \~english Class SerialDeviceWatcher followed by adding/removing the serial devices in the system.

    \~english This class is part of the library QSerialDevice and can be used in conjunction with the class AbstractSerial. \n
    
    This class allows provides the following features:
    - Receive a list of names of all available serial devices in the system.
    - Report on deleting/adding the serial device.
    
    \note In the current version - working class tested (tested by the author.) only in operating systems:
    - MS Windows XP Pro SP3
    - ArchLinux x86_64
    
    The principle of class in different operating systems have significant differences:
    - In MS Windows XP - information about the presence of serial devices, as well as events by adding/removing
    serial device, is extracted by reading the registry for the relevant ways, and Control
    changes some keys.
    - In Linux - for information \b UDEV used in conjunction with the analysis of the contents of the directory "/dev/".
    
    Getting Started with the class should begin with the creation of an object instance SerialDeviceWatcher. \n
    Example:
    \code
        ...
        SerialDeviceWatcher *sdw = new SerialDeviceWatcher(this);
        connect(sdw, SIGNAL(hasChanged(const QStringList &)), this, SLOT(showChange(const QStringList &)));
        ...
    \endcode
    By default, a new instance of the class is created with a disabled control/tracking of the presence/absence of serial devices. \n
    
    Code:
    \code
        ...
        connect(sdw, SIGNAL(hasChanged(const QStringList &)), this, SLOT(showChange(const QStringList &)));
        ...
    \endcode
    can be used if you wish to receive and process signals/notification about adding/removing the serial devices in
    system (see below description of the signal hasChanged() ).
    \note Make \b connect necessary to the call method: \b setEnabled(true).
    
    To activate/deactivate control/monitoring of serial devices use the method:
    - void SerialDeviceWatcher::setEnabled(bool enable). \n

    For the get state of class (enabled or disabled) using the method:
    - bool SerialDeviceWatcher::isEnabled() const. \n
    
    For a list of names of all available serial devices in the system using the method:
    - QStringList SerialDeviceWatcher::devicesAvailable() const. \n
    
    Class SerialDeviceWatcher implements the following signals:
    - void SerialDeviceWatcher::hasChanged(const QStringList &list) - emitted \b Each time you add/remove At least
    one serial device.
    \note When you first call the method \b setEnabled(true) - the signal will automatically transfer to the variable \b list
    list of names of all available in the current time serial devices. In subsequent calls to the method setEnabled() -
    signal is automatically emitted only: when the state of class is Enabled \b And when adding/removing devices. \n

    \n
    \n
    \n
    
    \warning By default, the class is configured in such a way as to write to the console debug messages.
    To disable these messages in the console - need to comment out in the *. cpp files, a line like:
    \code
        ...
        # define NATIVESERIALENGINE_DEBUG
        ...
    \endcode
    and re-rebuild the library.




    \version \~ 0.2.0

    \author \~ Denis Shienkov
    - ICQ       : 321789831
    - e-mail    : scapig2@yandex.ru

    \~russian
    \note
    - Работа библиотеки тестировалась автором только с: Windows XP Pro SP3 и ArchLinux x86_64.
    \~english
    \note
    - Work of library was tested by author only with: Windows XP Pro Sp3 Pro and Archlinux x86_64.
*/

#include <QtCore/QStringList>

#include "serialdevicewatcher.h"
#include "serialdevicewatcher_p.h"

//#define SERIALDEVICEWATCHER_DEBUG

#ifdef SERIALDEVICEWATCHER_DEBUG
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <ctype.h>
#endif


QStringList SerialDeviceWatcherPrivate::nativeAvailableDevices() const
{
    return m_devicesList;
}

bool SerialDeviceWatcherPrivate::isEnabled() const
{
    return m_enabled;
}

//

/*! \fn SerialDeviceWatcher::SerialDeviceWatcher(QObject *parent)
    \~russian Конструктор по умолчанию. 
    \~english Default constructor.
*/
SerialDeviceWatcher::SerialDeviceWatcher(QObject *parent)
    : QObject(parent), d_ptr(new SerialDeviceWatcherPrivate())
{
    Q_D(SerialDeviceWatcher);
    d->q_ptr = this;
}

/*! \fn SerialDeviceWatcher::~SerialDeviceWatcher()
    \~russian Деструктор по умолчанию.
    \~english Default destructor.
*/
SerialDeviceWatcher::~SerialDeviceWatcher()
{
    delete d_ptr;
}

/*! \fn QStringList SerialDeviceWatcher::devicesAvailable() const
    \~russian Возвращает список имен всех последовательных устройств, которые присутствуют в системе на момент вызова метода.
    \return
    - список имен последовательных устройств
    - 0 в случае ошибки или при отсутствии последовательных устройств
    \~english Returns a list of all serial devices that are present in the system at the time of the method call.
    \return
    - list of names for serial devices
    - 0 in case of error or the absence of serial devices
*/
QStringList SerialDeviceWatcher::devicesAvailable() const
{
    return d_func()->nativeAvailableDevices();
}

/*! \fn void SerialDeviceWatcher::setEnabled(bool enable)
    \~russian Включает или отключает режим мониторинга/слежения за добавлением или удалением последовательных
    устройств из системы.
    \param[in] enable - флаг:
    - если его значение \b true - то включается режим мониторинга/слежения
    - если его значение \b false - то отключается режим мониторинга/слежения
    \~english Enables or disables the mode of monitoring and tracking of adding or removing successive
    devices from the system.
    \param[in] enable - flag: 
    - If its value is \b true - that included the regime of monitoring and tracking
    - If its value is \b false - then switch off the monitoring/tracking
*/
void SerialDeviceWatcher::setEnabled(bool enable)
{
    d_func()->setEnabled(enable); 
}

/*! \fn bool SerialDeviceWatcher::isEnabled() const
    \~russian Возвращает текущий статус/состояние/режим  работы класса SerialDeviceWatcher.
    \return
    - true - мониторинг включен
    - false - мониторинг отключен
    \note Включение/отключение мониторинга производится методом: setEnabled(bool enable).
    \~english Returns the current status/state/mode class SerialDeviceWatcher.
    \return
    - true - monitor enabled
    - false - monitoring disabled
    \note enable/disable monitoring is performed by: setEnabled(bool enable).
*/
bool SerialDeviceWatcher::isEnabled() const
{
    return d_func()->isEnabled();
}

/*! \fn SerialDeviceWatcher::hasChanged(const QStringList &list)
    \~russian Этот сигнал испускается каждый раз только при добавлении/удалении последовательного устройства из системы.
    Исключением является только случай когда первый раз вызывается метод setEnabled(true) - тогда сигнал принудительно автоматически
    излучается!
    \param[out] list - обновленный после добавления/удаления список имен последовательных устройств.
    \~english This signal is emitted each time only when adding/removing the serial device from the system.
    The only exception is the case when the first method is called setEnabled(true) - then the signal is automatically forced
    emitted!
    \param[out] list - updated after the add/remove a list of names for serial devices.
*/
