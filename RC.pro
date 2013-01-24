QT       += core gui


TARGET = RC
TEMPLATE = app

RESOURCES     = rc.qrc

SOURCES += main.cpp\
        rcform.cpp\
        BtNXT.cpp\
        MyBlock.cpp\
        com/qserialdevice/abstractserial.cpp\
        com/qserialdevice/abstractserialengine.cpp\
        com/qserialdevice/nativeserialengine.cpp\
        com/qserialdevice/nativeserialengine_win.cpp\
        com/qserialdevice/winserialnotifier.cpp\
#        com/qserialdevice/nativeserialengine_unix.cpp\
#        com/qserialdevice/nativeserialnotifier_unix.cpp\
#        com/qserialdevice/ttylocker.cpp\
#        com/qserialdevice/abstractserialnotifier.cpp


HEADERS  += rcform.h\
            BtNXT.h \
    MyBlock.h

FORMS    += rcform.ui
