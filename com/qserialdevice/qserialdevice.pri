#

HEADERS         += $$PWD/abstractserial.h       \
                   $$PWD/abstractserialengine.h \
                   $$PWD/nativeserialengine.h

SOURCES         += $$PWD/abstractserial.cpp       \
                   $$PWD/abstractserialengine.cpp \
                   $$PWD/nativeserialengine.cpp

win32 {
    HEADERS     += $$PWD/winserialnotifier.h
    SOURCES     += $$PWD/nativeserialengine_win.cpp \
                   $$PWD/winserialnotifier.cpp
}

unix {
    SOURCES     += $$PWD/nativeserialengine_unix.cpp
}

INCLUDEPATH += $$PWD