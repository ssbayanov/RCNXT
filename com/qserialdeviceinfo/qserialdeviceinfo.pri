#

HEADERS         += $$PWD/serialdeviceinfo.h  \
                  $$PWD/serialdeviceinfo_p.h

SOURCES         += $$PWD/serialdeviceinfo.cpp

win32 {
    SOURCES += $$PWD/serialdeviceinfo_win.cpp
}

unix {
    SOURCES += $$PWD/serialdeviceinfo_unix.cpp
}

INCLUDEPATH += $$PWD