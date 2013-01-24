QSerialSevice v 0.2.0

Contents:
1. About
2. Building
3. Installation
4. License
5. Authors
6. Links
----------------




1. About the project.
~~~~~~~~~~~~

QSerialDevice - a cross-platform library for serial
devices (ports) based on Qt4 (http://qt.nokia.com/).

Attention:
In the current version - guaranteed free operation (tested by the author) only in operating systems:
- Windows XP Pro Sp3
- GNU/Linux
so in other cases, the work is not guaranteed!

1.1 About the classes.

The library contains the following classes:

AbstractSerial      - a class that provides an interface for
                      with serial devices.
SerialDeviceWatcher - a class that provides an interface
                      notification (tracking) for the serial device.
SerialDeviceInfo    - a class that provides an interface
                      for information on serial devices.
                          
(Detailed description see the documentation)

The library can be compiled in an integrated form (ie all classes is linked into one library)
and in differential form (ie each class is linked to a separate library).

1.2 About the examples.

Library in its structure contains console example programs in the directory /examples,
utilities:

writer                  - is intended to demonstrate the use of libraries when writing data to the serial device.

reader                  - is intended to demonstrate the use of the library when reading data from the
                          serial device using the method: waitForReadyRead().

sreader                 - intended to demonstrate the use of the library when reading data from the
                          serial device using a signal: readyRead().
    
serialdeviceinfo        - intended to demonstrate the use of libraries for
                          information about the serial devices.
                          
serialdevicewatcher     - intended to demonstrate the use of libraries for the notification (tracking)
                          for serial devices.

(More details on the building of the library and examples, see below in the section number 2 "Building")

1.3 About the Documentation.

Documentation: (doc/ru/html or doc/en/html) performed in two languages: English and Russian.
(note: generated using Doxygen)




2. Building
~~~~~~~~~~~~

2.1 Requirements.

To build the project in OS Windows and GNU/Linux (for example) require mandatory conditions:
- Must be installed library Qt4 > 4.5.0 and correctly set its own environment variables.
- The compiler must be installed and configured its environment variables.

Note: For OS GNU/Linux to build classes SerialDeviceWatcher and SerialDeviceInfo
must be installed UDEV (http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html)

2.2 Building the Library.

Note: By default, the library is going with the option: staticlib, so
all the recommendations of assembling the library and examples, which are given below,
are given only for the static build libraries.

    2.2.1 Compiling the library in an integrated form.
    
    1. Unpack the source tarball of the project.
    2. Navigate to the directory with the source code of the project:
        # cd <QSerialDeviceProjectDir>
    3. The following commands:
        # qmake BuildIntegratedLibrary.pro
        # make
        
    Result:
    in the directory: <QSerialDeviceProjectDir>/build/lib/qintegratedserialdevice/release
    will be binary: libqserialdevice.a libraries.
    
    2.2.2 Compiling libraries in differential form.
    
    1. Unpack the source tarball of the project.
    2. Navigate to the directory with the source code of the project:
        # cd <QSerialDeviceProjectDir>
    3. The following commands:
        # qmake BuildDifferentiatedLibrary.pro
        # make
        
    Result:
    in the directories:
    <QSerialDeviceProjectDir>/Build/lib/qserialdevice/release
    <QSerialDeviceProjectDir>/Build/lib/qserialdeviceinfo/release
    <QSerialDeviceProjectDir>/Build/lib/qserialdevicewatcher/release
    appear, respectively, the binary library files.
    
    2.2.2 Compiling examples.
    
    Note: Before building the examples must be building library in an integrated form (see Section 2.2.1)
    
    1. We assume that we have unpacked the source code and compiled the library in an integrated form.
    2. The following commands:
        # qmake BuildExamples.pro
        # make
        
    Result:
    in the directories:
    <QSerialDeviceProjectDir>/Build/bin/reader/release
    <QSerialDeviceProjectDir>/Build/bin/sreader/release
    <QSerialDeviceProjectDir>/Build/bin/writer/release
    <QSerialDeviceProjectDir>/Build/bin/serialdeviceinfo/release
    <QSerialDeviceProjectDir>/Build/bin/serialdevicewatcher/release
    appear respectively executable binary files are examples.
    
2.3 Building the documentation.

To build the documentation in Russian - Use file Doxyfile_RU, English - Doxyfile_EN




3. Installation
~~~~~~~~~~~~




4. License
~~~~~~~~~~~~~
GPL

Copyright (C) 2009 Denis Shienkov <scapig2@yandex.ru>

See file: copying




5. Authors
~~~~~~~~~~~~~

Developer: Denis Shienkov
ICQ: 321789831
e-mail: scapig2@yandex.ru




6. Links
~~~~~~~~~~~~~

Project's source code v 0.2.0 can be downloaded here:
http://fireforge.net/projects/qserialdevice/

Source code for the latest revision of SVN here:
http://fireforge.net/snapshots.php?group_id=199

Qt4 here: http://qt.nokia.com/

UDEV here: http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html