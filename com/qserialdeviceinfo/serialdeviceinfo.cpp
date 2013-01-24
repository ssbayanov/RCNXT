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


/*! \class SerialDeviceInfo

    \brief \~russian Класс SerialDeviceInfo для получения информации о последовательном устройсте в системе.

    \~russian Этот класс является частью библиотеки QSerialDevice и может использоваться совместно с классом AbstractSerial. \n

    С помощью данного класса можно получить информацию об интересующем последовательном устройстве.

    \note В текущей версии - работа класса тестировалась (тестировалась автором) только в операционных системах:
    - MS Windows XP Pro SP3
    - ArchLinux x86_64

    Принцип работы класса в разных операционных системах имеет существенные отличия:
    - в MS Windows XP - информация о последовательном устройстве добывается путем чтения системного реестра.
    - в Linux - для получении информации используется \b UDEV (http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html).

    Информацию о последовательном устройстве класс ищет по имени интересующего устройства, например
    в ОС Windows - это имена COM1 ... COMn, в ОС Linux - это имена /dev/ttyS0 ... /dev/ttySn или /dev/ttyUSBn и т.п.
    Т.е. для получения информации об последовательном устройстве необходимо сначала установить имя этого устройства!

    Начало работы с классом необходимо начинать с создания экземпляра объекта SerialDeviceInfo.\n
    Пример:
    \code
        ...
        SerialDeviceInfo *sdi = new SerialDeviceInfo(this);
        ...
    \endcode

    Далее необходимо установить имя интересующего последовательного устройства:
    - void SerialDeviceInfo::setName(const QString &name). \n
    \note
    1. Имена в ОС Windows должны быть "короткие" (т.е. без префиксов \\\\.\\ и т.п.): COM1 ... COMn (к примеру) \n
    2. Имена в ОС Linux (и в *.nix) должны быть "полные" (т.е. полный путь до устройства): /dev/ttyS0 ... /dev/ttySn (к примеру) \n
    3. Менять имя (переустанавливать новое имя) можно в любое время.\n

    .

    Для очистки состояния внутренних значений (переменных) класса используется метод:
    - void SerialDeviceInfo::clear() - очищает (обнуляет) "кешированное" ранее имя устройства и все сопутствующие внутренние переменные. \n

    Для получения текущего установленного (записанного) имени последовательного устройства используется метод:
    - QString SerialDeviceInfo::systemName() const. \n

    Для получения системного пути к последовательному устройству, используемому системой используется метод:
    - QString SerialDeviceInfo::systemPath(). \n
   
    Для получения информации о расположении используется метод:   
    - QString SerialDeviceInfo::locationInfo(). \n
    
    Для получения информации о драйвере используется метод:  
    - QString SerialDeviceInfo::driver().
    \note Еще не реализовано. \n

    .
        
    Для получения названия подсистемы используется метод:
    - QString SerialDeviceInfo::subSystem(). \n

    Для получения "дружественного" имени последовательного устройства используется метод:
    - QString SerialDeviceInfo::friendlyName(). \n

    Для получения описания последовательного устройства (или драйвера) используется метод:
    - QString SerialDeviceInfo::description(). \n

    Для получения идентификатора последовательного устройства в системе используется метод:
    - QStringList SerialDeviceInfo::hardwareID() - TODO: продумать, нужен или нет!? \n

    Для получения идентификатора производителя последовательного устройства (Vendor ID) используется метод:
    - QString SerialDeviceInfo::vendorID(). \n

    Для получения идентификатора продукта (чипа) последовательного устройства (Product ID) используется метод:
    - QString SerialDeviceInfo::productID(). \n

    Для получения название фирмы-производителя последовательного устройства используется метод:
    - QString SerialDeviceInfo::manufacturer(). \n

    \n

    Для определения наличия последовательного устройства в системе используется метод:
    - bool SerialDeviceInfo::isExists() const. \n
    Если устройство присутствует - то метод вернет \b true. \n

    Для определения установленого методом setName() имени последовательного устройства используется метод:
    - bool SerialDeviceInfo::isEmpty() const - определяет пуста или нет внутренняя переменная класса,
    которая соответствует имени устройства (т.е. было или нет установлено имя методом setName() ). \n
    Если метод setName() не вызывался или имя устройства пустое - то метод вернет \b true. \n

    Для определения занято ли текущее последовательное устройство каким-либо процессом (например открыто и используется), используется метод:
    - bool SerialDeviceInfo::isBusy() const. \n
    Если устройство занято - то метод вернет \b true. \n

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

    \brief \~english Class SerialDeviceInfo to obtain information on a serial devices in the system.

    \~english This class is part of the library QSerialDevice and can be used in conjunction with the class AbstractSerial. \n

    \note In the current version - working class tested (tested by the author) only in operating systems:
    - MS Windows XP Pro SP3
    - ArchLinux x86_64

    The principle of class in different operating systems have significant differences:
    - In MS Windows XP - information about serial device is extracted by reading the registry.
    - In Linux - for obtaining information used \b UDEV (http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html).

    Information about the serial device class is looking for is the interests of the device, such as
    in OS Windows - the names COM1 ... COMn, in the OS Linux - the names /dev/ttyS0 ... /dev/ttySn or /dev/ttyUSBn etc.
    Ie to obtain information about the serial device, you must first set the name of this device!

    Getting Started with the class should begin with the creation of an object instance SerialDeviceInfo. \n
    Example:
    \code
        ...
        SerialDeviceInfo *sdi = new SerialDeviceInfo(this);
        ...
    \endcode

    Next, you need to install the interests of the serial device:
    - void SerialDeviceInfo::setName(const QString &name). \n
    \note
    1. The names in Windows should be "short" (ie without the prefix \\\\.\\, Etc.): COM1 ... COMn (for example) \n
    2. Names in the OS Linux (and *. nix) must be "complete" (ie the full path to the device): /dev/ttyS0 ... /dev/ttySn (for example) \n
    3. Change the name (to reinstall the new name) at any time. \n

    .

    To clear the status of internal values (variables) class method is used:
    - void SerialDeviceInfo::clear() - clears (resets) "cached" before the name of the device and all related internal variables. \n

    For the current set (recorded) name of the serial device using the method:
    - QString SerialDeviceInfo::systemName() const. \n

    For the system path to a serial device, used by the system using the method:
    - QString SerialDeviceInfo::systemPath(). \n

    For information about the location method is used:
    - QString SerialDeviceInfo::locationInfo(). \n
    
    For information about the driver using the method:
    - QString SerialDeviceInfo::driver(). 
    \note Not yet implemented. \n
    
    .
    
    For the names of the subsystem used method:
    - QString SerialDeviceInfo::subSystem(). \n

    For "friendly" name of the serial device using the method:
    - QString SerialDeviceInfo::friendlyName(). \n

    For a description of the serial device (or driver) method is used:
    - QString SerialDeviceInfo::description(). \n

    To obtain the ID of the serial device in the system using the method:
    - QStringList SerialDeviceInfo::hardwareID() - TODO: think, want or not? \n

    To obtain manufacturer ID of the serial device (Vendor ID) method is used:
    - QString SerialDeviceInfo::vendorID(). \n

    For product ID (chip), the serial device (Product ID) method is used:
    - QString SerialDeviceInfo::productID(). \n

    For the name of manufacturer's serial device using the method:
    - QString SerialDeviceInfo:: manufacturer(). \n

    \n

    To determine whether the serial device in the system using the method:
    - bool SerialDeviceInfo::isExists() const. \n
    If the device is present - it will return \b true. \n

    To define the institution of methods setName() name of the serial device using the method:
    - bool SerialDeviceInfo::isEmpty() const - specifies empty or not internal variable class
    which corresponds to the device name (ie, was not found or the name of the method setName()). \n
    If the method setName() not called or the name of the device is empty - it will return \b true. \n

    To determine whether the current employing serial device to any process (eg open and used) method is used:
    - bool SerialDeviceInfo::isBusy() const. \n
    If the device is busy - it will return \b true. \n

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

#include "serialdeviceinfo.h"
#include "serialdeviceinfo_p.h"

//

/*! \fn SerialDeviceInfo::SerialDeviceInfo(QObject *parent)
    \~russian Конструктор по умолчанию.
    \~english Default constructor.
*/
SerialDeviceInfo::SerialDeviceInfo(QObject *parent)
    : QObject(parent), d_ptr(new SerialDeviceInfoPrivate())
{

}

/*! \fn SerialDeviceInfo::~SerialDeviceInfo()
    \~russian Деструктор по умолчанию.
    \~english Default destructor.
*/
SerialDeviceInfo::~SerialDeviceInfo()
{
    delete d_ptr;
}

/*! \fn void SerialDeviceInfo::setName(const QString &name)
    \~russian Устанавливает (присваивает) объекту класса SerialDeviceInfo имя последовательного устройства,
    информацию о котором мы хотим получить.
    \param[in] name - имя интересующего нас последовательного устройства.
    \note
    - в Windows имена должны быть "короткими", т.е. COM1 ... COMn (к примеру)
    - в Linux (и других *.nix) имена должны быть "длинными", т.е. /dev/ttyS0 ... /dev/ttySn (к примеру)
    \~english Sets object class SerialDeviceInfo name of the serial device information about where we want to get.
    \param[in] name - we are interested in the name of the serial device.
    \note
    - Names in Windows should be "short", ie COM1 ... COMn (for example)
    - In Linux (and other *. nix) names should be "long", ie /dev/ttyS0 ... /dev/ttySn (for example)
*/
void SerialDeviceInfo::setName(const QString &name)
{
    d_func()->nativeSetName(name);
}

/*! \fn void SerialDeviceInfo::clear()
    \~russian Очищает (сбрасывает, обнуляет) внутренние переменные объекта класса SerialDeviceInfo.
    \~english Clears (resets) the internal variables of a class object SerialDeviceInfo.
*/
void SerialDeviceInfo::clear()
{
    d_func()->nativeClear();
}

/*! \fn QString SerialDeviceInfo::systemName() const
    \~russian Возвращает имя последовательного устройства с которым сконфигурирован объект класса SerialDeviceInfo.
    \return имя последовательного устройства типа QString или 0 если объект пустой.
    \~english Returns the name of the serial device that is configured with an object of class SerialDeviceInfo.
    \return name of the serial device such as QString, or 0 if the object is empty.
*/
QString SerialDeviceInfo::systemName() const
{
    return d_func()->nativeName();
}

/*! \fn QString SerialDeviceInfo::systemPath()
    \~russian Возвращает путь к устройству, используемый внутри системы
    \return путь путь к устройству типа QString или 0 если объект пустой.
    \~english Returns the path to the device used within the system
    \return the path to the device such as QString, or 0 if the object is empty.
*/
QString SerialDeviceInfo::systemPath()
{
    return d_func()->nativeSystemPath();
}

/*! \fn QString SerialDeviceInfo::subSystem()
    \~russian Возвращает подсистему
    \return подсистема типа QString или 0 если объект пустой.
    \~english Returns the sub system
    \return the sub system such  as QString, or 0 if the object is empty.
*/
QString SerialDeviceInfo::subSystem()
{
    return d_func()->nativeSubSystem();
}

/*! \fn QString SerialDeviceInfo::locationInfo()
    \~russian Возвращает информацию о расположении
    \return информация о расположении типа QString или 0 если объект пустой.
    \~english Returns the location information
    \return the location info such  as QString, or 0 if the object is empty.
*/
QString SerialDeviceInfo::locationInfo()
{
    return d_func()->nativeLocationInfo();
}

/*! \fn QString SerialDeviceInfo::driver()
    \~russian Возвращает информацию о драйвере
    \return информация о драйвере типа QString или 0 если объект пустой.
    \note не реализовано
    \~english Returns the about driver information
    \return the info about driver such  as QString, or 0 if the object is empty.
    \note not implemented
*/
QString SerialDeviceInfo::driver()
{
    return d_func()->nativeLocationInfo();
}

/*! \fn QString SerialDeviceInfo::friendlyName()
    \~russian Возвращает "дружественное" имя последовательного устройства.
    \return
    - "дружественное" имя последовательного устройства в виде QString
    - 0 если "дружественного" имени нет (пустое) или произошла ошибка
    \~english Returns a "friendly" name of the serial device.
    \return
    - "friendly" name of the serial device to a QString
    - 0 if the "friendly" name does not (empty) or an error
*/
QString SerialDeviceInfo::friendlyName()
{
    return d_func()->nativeFriendlyName();
}

/*! \fn QString SerialDeviceInfo::description()
    \~russian Возвращает "описание" последовательного устройства (или драйвера).
    \return
    - "описание" последовательного устройства в виде QString
    - 0 если "описания" нет (пустое) или произошла ошибка
    \~english Returns the "description" of the serial device (or driver).
    \return
    - "description" of the serial device to a QString
    - 0 if the "descriptions" not (empty) or an error
*/
QString SerialDeviceInfo::description()
{
    return d_func()->nativeDescription();
}

/*! \fn QStringList SerialDeviceInfo::hardwareID()
    \~russian TODO: нужно продумать
    \return
    \~english TODO: need to consider
    \return
*/
QStringList SerialDeviceInfo::hardwareID()
{
    return d_func()->nativeHardwareID();
}

/*! \fn QString SerialDeviceInfo::vendorID()
    \~russian Получает Vendor ID последовательного устройства
    \return
    - Vendor ID в виде QString
    - 0 при ошибке или пустом Vendor ID
    \~english Get Vendor ID serial device
    \return
    - Vendor ID in the form QString
    - 0 if error or empty Vendor ID

*/
QString SerialDeviceInfo::vendorID()
{
    return d_func()->nativeVendorID();
}

/*! \fn QString SerialDeviceInfo::productID()
    \~russian Получает Product ID последовательного устройства
    \return
    - Product ID в виде QString
    - 0 при ошибке или пустом Product ID
    \~english Get Product ID serial device
    \return
    - Product ID in the form QString
    - 0 if error or empty Product ID
*/
QString SerialDeviceInfo::productID()
{
    return d_func()->nativeProductID();
}

/*! \fn QString SerialDeviceInfo::manufacturer()
    \~russian Получает производителя оборудования(или драйвера устройства)
    \return
    - Производитель в виде QString
    - 0 при ошибке или пустом Product ID
    \~english  Gets a manufacturer of the device(or device driver)
    \return
    - manufacturer in the form QString
    - 0 if error or empty Product ID
*/
QString SerialDeviceInfo::manufacturer()
{
    return d_func()->nativeManufacturer();
}

/*! \fn bool SerialDeviceInfo::isExists() const
    \~russian Проверяет наличие последовательного устройства в системе в текущий момент.
    \return
    - true - если последовательное устройство присутствует в системе
    - false - если последовательное устройство отсутствует в системе (нет устройства)
    \~english Checks for the serial device in the system at present.
    \return
    - true - if the serial device on the system
    - false - if the serial device is not in the system (no device)
*/
bool SerialDeviceInfo::isExists() const
{
    return d_func()->nativeExists();
}

/*! \fn bool SerialDeviceInfo::isEmpty() const
    \~russian Проверяет конфигурацию объекта класса SerialDeviceInfo на факт того что класс сконфигурирован.
    \return
    - true - если объект класса пустой, т.е. пустое внутреннее имя которое соответствует имени последовательного устройства
    - false - если объект класса не пустой, т.е. сконфигурирован
    \~english Check the configuration of the object class SerialDeviceInfo to the fact that the class is configured.
    \return
    - true - if the object class is empty, ie empty internal name that matches the name of the serial device
    - false - if the object class is not empty, ie configured
*/
bool SerialDeviceInfo::isEmpty() const
{
    return QString(d_func()->nativeName()).isEmpty();
}

/*! \fn bool SerialDeviceInfo::isBusy() const
    \~russian Проверяет занято или нет последовательное устройство в текущий момент.
    \return
    - true - если устройство занято каким-либо процессом (например открыто) или произошла ошибка
    - false - если устройство свободно
    \~english Checks busy or not a serial device at the current moment.
    \return
    - true - if the device is occupied by any process (eg open) or an error
    - false - if the device is free
*/
bool SerialDeviceInfo::isBusy() const
{
    return d_func()->nativeBusy();
}
