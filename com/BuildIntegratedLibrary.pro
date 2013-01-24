#
#PROJECT        = Integrated Serial Device Liblary
TEMPLATE        = lib

CONFIG          += staticlib
#CONFIG          += dll
CONFIG          -= debug_and_release debug
QT              -= gui

OBJECTS_DIR     = build/lib/qintegratedserialdevice/obj
MOC_DIR         = build/lib/qintegratedserialdevice/moc

include(qserialdevice/qserialdevice.pri)
include(qserialdeviceinfo/qserialdeviceinfo.pri)
include(qserialdevicewatcher/qserialdevicewatcher.pri)

unix {
    LIBS        += -ludev
}

DESTDIR         = build/lib/qintegratedserialdevice/release
TARGET          = qserialdevice

VERSION         = 0.1.0
