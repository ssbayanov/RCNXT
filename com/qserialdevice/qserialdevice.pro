#
#PROJECT        = Serial Device Liblary
TEMPLATE        = lib

CONFIG          += staticlib
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../build/lib/qserialdevice/obj
MOC_DIR         = ../build/lib/qserialdevice/moc

include(qserialdevice.pri)

DESTDIR         = ../build/lib/qserialdevice/release
TARGET          = qserialdevice


VERSION         = 0.1.0
