#.
#PROJECT         = Writer example application
TEMPLATE        = app

CONFIG          += console 
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../../build/bin/writer/obj
MOC_DIR         = ../../build/bin/writer/moc

DEPENDDIR       = .
INCLUDEDIR      = .

DEPENDPATH      += .
INCLUDEPATH     += ../../qserialdevice

QMAKE_LIBDIR    += ../../build/lib/qintegratedserialdevice/release

SOURCES         = main.cpp 

LIBS            += -lqserialdevice

DESTDIR         = ../../build/bin/writer/release
TARGET          = writer

VERSION         = 0.2.0

win32:RC_FILE   += version.rc
