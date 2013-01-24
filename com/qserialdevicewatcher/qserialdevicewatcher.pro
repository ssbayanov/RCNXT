#
#PROJECT        = Serial Device Liblary
TEMPLATE        = lib

CONFIG          += staticlib
#CONFIG          += dll
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../build/lib/qserialdevicewatcher/obj
MOC_DIR         = ../build/lib/qserialdevicewatcher/moc

include(qserialdevicewatcher.pri)

unix {
    LIBS        += -ludev
}

DESTDIR         = ../build/lib/qserialdevicewatcher/release
TARGET          = qserialdevicewatcher
