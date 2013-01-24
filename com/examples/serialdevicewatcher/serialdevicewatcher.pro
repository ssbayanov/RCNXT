#.
#PROJECT         = Serial device watcher example application 
TEMPLATE        = app

CONFIG          += console
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../../build/bin/serialdevicewatcher/obj
MOC_DIR         = ../../build/bin/serialdevicewatcher/moc

DEPENDDIR       = .
INCLUDEDIR      = .

DEPENDPATH      += .
INCLUDEPATH     += ../../qserialdevicewatcher

QMAKE_LIBDIR    += ../../build/lib/qintegratedserialdevice/release

HEADERS         = myclass.h
SOURCES         = main.cpp

LIBS            += -lqserialdevice

unix {
    LIBS            += -ludev
}

DESTDIR         = ../../build/bin/serialdevicewatcher/release
TARGET          = serialdevicewatcher

VERSION         = 0.2.0

win32:RC_FILE   += version.rc