#.
#PROJECT         = Serial device info example application 
TEMPLATE        = app

CONFIG          += console
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../../build/bin/serialdeviceinfo/obj
MOC_DIR         = ../../build/bin/serialdeviceinfo/moc

DEPENDDIR       = .
INCLUDEDIR      = .

DEPENDPATH      += .
INCLUDEPATH     += ../../qserialdeviceinfo

QMAKE_LIBDIR    += ../../build/lib/qintegratedserialdevice/release

SOURCES         = main.cpp

LIBS            += -lqserialdevice

unix {
    LIBS            += -ludev
}

DESTDIR         = ../../build/bin/serialdeviceinfo/release
TARGET          = serialdeviceinfo

VERSION         = 0.2.0

win32:RC_FILE   += version.rc