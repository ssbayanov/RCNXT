#.
#PROJECT         = SReader example application with use signals readyRead()
TEMPLATE        = app

CONFIG          += console
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../../build/bin/sreader/obj
MOC_DIR         = ../../build/bin/sreader/moc

DEPENDDIR       = .
INCLUDEDIR      = .

DEPENDPATH      += .
INCLUDEPATH     += ../../qserialdevice

QMAKE_LIBDIR    += ../../build/lib/qintegratedserialdevice/release

HEADERS         += sreader.h
SOURCES         += main.cpp

LIBS            += -lqserialdevice

DESTDIR         = ../../build/bin/sreader/release
TARGET          = sreader

VERSION         = 0.2.0

win32:RC_FILE   += version.rc