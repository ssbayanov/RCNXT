#.
#PROJECT         = Reader example application with use waitForReadyRead()
TEMPLATE        = app

CONFIG          += console 
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../../build/bin/reader/obj
MOC_DIR         = ../../build/bin/reader/moc

DEPENDDIR       = .
INCLUDEDIR      = .

DEPENDPATH      += .
INCLUDEPATH     += ../../qserialdevice

QMAKE_LIBDIR    += ../../build/lib/qintegratedserialdevice/release

SOURCES         = main.cpp 

LIBS            += -lqserialdevice

DESTDIR         = ../../build/bin/reader/release
TARGET          = reader

VERSION         = 0.2.0

win32:RC_FILE   += version.rc