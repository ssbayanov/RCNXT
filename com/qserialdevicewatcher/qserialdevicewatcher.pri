#

HEADERS         += $$PWD/serialdevicewatcher.h  \
                  $$PWD/serialdevicewatcher_p.h

SOURCES         += $$PWD/serialdevicewatcher.cpp

win32 {
    SOURCES     += $$PWD/serialdevicewatcher_p_win.cpp
}

unix {
    SOURCES     += $$PWD/serialdevicewatcher_p_unix.cpp
}

INCLUDEPATH += $$PWD