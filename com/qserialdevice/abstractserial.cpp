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


/*! \class AbstractSerial

    \brief \~russian Класс AbstractSerial предоставляет минимальный и достаточный интерфейс для работы с последовательными устройствами.

    \~russian Класс для работы с последовательными устройствами (портами).
    Этот класс является кросс - платформенной "низкоуровневой" библиотекой, которая основана на Qt4 и,
    соответственно, может использоваться для создания приложений Qt4.\n

    Реализация структуры этого класса во многом аналогична реализации QAbstractSocket из поставки Qt4, т.е.
    соответствует идеологии открытых/закрытых классов (pimpl), а также этот класс является наследником класса QIODevice и
    соответственно наследует от него все его методы.\n

    Работа этого класса с последовательным устройством осуществляется в асинхронном режиме!\n

    При использовании в различных операционных системах - используются их родные системные API:
    - в ОС Windows семейств NT/2K/XP и т.д. используется Win32 API.
    - в POSIX подобных ОС используются системные вызовы POSIX.

    Начало работы с классом необходимо начинать с создания экземпляра объекта AbstractSerial.\n
    Пример:
    \code
        ...
        AbstractSerial *serialDevice = new AbstractSerial();
        ...
    \endcode

    Параметры устройства, такие как:\n
    скорость, биты данных, четность, стоп биты, контроль потоком, интервал ожидания символа (в мсек) -
    всегда принимаются по умолчанию соответственно: 9600 8 N 1 10ms !
    Также по умолчанию объекту присвоится первое имя реально существующего в системе устройства.

    \verbatim
        Операционная система:          Имя устройства по умолчанию:
        Windows                        "COM1"
        IRIX                           "/dev/ttyf1"
        HPUX                           "/dev/tty1p0"
        SOLARIS                        "/dev/ttya"
        FREEBSD                        "/dev/ttyd1"
        LINUX                          "/dev/ttyS0"
        <default>                      "/dev/ttyS0"
    \endverbatim

    Далее необходимо устройству(объекту) присвоить реально существующее имя в системе:
    - void AbstractSerial::setDeviceName(const QString &deviceName).

    Для открытия последовательного устройства необходимо вызвать метод:
    - bool AbstractSerial::open(OpenMode mode) - открывает последовательное устройство.\n
    При успешном открытии этот метод вернет \b true.

    Класс AbstractSerial поддерживает только такие опции открытия QIODevice как:
    \b OpenMode::ReadOnly, \b OpenMode::WriteOnly, \b OpenMode::ReadWrite,\n
    при этом для корректной работы необходимо добавлять режим \b OpenMode::Unbuffered.\n
    Пример:
    \code
        ...
        //this example open device as ReadOnly
        bool ret = serialDevice->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
        ...
    \endcode
    \note Последовательное устройство \b ВСЕГДА открывается с параметрами по \b умолчанию : 9600 8 N 1 50ms.\n

    Для закрытия последовательного устройства необходимо вызвать метод:
    - void AbstractSerial::close() - закрывает последовательное устройство.\n

    После того, как устройство успешно открыто можно приступать к его конфигурированию,
    для чего необходимо воспользоваться следующими методами:
    - bool AbstractSerial::setBaudRate(AbstractSerial::BaudRate) - устанавливает скорость обмена данными.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setDataBits(AbstractSerial::DataBits) - устанавливает количество бит данных.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setParity(AbstractSerial::Parity) - устанавливает контроль четности.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setStopBits(AbstractSerial::StopBits) - устанавливает количество стоп бит.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setFlowControl(AbstractSerial::Flow) - устанавливает контроль управления потоком.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setCharIntervalTimeout(int msecs) - устанавливает время ожидания прихода символа при чтении данных.\n
    При успехе метод вернет \b true. \n
    \note Т.к. устройство работает в асинхронном режиме, то достаточно для уверенного чтения данных подобрать опытным путем только этот параметр таймаута \b msecs.
    Этот параметр является единственным параметром при чтении данных, он только зависит от скорости, и выбирать/рассчитывать его можно опытным путем.
    Обычно для уверенного приема достаточна величина \b 10ms для всех скоростей.
    Это упрощает работу с последовательным устройством.
    
    \n
    
    Для чтения/записи данных в порт рекомендуется использовать следующие методы:
    - QByteArray \b AbstractSerial::read(qint64 maxSize) - читает данные из последовательного устройства.\n
    Метод \b read(qint64 maxSize) наследован от QIODevice. Теоретически размер maxSize не ограничен, т.к. метод читает доступное
    количество байт частями.
    Реально в классе AbstractSerial реализован защищенный метод: qint64 readData(char *data, qint64 maxSize) в
    котором определена вся логика чтения данных из устройства. Чтение данных осуществляется таким образом, чтобы
    не терялись данные!
    \note Работа метода: QByteArray \b readAll() не тестировалась. Рекомендуется использовать метод \b read(qint64 maxSize).
    - qint64 \b AbstractSerial::write(const QByteArray &byteArray) - записывает данные в последовательное устройство.\n
    Метод \b write(const QByteArray &byteArray) наследован от QIODevice. Теоретически размер byteArray не ограничен, т.к. метод записывает
    данные по частям. После записи в устройство всех данных излучается сигнал \b bytesWritten(qint64 bytes).
    Реально в классе AbstractSerial реализован защищенный метод: qint64 writeData(const char *data, qint64 maxSize) в
    котором определена вся логика записи данных в устройство.

    Для ожидания прихода данных в последовательное устройство используется метод:
    - bool AbstractSerial::waitForReadyRead(int msecs) - метод ожидает в течении времени msecs прихода в приемный буфер
    последовательного устройства хотя бы одного байта данных.\n
    При успешном завершении ожидания (когда байт пришел до таймаута \b msecs) метод излучает сигнал \b readyRead() и
    возвращает \b true.

    Для очистки буферов последовательного устройства используются методы:
    - bool AbstractSerial::flush() - принудительно ожидает завершения операций чтения/записи и по их окончании
    очищает очереди ввода/вывода.
    При успехе метод вернет \b true.
    - bool AbstractSerial::reset() - не ожидая завершения операций ввода/вывода очищает буферы последовательного
    устройства.
    При успехе метод вернет \b true.

    Для управления линиями DTR и RTS используются методы:
    - bool AbstractSerial::setDtr(bool set) - устанавливает/сбрасывает линию DTR.\n
    При успехе метод вернет \b true.
    - bool AbstractSerial::setRts(bool set) - устанавливает/сбрасывает линию RTS.\n
    При успехе метод вернет \b true.

    Для получения статусов линий CTS, DSR, DCD, RI, RTS, DTR, ST, SR используется метод:
    - ulong AbstractSerial::lineStatus() - возвращает закодированные значения статусов линий побитовым \b ИЛИ (см. enum AbstractSerial::LineStatus). \n

    \verbatim
        CTS  0x01
        DSR  0x02
        DCD  0x04
        RI   0x08
        RTS  0x10
        DTR  0x20
        ST   0x40
        SR   0x80
    \endverbatim

    Для получения текущего количества байт в приемном буфере последовательного устройства готовых для чтения
    используется метод:
    - qint64 AbstractSerial::bytesAvailable() - получает текущее количество байт в приемном буфере.\n

    Для включения/отключения режима излучения сигнала signalStatus() используется метод:
    - void AbstractSerial::enableEmitStatus(bool enable). \n

    Для получения списка имен последовательных устройств доступных в системе, 
    а также слежения за ними используйте класс: SerialDeviceWatcher. \n

    Класс AbstractSerial реализует следующие сигналы:
    - void \b AbstractSerial::readyRead() - излучается \b ОДИН раз при приходе в приемный буфер устройства хотя бы одного байта данных.\n
    Событие о приходе данных контролируется в цикле сообщений, если приходит следующая часть данных (или один байт) - то сигнал не излучается!\n
    Для того, чтобы сигнал вновь излучился необходимо прочитать из буфера данные методом \b read() или сбросить буфер \b reset()!\n
    Также этот сигнал излучается каждый раз при успешном вызове метода waitForReadyRead().
    - void \b AbstractSerial::bytesWritten(qint64 bytes) - излучается при успешном завершении метода \b write().\n
    - void AbstractSerial::signalStatus(const QString &status, QDateTime current) - несет информацию о текущем статусе последовательного
    устройства, а также времени и дате возникновения статуса.
    Излучение сигнала можно включать/отключать при вызове: AbstractSerial::enableEmitStatus(bool enable).\n
    Этот сигнал излучается всякий раз при следующих действиях:
        - при выполнении методов open(), close()
        - при ошибках при конфигурировании
        - при ошибках чтении/записи и т.д.

    \n
    \n
    \n
    
    \warning
    - В этом описании перечислены все реализованные в классе методы.
    Если же не указан реализованный метод - то значит его значение несущественно.
    - По умолчанию класс сконфигурирован таким образом, чтобы записывать в консоль отладочные сообщения.
    Для отключения вывода этих сообщений в консоль - необходимо закомментировать в *.cpp файлах строчки вида:
    \code
        ...
        #define NATIVESERIALENGINE_DEBUG
        ...
    \endcode
    и заново пересобрать библиотеку.

    \brief \~english Class AbstractSerial provides the minimum and adequate interface for working with serial devices.

    \~english Class to work with serial devices (ports).
    This class is a cross-platform low-level library, which is based on Qt4, and
    respectively, can be used to create applications Qt4.\n

    Implementation of this class structure is broadly similar to the implementation of the supply QAbstractSocket Qt4, ie
    corresponds to the ideology of public/private classes  (pimpl), and this class is a descendant of QIODevice and
    respectively inherits from him all his methods.\n

    The work of this class with a serial device is in asynchronous mode!\n

    When used in a variety of operating systems - used their native system API:
    - On Windows NT/2K/XP family, etc. use Win32 API.
    - In the POSIX like OS system calls used by POSIX.

    Getting Started with the class should begin with the creation of an object instance AbstractSerial.\n
    Example:
    \code
        ...
        AbstractSerial *serialDevice = new AbstractSerial()
        ...
    \endcode

    Device parameters, such as: \n
    speed, data bits, parity, stop bits, flow control, timed character (in milliseconds) -
    always accepted by default, respectively: 9600 8 N 1 10ms!
    Also by default, be assigned to the object first name actually exists in the system device.

    \verbatim
        Operating system:          The device name default:
        Windows                    "COM1"
        IRIX                       "/dev/ttyf1"
        HPUX                       "/dev/tty1p0"
        SOLARIS                    "/dev/ttya"
        FREEBSD                    "/dev/ttyd1"
        LINUX                      "/dev/ttyS0"
        <default>                  "/dev/ttyS0"
    \endverbatim

    Next, you need a device (object) to assign a name actually exists in the system:
    - void AbstractSerial::setDeviceName(const QString &deviceName).

    To open a serial device, you must call the method:
    - bool AbstractSerial::open(QIODevice::OpenMode).

    If successful, the opening of this method returns \b true.
    Class AbstractSerial supports only option open QIODevice as:
    \b OpenMode::ReadOnly, \b OpenMode::WriteOnly, \b OpenMode::ReadWrite,
    while for the correct operation mode to add \b OpenMode::Unbuffered
    Example:
    \code
        ...
        //this example open device as ReadOnly
        bool ret = serialDevice->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
        ...
    \endcode
    \note the serial device \b always opens with the parameters for default: 9600 8 N 1 10ms

    To close the serial device need to call the method:
    - void AbstractSerial::close()

    Once the device successfully opened, you are ready to configure it.
    To do this, use the following methods:
    - bool AbstractSerial::setBaudRate(AbstractSerial::BaudRate) - sets the speed of data exchange.\n
    With the success of the method returns \b true.
    - bool AbstractSerial::setDataBits(AbstractSerial::DataBits) - sets the number of data bits.\n
    With the success of the method returns \b true.
    - bool AbstractSerial::setParity(AbstractSerial::Parity) - sets the parity. \n
    With the success of the method returns \b true.
    - bool AbstractSerial::setStopBits(AbstractSerial::StopBits) - sets the number of stop bits. \n
    With the success of the method returns \b true.
    - bool AbstractSerial::setFlowControl(AbstractSerial::Flow) - sets the control flow control.\n
    With the success of the method returns \b true.
    - bool AbstractSerial::setCharIntervalTimeout(int msecs) - sets the waiting time of arrival character when reading data. \n
    With the success of the method returns \b true. \n
    Since device operates in asynchronous mode, it is sufficient for a confident reading of data to select only experienced by this parameter timeout.
    This option is the only option when reading dannh, it just depends on the speed and choose/calculate it can be empirically.
    Typically, for sure receive sufficient magnitude \b 10ms for all speeds.
    It simplifies work with a serial device.

    To read/write data to the port is recommended to use the following methods:
    - QByteArray \b AbstractSerial::read(qint64 maxSize) - reads data from the serial device. \n
    Method \b read() inherited from QIODevice. Theoretically, the size \b maxSize is not limited, because method reads the available
    number of bytes in parts.
    Really class AbstractSerial implemented a secure method: qint64 readData(char *data, qint64 maxlen) in
    which defines all the logic of reading data from the device. Reading data is carried out in such a way that
    not lose data!
    \note work method: QByteArray \b readAll() not tested. It is recommended to use the method \b read()
    - qint64 \b AbstractSerial::write(const QByteArray &byteArray) - writes data to a serial device.\n
    Method \b write() inherited from QIODevice. Theoretically, the size byteArray is not limited, because method records
    data in parts. After recording all data in the device emitted a signal \b bytesWritten(qint64 bytes).
    Really class AbstractSerial implemented a secure method: qint64 writeData(const char *data, qint64 maxSize) in
    which defines all the logic of writing data to the device.

    In anticipation of the return data in the serial device using the method:
    - bool AbstractSerial::waitForReadyRead(int msecs)\n
    The method waits for the arrival time \b msecs in the receive buffer of the serial device at least one byte of data.
    If successful, the expectation (when B came to a timeout msecs) method emits a signal \b readyRead() and
    returns \b true.

    To clear the buffer of the serial device used methods:
    - bool AbstractSerial::flush() - forced forward to the completion of read/write and at their end
    clears the I/O queues
    - bool AbstractSerial::reset() - not waiting for the completion of I/O clears the serial buffer
    device.

    To control lines DTR and RTS methods are used:
    - bool AbstractSerial::setDtr(bool set)
    - bool AbstractSerial::setRts(bool set)

    If successful, the method returns \b true.

    For the status lines CTS, DSR, DCD, RI, RTS, DTR, ST, SR method is used:
    - ulong AbstractSerial::lineStatus().
    The method returns an encoded value status lines bitwise \b OR (see. enum AbstractSerial::LineStatus). \n

    \verbatim
        CTS     0x01
        DSR     0x02
        DCD     0x04
        RI      0x08
        RTS     0x10
        DTR     0x20
        ST      0x40
        SR      0x80
    \endverbatim

    For the current number of bytes in the receive buffer the serial device is ready for reading
    method is used:
    - qint64 AbstractSerial::bytesAvailable().\n

    To enable/disable the radiation signal \b signalStatus() method is used:
    - void AbstractSerial::enableEmitStatus(bool enable).

    For a list of names for serial devices available in the system, as well as tracking them use the class: SerialDeviceWatcher. \n

    Class AbstractSerial implements the following signals:
    - void \b AbstractSerial::readyRead() - emitted once at the arrival in the receiving buffer device at least one byte of data.\n
    Event of the arrival of data is controlled in the message loop.
    When comes the next part of the data (or B) - the signal is not radiated! In order to re-emitted signal
    must read the data from the buffer by the method \b read() or reset the buffer reset()!
    Also, this signal is emitted each time a successful call to the method waitForReadyRead().
    - void \b AbstractSerial::bytesWritten(qint64 bytes) - emitted when the successful completion of the method \b write().
    - void AbstractSerial::signalStatus(const QString &status, QDateTime current) - carries information about the current status of the serial
    devices, as well as time and date of status. Is emitted whenever the following actions:
        - The performance of methods open(), close()
        - On error when configuring
        - On error reading/writing, etc.

    The emission signal can be switched on/off when you call: AbstractSerial::enableEmitStatus(bool enable).

    \n
    \n
    \n
    
    \warning
    - In this description lists all the methods implemented in the classroom.
    If you do not specify the method implemented - that means its value is immaterial.
    - By default, the class is configured in such a way as to write to the console debug messages.
    To disable these messages in the console - need to comment out in the *. cpp files, a line like:
    \code
    ...
    #define NATIVESERIALENGINE_DEBUG
    ...
    \endcode
    and re-rebuild library.

    
    \version \~ 0.2.0

    \author \~ Denis Shienkov
    - ICQ       : 321789831
    - e-mail    : scapig2@yandex.ru

    \~russian
    \note
    - В версии 0.1.0. работа библиотеки тестировалась автором только с: Windows XP SP3 Pro и ArchLinux x86_64.
    - Библиотекой теоретически должны поддерживаться такие ОС как: все семейство Windows NT/2k/XP , а также POSIX совместимые.
    \~english
    \note
    - In a version 0.1.0. work of library was tested by author only with: Windows XP Sp3 Pro and Archlinux x86_64.
    - In theory must a library be supported such OS as: all of family of Windows Nt/2k/xp, and also POSIX compatible.
*/

#include <QtCore/QStringList>
#include <QtCore/QAbstractEventDispatcher>

#include "abstractserialengine.h"

#define ABSTRACTSERIAL_DEBUG

#ifdef ABSTRACTSERIAL_DEBUG
#include <QtCore/QDebug>
#endif


#ifdef Q_OS_WIN32
    const char AbstractSerial::defaultDeviceName[] = "COM1";
#elif defined(Q_OS_IRIX)
    const char AbstractSerial::defaultDeviceName[] = "/dev/ttyf1";
#elif defined(Q_OS_HPUX)
    const char AbstractSerial::defaultDeviceName[] = "/dev/tty1p0";
#elif defined(Q_OS_SOLARIS)
    const char AbstractSerial::defaultDeviceName[] = "/dev/ttya";
#elif defined(Q_OS_FREEBSD)
    const char AbstractSerial::defaultDeviceName[] = "/dev/ttyd1";
#elif defined(Q_OS_LINUX)
    const char AbstractSerial::defaultDeviceName[] = "/dev/ttyS0";
#else
    const char AbstractSerial::defaultDeviceName[] = "/dev/ttyS0";
#endif


AbstractSerialPrivate::AbstractSerialPrivate()
    :
    waitForReadyReadCalled(false),
    emittedReadyRead(false),
    emittedBytesWritten(false),
    emittedStatus(false),
    serialEngine(0)
{
}

AbstractSerialPrivate::~AbstractSerialPrivate()
{
}

void AbstractSerialPrivate::resetSerialLayer()
{
    if (serialEngine) {
        delete serialEngine;
        serialEngine = 0;
    }
}

bool AbstractSerialPrivate::initSerialLayer()
{
    Q_Q(AbstractSerial);
    serialEngine = AbstractSerialEngine::createSerialEngine(q);
    if (serialEngine) return true;
    return false;
}

void AbstractSerialPrivate::setupSerialNotifiers()
{
    Q_Q(AbstractSerial);
    if (serialEngine) {
        switch ((q->openMode()) & (~QIODevice::Unbuffered)) {
            case QIODevice::ReadOnly:
                serialEngine->setReadNotificationEnabled(true);
                QObject::connect(serialEngine, SIGNAL(readNotification()), q, SLOT(canReadNotification()));
                break;
            case QIODevice::WriteOnly:
                break;
            case QIODevice::ReadWrite:
                serialEngine->setReadNotificationEnabled(true);
                QObject::connect(serialEngine, SIGNAL(readNotification()), q, SLOT(canReadNotification()));
                break;
            default:
                ;
        }
    }
}

void AbstractSerialPrivate::enableSerialNotifiers(bool enable)
{
    if (serialEngine) {
        serialEngine->setReadNotificationEnabled(enable);
    }
}

bool AbstractSerialPrivate::canReadNotification()
{
    Q_Q(AbstractSerial);
    if (waitForReadyReadCalled) {
        emittedReadyRead = true;
        emit q->readyRead();
        waitForReadyReadCalled = false;
        return true;
    }
    if (!emittedReadyRead) {
        emittedReadyRead = true;
        emit q->readyRead();
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------

/*! \fn AbstractSerial::AbstractSerial(QObject *parent)
    \~russian Конструктор по умолчанию. Создает объект, ассоциированный с реальным последовательным устройством в системе.
    Например:
    - для Windows - это COM1, COM2 ... COMn
    - для Linux - это /dev/ttyS0, /dev/ttyS1 и т.д.
    \note  Если не указать (пропустить) в конструкторе параметр deviceName - то по умолчанию устройству присвоится имя = DEFAULT_DEVICE_NAME
    \~english Default constructor. Creates an object, associated with the real serial device in the system.
    For example: for Windows - it COM1, COM2 ... COMn, for Linux - it /dev/ttyS0, /dev/ttyS1 etc
    \note If not to specify (to skip) the parameter of devicename in a constructor - that by default the name will be appropriated a device = DEFAULT_DEVICE_NAME
    \~
    \verbatim
        Operating system:       Name DEFAULT_DEVICE_NAME:
        Windows                 "COM1"
        IRIX                    "/dev/ttyf1"
        HPUX                    "/dev/tty1p0"
        SOLARIS                 "/dev/ttya"
        FREEBSD                 "/dev/ttyd1"
        LINUX                   "/dev/ttyS0"
        <default>               "/dev/ttyS0"
    \endverbatim
*/
AbstractSerial::AbstractSerial(QObject *parent)
    : QIODevice(parent), d_ptr(new AbstractSerialPrivate())
{
    Q_D(AbstractSerial);
    d->q_ptr = this;
    if (!d->initSerialLayer()) return;
    initialiseHash();
}

/*! \fn AbstractSerial::~AbstractSerial()
    \~russian Деструктор по умолчанию.
    \~english Default destructor.
*/
AbstractSerial::~AbstractSerial()
{
    Q_D(AbstractSerial);
    d->resetSerialLayer();
    delete d_ptr;
}

/*! \fn void AbstractSerial::initialiseHash()
    \~russian Инициализирует хэш скоростей, и т.д. Вызывается автоматически при инициализации класса.
    \~english Initializes the hash speeds, etc. Caused automatically during initializing of class.
*/
void AbstractSerial::initialiseHash()
{
    //filling m_baudRateHash
#ifndef Q_OS_WIN
    m_baudRateHash[QObject::tr("50 baud")] = AbstractSerial::BaudRate50;
    m_baudRateHash[QObject::tr("75 baud")] = AbstractSerial::BaudRate75;
#endif
    m_baudRateHash[QObject::tr("110 baud")] = AbstractSerial::BaudRate110;
#ifndef Q_OS_WIN
    m_baudRateHash[QObject::tr("134 baud")] = AbstractSerial::BaudRate134;
    m_baudRateHash[QObject::tr("150 baud")] = AbstractSerial::BaudRate150;
    m_baudRateHash[QObject::tr("200 baud")] = AbstractSerial::BaudRate200;
#endif
    m_baudRateHash[QObject::tr("300 baud")] = AbstractSerial::BaudRate300;
    m_baudRateHash[QObject::tr("600 baud")] = AbstractSerial::BaudRate600;
    m_baudRateHash[QObject::tr("1200 baud")] = AbstractSerial::BaudRate1200;
#ifndef Q_OS_WIN
    m_baudRateHash[QObject::tr("1800 baud")] = AbstractSerial::BaudRate1800;
#endif
    m_baudRateHash[QObject::tr("2400 baud")] = AbstractSerial::BaudRate2400;
    m_baudRateHash[QObject::tr("4800 baud")] = AbstractSerial::BaudRate4800;
    m_baudRateHash[QObject::tr("9600 baud")] = AbstractSerial::BaudRate9600;
#ifdef Q_OS_WIN
    m_baudRateHash[QObject::tr("14400 baud")] = AbstractSerial::BaudRate14400;
#endif
    m_baudRateHash[QObject::tr("19200 baud")] = AbstractSerial::BaudRate19200;
    m_baudRateHash[QObject::tr("38400 baud")] = AbstractSerial::BaudRate38400;
#ifdef Q_OS_WIN
    m_baudRateHash[QObject::tr("56000 baud")] = AbstractSerial::BaudRate56000;
#endif
    m_baudRateHash[QObject::tr("57600 baud")] = AbstractSerial::BaudRate57600;
/*
#ifdef Q_OS_WIN
    m_baudRateHash[AbstractSerial::BaudRate76800]=QObject::tr("76800 baud");
#endif
*/
    m_baudRateHash[QObject::tr("115200 baud")] = AbstractSerial::BaudRate115200;
#ifdef Q_OS_WIN
    m_baudRateHash[QObject::tr("128000 baud")] = AbstractSerial::BaudRate128000;
    m_baudRateHash[QObject::tr("256000 baud")] = AbstractSerial::BaudRate256000;
#endif
    //filling m_dataBitsHash
    m_dataBitsHash[QObject::tr("5 bit")] = AbstractSerial::DataBits5;
    m_dataBitsHash[QObject::tr("6 bit")] = AbstractSerial::DataBits6;
    m_dataBitsHash[QObject::tr("7 bit")] = AbstractSerial::DataBits7;
    m_dataBitsHash[QObject::tr("8 bit")] = AbstractSerial::DataBits8;
    //filling m_parityHash
    m_parityHash[QObject::tr("None")] = AbstractSerial::ParityNone;
    m_parityHash[QObject::tr("Odd")] = AbstractSerial::ParityOdd;
    m_parityHash[QObject::tr("Even")] = AbstractSerial::ParityEven;
    m_parityHash[QObject::tr("Mark")] = AbstractSerial::ParityMark;
    m_parityHash[QObject::tr("Space")] = AbstractSerial::ParitySpace;
    //filling m_stopBitsHash
    m_stopBitsHash[QObject::tr("1")] = AbstractSerial::StopBits1;
    m_stopBitsHash[QObject::tr("1.5")] = AbstractSerial::StopBits1_5;
    m_stopBitsHash[QObject::tr("2")] = AbstractSerial::StopBits2;
    //filling flowsHash
    m_flowHash[QObject::tr("Disable")] = AbstractSerial::FlowControlOff;
    m_flowHash[QObject::tr("Hardware")] = AbstractSerial::FlowControlHardware;
    m_flowHash[QObject::tr("Xon/Xoff")] = AbstractSerial::FlowControlXonXoff;
}

/*! \fn bool AbstractSerial::isValid() const
*/
bool AbstractSerial::isValid() const
{
    Q_D(const AbstractSerial);
    return (d->serialEngine) ? true : false;
}

/*! \fn void AbstractSerial::setDeviceName(const QString &deviceName)
    \~russian Присваивает созданному объекту имя последовательного устройства, с которым необходимо работать.
    \param[in] deviceName - имя последовательного устройства, реально имеющегося в системе.\n
    Например: для Windows - это COM1, COM2 ... COMn, для Linux - это /dev/ttyS0, /dev/ttyS1 и т.д.
    \note Использовать этот метод необходимо только ПЕРЕД открытием устройства!
    \~english Appropriates the created object the serial device with which it is necessary to work name.
    \param[in] deviceName - serial device, really present in the system name.\n
    For example: for Windows - it COM1, COM2 ... COMn, for Linux - it /dev/ttyS0, /dev/ttyS1 etc
    \note  Uses this method is necessary only BEFORE opening of device!
*/
void AbstractSerial::setDeviceName(const QString &deviceName)
{
    Q_D(AbstractSerial);
    if (isValid()) d->serialEngine->setDeviceName(deviceName);
}

/*! \fn QString AbstractSerial::deviceName() const
    \~russian Возвращает имя последовательного устройства, с которым сконфигурирован объект.
    \return
    - Имя последовательного устройства, с которым сконфигурирован объект в виде QString
    - 0 в случае ошибки
    \note Использовать этот метод можно в любой момент после создания объекта, т.е. неважно открыто реально устройство или нет.
    \~english Returns the serial device which an object is configured with name.
    \return
    - Serial device with which an object is configured as Qstring name
    - 0 in the case of error
    \note Uses this method is possible at any moment after creation of object, I.e. it is unimportant openly really device or not.
*/
QString AbstractSerial::deviceName() const
{
    Q_D(const AbstractSerial);
    return (isValid()) ? d->serialEngine->deviceName() : QString("%1").arg(0);
}

/*! \fn bool AbstractSerial::setBaudRate(BaudRate baudRate)
    \~russian Устанавливает для последовательного устройства скорость в бодах.
    \param[in] baudRate - желаемая скорость последовательного устройства в бодах из перечисления.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b baudRate будет игнорироваться)
    \~english Sets a baud rate for a serial device
    \param[in] baudRate -  desired baud rate of serial device from enumeration.
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of BaudRate will be ignored otherwise)
*/
bool AbstractSerial::setBaudRate(BaudRate baudRate)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setBaudRate(baudRate);
        (ret) ? emitStatusString(ENoneSetBaudRate) : emitStatusString(ESetBaudRate);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setBaudRate(const QString &baudRate)
    \overload
    \~russian Устанавливает для последовательного устройства скорость в бодах.
    \param[in] baudRate - желаемая скорость последовательного устройства в бодах в виде строки.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b baudRate будет игнорироваться)
    \~english Sets a baud rate for a serial device
    \param[in] baudRate -  desired baud rate of serial device as an string
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of BaudRate will be ignored otherwise)
*/
bool AbstractSerial::setBaudRate(const QString &baudRate)
{
    int res = m_baudRateHash.value(baudRate);
    if (!res) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::setBaudRate(const QString &) returned false because input parameter speed:" <<  baudRate << "\n is not defined in hash. Error!";
#endif
        emitStatusString(ENoneSetBaudRate);
        return false;
    }
    return setBaudRate(BaudRate(res));
}

/*! \fn QString AbstractSerial::baudRate() const
    \~russian Возвращает скорость в ботах в виде строки с которой сконфигурировано последовательное устройство.
    \return
    - Скорость в виде строки QString
    - 0 - в случае ошибки
    \~english  Returns baud rate as a string which a serial device is configured with.
    \return
    - baud rate as an string QString
    - 0 - in the case of error
*/
QString AbstractSerial::baudRate() const
{
    Q_D(const AbstractSerial);
    QString res = 0;
    if (isValid()) {
        res = m_baudRateHash.key(d->serialEngine->baudRate());
        if (res.isEmpty()) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::baudRate() -> hash is empty. Error!";
#endif
        }
        return res;
    }
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::baudRate() -> object AbstractSerial is invalid. Error!";
#endif
    return res;
}

/*! \fn QStringList AbstractSerial::listBaudRate() const
    \~russian Возвращает текстовый список всех скоростей, поддерживаемых классом AbstractSerial.
    \return Список скоростей в виде QStringList
    \~english Returns the text list of all of speeds (baud rates), supported the class of Abstractserial.
    \return List of speeds (baud rates) as Qstringlist
*/
QStringList AbstractSerial::listBaudRate() const
{
    return m_baudRateHash.keys();
}

/*! \fn bool AbstractSerial::setDataBits(DataBits dataBits)
    \~russian Устанавливает для последовательного устройства количество бит данных.
    \param[in] dataBits - желаемое количество бит данных последовательного устройства из перечисления.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b dataBits будет игнорироваться)
    \~english Sets a data bits for a serial device
    \param[in] dataBits - desired data bits of serial device from enumeration.
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of dataBits will be ignored otherwise)
*/
bool AbstractSerial::setDataBits(DataBits dataBits)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setDataBits(dataBits);
        (ret) ? emitStatusString(ENoneSetDataBits) : emitStatusString(ESetDataBits);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setDataBits(const QString &dataBits)
    \overload
    \~russian Устанавливает для последовательного устройства количество бит данных.
    \param[in] dataBits - желаемое количество бит данных последовательного устройства в виде строки
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b dataBits будет игнорироваться)
    \~english Sets a data bits for a serial device
    \param[in] dataBits -  desired data bits of serial device as an string
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of dataBits will be ignored otherwise)
*/
bool AbstractSerial::setDataBits(const QString &dataBits)
{
    int res = m_dataBitsHash.value(dataBits);
    if (!res) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::setDataBits(const QString &) returned false because input parameter databits:" <<  dataBits << "\n is not defined in hash. Error!";
#endif
        emitStatusString(ENoneSetDataBits);
        return false;
    }
    return setDataBits(DataBits(res));
}

/*! \fn QString AbstractSerial::dataBits() const
    \~russian Возвращает количество бит данных в виде строки с которым сконфигурировано последовательное устройство.
    \return
    - Количество бит данных в виде строки QString
    - 0 - в случае ошибки
    \~english  Returns data bits as a string which a serial device is configured with.
    \return
    - data bits as an string QString
    - 0 - in the case of error
*/
QString AbstractSerial::dataBits() const
{
    Q_D(const AbstractSerial);
    QString res = 0;
    if (isValid()) {
        res = m_dataBitsHash.key(d->serialEngine->dataBits());
        if (res.isEmpty()) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::dataBits() -> hash is empty. Error!";
#endif
        }
        return res;
    }
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::dataBits() -> object AbstractSerial is invalid. Error!";
#endif
    return res;
}

/*! \fn QStringList AbstractSerial::listDataBits() const
    \~russian Возвращает текстовый список всех типов бит данных, поддерживаемых классом AbstractSerial.
    \return Список бит данных в виде QStringList
    \~english Returns the text list of all of data bits, supported the class of Abstractserial.
    \return List of data bits as Qstringlist
*/
QStringList AbstractSerial::listDataBits() const
{
    return m_dataBitsHash.keys();
}

/*! \fn bool AbstractSerial::setParity(Parity parity)
    \~russian Устанавливает для последовательного устройства тип контроля четности.
    \param[in] parity - желаемый тип контроля четности последовательного устройства из перечисления.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b parity будет игнорироваться)
    \~english Sets a parity for a serial device
    \param[in] parity - desired parity of serial device from enumeration.
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of parity will be ignored otherwise)
*/
bool AbstractSerial::setParity(Parity parity)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setParity(parity);
        (ret) ? emitStatusString(ENoneSetParity) : emitStatusString(ESetParity);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setParity(const QString &parity)
    \overload
    \~russian Устанавливает для последовательного устройства тип контроля четности.
    \param[in] parity - желаемый тип контроля четности последовательного устройства в виде строки
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b parity будет игнорироваться)
    \~english Sets a parity for a serial device
    \param[in] parity -  desired parity of serial device as an string
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of parity will be ignored otherwise)
*/
bool AbstractSerial::setParity(const QString &parity)
{
    int res = m_parityHash.value(parity);
    if (!res) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::setParity(const QString &) returned false because input parameter parity:" <<  parity << "\n is not defined in hash. Error!";
#endif
        emitStatusString(ENoneSetParity);
        return false;
    }
    return setParity(Parity(res));
}

/*! \fn QString AbstractSerial::parity() const
    \~russian Возвращает тип контроля четности в виде строки с которым сконфигурировано последовательное устройство.
    \return
    - Тип контроля четности в виде строки QString
    - 0 - в случае ошибки
    \~english  Returns parity as a string which a serial device is configured with.
    \return
    - parity as an string QString
    - 0 - in the case of error
*/
QString AbstractSerial::parity() const
{
    Q_D(const AbstractSerial);
    QString res = 0;
    if (isValid()) {
        res = m_parityHash.key(d->serialEngine->parity());
        if (res.isEmpty()) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::parity() -> hash is empty. Error!";
#endif
        }
        return res;
    }
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::parity() -> object AbstractSerial is invalid. Error!";
#endif
    return res;
}

/*! \fn QStringList AbstractSerial::listParity() const
    \~russian Возвращает текстовый список всех типов контроля четности , поддерживаемых классом AbstractSerial.
    \return Список контроля четности в виде QStringList
    \~english Returns the text list of all of paritys, supported the class of Abstractserial.
    \return List of paritys as Qstringlist
*/
QStringList AbstractSerial::listParity() const
{
    return m_parityHash.keys();
}

/*! \fn bool AbstractSerial::setStopBits(StopBits stopBits)
    \~russian Устанавливает для последовательного устройства количество стоп-бит.
    \param[in] stopBits - желаемое количество стоп-бит последовательного устройства из перечисления.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b stopBits будет игнорироваться)
    \~english Sets a stop bits for a serial device
    \param[in] stopBits - desired stop bits of serial device from enumeration.
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of stopBits will be ignored otherwise)
*/
bool AbstractSerial::setStopBits(StopBits stopBits)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setStopBits(stopBits);
        (ret) ? emitStatusString(ENoneSetStopBits) : emitStatusString(ESetStopBits);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setStopBits(const QString &stopBits)
    \overload
    \~russian Устанавливает для последовательного устройства количество стоп-бит.
    \param[in] stopBits - желаемое количество стоп-бит последовательного устройства в виде строки
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b stopBits будет игнорироваться)
    \~english Sets a stop bits for a serial device
    \param[in] stopBits -  desired stop bits of serial device as an string
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of stopBits will be ignored otherwise)
*/
bool AbstractSerial::setStopBits(const QString &stopBits)
{
    int res = m_stopBitsHash.value(stopBits);
    if (!res) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::setStopBits(const QString &) returned false because input parameter stopbits:" <<  stopBits << "\n is not defined in hash. Error!";
#endif
        emitStatusString(ENoneSetStopBits);
        return false;
    }
    return setStopBits(StopBits(res));
}

/*! \fn QString AbstractSerial::stopBits() const
    \~russian Возвращает количество стоп-бит в виде строки с которым сконфигурировано последовательное устройство.
    \return
    - Количество стоп-бит в виде строки QString
    - 0 - в случае ошибки
    \~english  Returns stop bits as a string which a serial device is configured with.
    \return
    - stop bits as an string QString
    - 0 - in the case of error
*/
QString AbstractSerial::stopBits() const
{
    Q_D(const AbstractSerial);
    QString res = 0;
    if (isValid()) {
        res = m_stopBitsHash.key(d->serialEngine->stopBits());
        if (res.isEmpty()) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::stopBits() -> hash is empty. Error!";
#endif
        }
        return res;
    }
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::stopBits() -> object AbstractSerial is invalid. Error!";
#endif
    return res;
}

/*! \fn QStringList AbstractSerial::listStopBits() const
    \~russian Возвращает текстовый список всех типов стоп-бит, поддерживаемых классом AbstractSerial.
    \return Список стоп-бит в виде QStringList
    \~english Returns the text list of all of stop bits, supported the class of Abstractserial.
    \return List of stop bits as Qstringlist
*/
QStringList AbstractSerial::listStopBits() const
{
    return m_stopBitsHash.keys();
}

/*! \fn bool AbstractSerial::setFlowControl(Flow flow)
    \~russian Устанавливает для последовательного устройства режим управления потоком.
    \param[in] flow - желаемый тип управления потоком последовательного устройства из перечисления.
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b flow будет игнорироваться)
    \~english Sets a flow control for a serial device
    \param[in] flow - desired flow control of serial device from enumeration.
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of flow will be ignored otherwise)
*/
bool AbstractSerial::setFlowControl(Flow flow)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setFlowControl(flow);
        (ret) ? emitStatusString(ENoneSetFlow) : emitStatusString(ESetFlowControl);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setFlowControl(const QString &flow)
    \overload
    \~russian Устанавливает для последовательного устройства режим управления потоком.
    \param[in] flow - желаемый тип управления потоком последовательного устройства в виде строки
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b flow будет игнорироваться)
    \~english Sets a flow control bits for a serial device
    \param[in] flow -  desired flow of serial device as an string
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of flow will be ignored otherwise)
*/
bool AbstractSerial::setFlowControl(const QString &flow)
{
    int res = m_flowHash.value(flow);
    if (!res) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::setFlowControl(const QString &) returned false because input parameter flow:" <<  flow << "\n is not defined in hash. Error!";
#endif
        emitStatusString(ENoneSetFlow);
        return false;
    }
    return setFlowControl(Flow(res));
}

/*! \fn QString AbstractSerial::flowControl() const
    \~russian Возвращает режим управления потоком в виде строки с которым сконфигурировано последовательное устройство.
    \return
    - Режим управления потоком в виде строки QString
    - 0 - в случае ошибки
    \~english  Returns flow control as a string which a serial device is configured with.
    \return
    - flow control as an string QString
    - 0 - in the case of error
*/
QString AbstractSerial::flowControl() const
{
    Q_D(const AbstractSerial);
    QString res = 0;
    if (isValid()) {
        res = m_flowHash.key(d->serialEngine->flow());
        if (res.isEmpty()) {
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::flowControl() -> hash is empty. Error!";
#endif
        }
        return res;
    }
#if defined (ABSTRACTSERIAL_DEBUG)
    qDebug() << "AbstractSerial::flowControl() -> object AbstractSerial is invalid. Error!";
#endif
    return res;
}

/*! \fn QStringList AbstractSerial::listFlowControl() const
    \~russian Возвращает текстовый список всех режимов управления потоком, поддерживаемых классом AbstractSerial.
    \return Список режимов управления потоком в виде QStringList
    \~english Returns the text list of all of flow controls, supported the class of Abstractserial.
    \return List of flow controls as Qstringlist
*/
QStringList AbstractSerial::listFlowControl() const
{
    return m_flowHash.keys();
}

/*! \fn bool AbstractSerial::setCharIntervalTimeout(int msecs)
    \~russian Устанавливает время ожидания прихода символа в приемный буфер последовательного устройства.
    \param[in] msecs - желаемое время ожидания прихода символа, в мсек
    \return
    - true - при успехе
    - false - в случае ошибки
    \note Использовать этот метод можно \b ТОЛЬКО \b ПОСЛЕ \b ТОГО \b КАК \b УСТРОЙСТВО \b ОТКРЫЛИ! (иначе параметр \b msecs будет игнорироваться)
    \~english Fixes time expectation of arrival of character in the receiving buffer of serial device.
    \param[in] msecs - desired time of expectation of arrival of character, in msek
    \return
    - true - at success
    - false - at an error
    \note Uses this method is possible ONLY Since DEVICE was OPENED! (the parameter of msecs will be ignored otherwise)
*/
bool AbstractSerial::setCharIntervalTimeout(int msecs)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setCharIntervalTimeout(msecs);
        (ret) ? emitStatusString(ENoneSetCharTimeout) : emitStatusString(ESetCharIntervalTimeout);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn int AbstractSerial::charIntervalTimeout() const
    \~russian Возвращает время ожидания прихода символа в приемный буфер последовательного устройства с которым сконфигурировано последовательное устройство.
    \return
    - Время ожидания прихода символа, в мсек
    - 0 - в случае ошибки
    \~english  Returns time of expectation of arrival of character in the receiving buffer of serial device which a serial device is configured with.
    \return
    - Time of expectation of arrival of character, in msek
    - 0 - in the case of error
*/
int AbstractSerial::charIntervalTimeout() const
{
    Q_D(const AbstractSerial);
    return (isValid()) ? d->serialEngine->charIntervalTimeout() : 0;
}

/*! \fn bool AbstractSerial::setDtr(bool set)
    \~russian Устанавливает линию DTR в состояное "high" или "low" в зависимости от входного параметра
    \param[in] set - желаемое состояние линии DTR
    \return
    - true - при успехе
    - false - в случае ошибки
    \~english Sets DTR line an state "high" or "low"
    \param[in] set - desired state of line DTR
    \return
    - true - at success
    - false - at an error
*/
bool AbstractSerial::setDtr(bool set)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setDtr(set);
        if (!ret)
            emitStatusString(ESetDtr);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::setRts(bool set)
    \~russian Устанавливает линию RTS в состояное "high" или "low" в зависимости от входного параметра
    \param[in] set - желаемое состояние линии RTS
    \return
    - true - при успехе
    - false - в случае ошибки
    \~english Sets DTR line an state "high" or "low"
    \param[in] set - desired state of line RTS
    \return
    - true - at success
    - false - at an error
*/
bool AbstractSerial::setRts(bool set)
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->setRts(set);
        if (!ret)
            emitStatusString(ESetRts);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn ulong AbstractSerial::lineStatus()
    \~russian Возвращает статусы линий CTS, DSR, DCD, RI, RTS, DTR, ST, SR. Метод возвращает закодированные значения статусов линий побитовым ИЛИ.
    \return
    - true - при успехе
    - false - в случае ошибки
    \~english Returned the status lines CTS, DSR, DCD, RI, RTS, DTR, ST, SR. The method returns an encoded value status lines bitwise OR.
    \return
    - true - at success
    - false - at an error
*/
ulong AbstractSerial::lineStatus()
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->lineStatus();
        if (!ret)
            emitStatusString(ELineStatus);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::open(OpenMode mode)
    \~russian Открывает последовательное устройство.
    \param[in] mode - режим открытия последовательного устройства
    \return
    - true - при успехе
    - false - в случае ошибки
    \note
    - Последовательное устройство открывается с параметрами по умолчанию в режиме 9600 8 N 1, и 10 мс время ожидания символа.
    После того, как устройство успешно открыто - можно приступать к его конфигурированию!
    - Устройство правильно работает ТОЛЬКО при использовании режима QIODevice::Unbuffered
    \~english Opens a serial device.
    \param[in] mode - mode of opening of serial device
    \return
    - true - at success
    - false - in the case of error
    \note
    - A serial device is opened with parameters by default in the mode 9600 8 N 1, and 10 ms time of expectation of character.
    Since a device is successful openly - it is possible to proceed to his configuring!
    - A device correctly works ONLY at the use of the mode of QIODevice::Unbuffered
*/
bool AbstractSerial::open(OpenMode mode)
{
    Q_D(AbstractSerial);

    if (!isOpen()) {
        if (isValid()) {
            if (d->serialEngine->open(mode)) {

                QIODevice::open(mode);

                if (QAbstractEventDispatcher::instance(thread()))
                    d->setupSerialNotifiers();
                emitStatusString(ENoneOpen);
                return true;
            }
        }//isValid
        emitStatusString(EOpen);
        return false;
    }
    emitStatusString(EDeviceIsOpen);
    return false;
}

/*! \fn void AbstractSerial::close()
    \~russian Закрывает последовательное устройство.
    \~english Closes a serial device
*/
void AbstractSerial::close()
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        d->serialEngine->close();
        emitStatusString(ENoneClose);
    }
    else
        emitStatusString(EClose);
    QIODevice::close();
}

/*! \fn bool AbstractSerial::flush()
    \~russian Очищает буферы последовательного устройства.
    \return
    - true - при успехе
    - false - в случае ошибки
    \~english Clears the buffers of serial device.
    \return
    - true - at success
    - false - at an error
*/
bool AbstractSerial::flush()
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->flush();
        if (!ret)
            emitStatusString(EFlush);
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn bool AbstractSerial::reset()
    \~russian Сбрасывает буферы последовательного устройства.
    \return
    - true - при успехе
    - false - в случае ошибки
    \~english Reset the buffers of serial device.
    \return
    - true - at success
    - false - at an error
*/
bool AbstractSerial::reset()
{
    Q_D(AbstractSerial);
    if (isOpen()) {
        bool ret = d->serialEngine->reset();
/*
        if (!ret)
            emitStatusString(EFlush);
*/
        return ret;
    }
    emitStatusString(EDeviceIsNotOpen);
    return false;
}

/*! \fn qint64 AbstractSerial::bytesAvailable() const
    \~russian Возвращает количество байт готовых для чтения, которые находятся во входном буфере последовательного устройства
    \return
    - n - количество байт при успехе
    - -1 - в случае ошибки
    \~english A byte returns an amount ready for reading,
    \return
    - n - is an amount byte at success
    - -1 - in the case of error
*/
qint64 AbstractSerial::bytesAvailable() const
{
    Q_D(const AbstractSerial);
    if (isOpen()) {
        qint64 ret = d->serialEngine->bytesAvailable(true);
        /*
        if (ret >= 0) {
        }
        else
            emitStatusString(EBytesAvailable);
        */
        return ret;
    }
    //emitStatusString(EDeviceIsNotOpen);
    return -1;
}

/*! \fn bool AbstractSerial::isSequential() const
    \~russian
    \~english
*/
bool AbstractSerial::isSequential() const
{
    return true;
}

/*
   Returns the difference between msecs and elapsed. If msecs is -1,
   however, -1 is returned.
*/
static int qt_timeout_value(int msecs, int elapsed)
{
    if (msecs == -1)
        return -1;
    int timeout = msecs - elapsed;
    return (timeout < 0) ? 0 : timeout;
}

/*! \fn bool AbstractSerial::waitForReadyRead(int msecs)
    \~russian Ожидает прихода во входной буфер последовательного устройства хотя-бы одного байта данных в течении определенного времени.
    При успешном выполнении излучается сигнал readyRead().
    \param[in] msecs - время в течении которого ожидается приход символа, в мсек
    \return
    - true - при успехе
    - false - в случае ошибки или при таймауте
    \~english Expects arrival in the entrance buffer of serial device of even one byte of data in the flow of set time.
    The signal of readyRead() emitted at successful implementation().
    \param[in] msecs - time in the flow of which is expected arrival of character, in msek
    \return
    - true - at success
    - false - in the case of error or at a timeout
*/
bool AbstractSerial::waitForReadyRead(int msecs)
{
    Q_D(AbstractSerial);

    if (!isOpen()) {
        emitStatusString(EDeviceIsNotOpen);
        return false;
    }

    QTime stopWatch;
    stopWatch.start();

    forever {
        bool readyToRead = false;
        bool readyToWrite = false;
        if (!d->serialEngine->waitForReadOrWrite(&readyToRead, &readyToWrite, true, false,
                                                    qt_timeout_value(msecs, stopWatch.elapsed()))) {
            emitStatusString(EWaitReadyReadTimeout);
            return false;
        }
        if (readyToRead) {
            if (bytesAvailable() > 0) {
                d->waitForReadyReadCalled = true;
                if (d->canReadNotification()) {
                    return true;
                }
            }//bytesAvailable
        }//readyToRead
    }
}

/*! \fn bool AbstractSerial::waitForBytesWritten(int msecs)
    \~russian Ожидает ухода из выходного буфера последовательного устройства последнего символа в течении определенного времени.
    \note В данной версии библиотеки этот метод пока не реализован.
    \~english Expects a care from the output buffer of serial device of the last character in the flow of set time.
    \note In this version of library this method while is not realized.
*/
bool AbstractSerial::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs)
    return false;
}

/*! \fn qint64 AbstractSerial::readData(char *data, qint64 maxSize)
    \~russian Читает пакет данных из входного буфера последовательного устройства.
    \param[out] *data - указатель на буфер (массив) в который происходит чтение пакета данных
    \param[in] maxSize - количество байт, которые необходимо прочитать
    \return
    - n - количество реально прочитанных байт - при успехе
    - -1 - в случае ошибки
    \~english Reads the package of data from the input buffer of serial device.
    \param[out] *data - a pointer to the buffer (array) in which takes a place reading of package of data
    \param[in] maxSize - an amount is a byte, which must be read
    \return
    - n - an amount is real read byte - at success
    - -1 - in the case of error
*/
qint64 AbstractSerial::readData(char *data, qint64 maxSize)
{
    Q_D(AbstractSerial);
    qint64 ret = d->serialEngine->read(data, maxSize);
    if (ret < 0)
        emitStatusString(EReadDataIO);
    d->emittedReadyRead = false;
    return ret;
}

/*! \fn qint64 AbstractSerial::writeData(const char *data, qint64 maxSize)
    \~russian Передает пакет данных в выходной буфер последовательного устройства. При успешном выполнении излучается сигнал bytesWritten().
    \param[in] *data - указатель на буфер (массив) из которого происходит передача пакета данных
    \param[in] maxSize - количество байт, которые необходимо передать
    \return
    - n - количество реально переданных байт - при успехе
    - -1 - в случае ошибки
    \~english Passes the package of data in the output buffer of serial device. The signal of bytesWritten() emitted at successful implementation.
    \param[in] *data - a pointer to the buffer (array) from which takes a place write of package of data
    \param[in] maxSize - an amount is a byte, which must be write
    \return
    - n - an amount is real write byte - at success
    - -1 - in the case of error
*/
qint64 AbstractSerial::writeData(const char *data, qint64 maxSize)
{
    Q_D(AbstractSerial);
    qint64 ret = d->serialEngine->write(data, maxSize);
    if (ret < 0)
        emitStatusString(EWriteDataIO);
    if (ret >= 0)
        emit bytesWritten(ret);
    return ret;
}

/*! Converted from status value to string
*/
QString AbstractSerial::statusToString(Status val) const
{
    switch (val) {
        case AbstractSerial::ENone: return QObject::tr("No errors.");
        case AbstractSerial::ENoneOpen: return QObject::tr("Opened::Device is successfully opened. OK!");
        case AbstractSerial::ENoneClose: return QObject::tr("Closed::Device is successfully closed. OK!");
        case AbstractSerial::ENoneSetBaudRate: return QObject::tr("Controls::Baud rate is successfully set. OK!");
        case AbstractSerial::ENoneSetParity: return QObject::tr("Controls::Parity is successfully set. OK!");
        case AbstractSerial::ENoneSetDataBits: return QObject::tr("Controls::Data bits is successfully set. OK!");
        case AbstractSerial::ENoneSetStopBits: return QObject::tr("Controls::Stop bits is successfully set. OK!");
        case AbstractSerial::ENoneSetFlow: return QObject::tr("Controls::Flow is successfully set. OK!");
        case AbstractSerial::ENoneSetCharTimeout: return QObject::tr("Controls::Char timeout is successfully set. OK!");
        case AbstractSerial::EOpen: return QObject::tr("Error opening. Error!");
        case AbstractSerial::EDeviceIsNotOpen: return QObject::tr("Device is not open. Error!");
        case AbstractSerial::EOpenModeUnsupported: return QObject::tr("Opened::Opened mode unsupported. Error!");
        case AbstractSerial::EOpenModeUndefined: return QObject::tr("Opened::Opened mode undefined. Error!");
        case AbstractSerial::EOpenInvalidFD: return QObject::tr("Opened::Invalid device descriptor. Error!");
        case AbstractSerial::EOpenOldSettingsNotSaved: return QObject::tr("Opened::Fail saved old settings. Error!");
        case AbstractSerial::EOpenGetCurrentSettings: return QObject::tr("Opened::Fail get current settings. Error!");
        case AbstractSerial::EOpenSetDefaultSettings: return QObject::tr("Opened::Fail set default settings. Error!");
        case AbstractSerial::EDeviceIsOpen: return QObject::tr("Device is already open. Error!");
        case AbstractSerial::ECloseSetOldSettings: return QObject::tr("Closed::Fail set old settings. Error!");
        case AbstractSerial::ECloseFD: return QObject::tr("Closed::Fail close device descriptor. Error!");
        case AbstractSerial::EClose: return QObject::tr("Closed::Fail close device. Error!");
        case AbstractSerial::ESetBaudRate: return QObject::tr("Parameters::Set baud rate fail. Error!");
        case AbstractSerial::ESetDataBits: return QObject::tr("Parameters::Set data bits fail. Error!");
        case AbstractSerial::ESetParity: return QObject::tr("Parameters::Set parity fail. Error!");
        case AbstractSerial::ESetStopBits: return QObject::tr("Parameters::Set stop bits fail. Error!");
        case AbstractSerial::ESetFlowControl: return QObject::tr("Parameters::Set flow control fail. Error!");
        case AbstractSerial::ESetCharIntervalTimeout: return QObject::tr("Parameters::Set char interval timeout. Error!");
        case AbstractSerial::EBytesAvailable: return QObject::tr("Controls::Get bytes available fail. Error!");
        case AbstractSerial::ESetDtr: return QObject::tr("Controls::Set DTR fail. Error!");
        case AbstractSerial::ESetRts: return QObject::tr("Controls::Set RTS fail. Error!");
        case AbstractSerial::ELineStatus: return QObject::tr("Controls::Get lines status fail. Error!");
        case AbstractSerial::EWaitReadyReadIO: return QObject::tr("Controls::Wait for ready read from device - i/o problem. Error!");
        case AbstractSerial::EWaitReadyReadTimeout: return QObject::tr("Controls::Wait for ready read timeout. Error!");
        case AbstractSerial::EWaitReadyWriteIO: return QObject::tr("Controls::Wait for bytes writtten to device - i/o problem. Error!");
        case AbstractSerial::EWaitReadyWriteTimeout: return QObject::tr("Controls::Wait for bytes writtten timeout. Error!");
        case AbstractSerial::EReadDataIO: return QObject::tr("Controls::Read data from device - i/o problem. Error!");
        case AbstractSerial::EWriteDataIO: return QObject::tr("Controls::Write data to device - i/o problem. Error!");
        case AbstractSerial::EFlush: return QObject::tr("Controls::Flush fail. Error!");
        default: return QObject::tr("AbstractSerial::statusToString -> Status mode undefined. Error!");
    }
}

/*! \fn void AbstractSerial::enableEmitStatus(bool enable)
    \~russian Включает или отключает флаг испускания сигнала signalStatus()
    \param[in] enable - желаемое значение флага
    \note если:
    - enable = true - то это значит, что включить испускание сигнала signalStatus()
    - enable = false - то это значит, что отключить испускание сигнала signalStatus()
    \~english On or Off the flag of emitting of signal of signalStatus()
    \param[in] enable - desired value of flag
    \note if:
    - enable = true -  it means that to On emitting of signal of signalStatus()
    - enable = false -  it means that to Off emitting of signal of signalStatus()
*/
void AbstractSerial::enableEmitStatus(bool enable)
{
    Q_D(AbstractSerial);
    d->emittedStatus = enable;
}

/*! \fn bool AbstractSerial::canEmitStatusString() const
    \~russian Возвращает значение флага, который определяет испускать или нет сигнал signalStatus()
    \return Значение флага
    \note если:
    - true - то это значит, что испускание сигнала signalStatus() включено
    - false - то это значит, что испускание сигнала signalStatus() отключено
    \~english Returns the value of flag which determines to emit or not signal of signalStatus()
    \note if:
    - true - it means that emitting of signal of signalStatus() it is On
    - false - it means that emitting of signal of signalStatus() it is Off.
*/
bool AbstractSerial::canEmitStatusString() const
{
    Q_D(const AbstractSerial);
    return d->emittedStatus;
}

/*! \fn void AbstractSerial::emitStatusString(Status status)
    \~russian Испускает сигнал signalStatus()
    \param[in] status - тип статуса
    \~english  Emits the signal of signalStatus()
    \param[in] status - type of status
*/
void AbstractSerial::emitStatusString(Status status)
{
    if (canEmitStatusString())
        emit signalStatus(statusToString(status), QDateTime::currentDateTime());
}

/*! \fn AbstractSerial::signalStatus(const QString &status, QDateTime current)
    \~russian Этот сигнал испускается всякий раз при изменении статуса объекта  AbstractSerial при вызове его методов из секций public и protected.
    Изменение статуса может быть вызвано следующими причинами:
    - при ошибках конфигурировании устройства
    - при его открытии/закрытии
    - при ошибках чтения/записи данных и т.п.
    \param[out] status - текущий статус последовательного устройства
    \param[out] current - текущее значение даты/времени произошедшей смены статуса
    \~english This signal is emitted every time at the change of status of object  of Abstractserial at the call of his methods from the sections of public and protected.
    The change of status can be caused the followings reasons:
    - at errors configuring of device
    - at his opening/closing
    - at the errors of reading/writting of data etc.

    \~
    \note \~russian Применять этот сигнал можно в GUI приложении например таким образом:
    \note \~english Applying this signal is possible in GUI application for example thus:
    \~
    \code
        ...
        connect( this, SIGNAL(signalStatus(const QString &, QDateTime)), widget, SLOT(update(const QString &, QDateTime)) );
        ...
        void widget::update(const QString &msg, QDateTime dt)
        {
            ...
            QString dtString = dt.time().toString();
            QString str = msg + ", in time:" + dtString;
            ...
            listWidget->addItem(str);
            ...
        }
    \endcode
*/
