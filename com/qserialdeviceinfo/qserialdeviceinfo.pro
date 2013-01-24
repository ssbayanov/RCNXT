#
#PROJECT         = Serial Device Liblary
TEMPLATE        = lib

CONFIG          += staticlib
#CONFIG          += dll
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = ../build/lib/qserialdeviceinfo/obj
MOC_DIR         = ../build/lib/qserialdeviceinfo/moc

include(qserialdeviceinfo.pri)

unix {
    LIBS        += -ludev
}

DESTDIR         = ../build/lib/qserialdeviceinfo/release
TARGET          = qserialdeviceinfo
