﻿/*
* This file is part of QSerialDevice, an open-source cross-platform library
* Copyright (C) 2009  Denis Shienkov
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#include <QtCore/private/qwineventnotifier_p.h>



#include "winserialnotifier.h"

#define WINSERIALNOTIFIER_DEBUG

#ifdef WINSERIALNOTIFIER_DEBUG
#include <QtCore/QDebug>
#endif


WinSerialNotifier::WinSerialNotifier(HANDLE hd, Type type, QObject *parent)
    : QObject(parent), shd(hd), wsntype(type), wsnenabled(true), eventMask(0)
{
    if (hd == INVALID_HANDLE_VALUE) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::WinSerialNotifier(...) \n"
                " -> hd == INVALID_HANDLE_VALUE. Error! \n";
#endif
        return;
    }

    ::ZeroMemory(&ovl, sizeof(ovl));

    QT_WA ({
        ovl.hEvent = ::CreateEventW(0, false, false, 0);
    }, {
        ovl.hEvent = ::CreateEventA(0, false, false, 0);
    });

    if (ovl.hEvent == INVALID_HANDLE_VALUE) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::WinSerialNotifier(...) \n"
                " -> function: ::CreateEvent(0, false, false, 0) returned: " << ovl.hEvent << ". Error! \n";
#endif
        return;
    }

    wen = new QWinEventNotifier(ovl.hEvent, this);
    connect(wen, SIGNAL(activated(HANDLE)), this, SLOT(updateNotifier(HANDLE)));

    setEnabled(wsnenabled);
}

/*!
    Destroys the WinSerialNotifier.
*/
WinSerialNotifier::~WinSerialNotifier()
{
    setEnabled(false);

    if (ovl.hEvent) {
        if (::CloseHandle(ovl.hEvent) == 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::~WinSerialNotifier() \n"
                " -> function: ::CloseHandle(ovl.hEvent) returned: 0. Error! \n";
#endif
        }
        ovl.hEvent = 0;
    }
}

void WinSerialNotifier::updateNotifier(HANDLE h)
{
    if (h == ovl.hEvent) {
        switch (wsntype) {
            case Read:
                if ( (eventMask & EV_RXCHAR) == EV_RXCHAR ) {
                    DWORD err = 0;
                    COMSTAT cs; ::ZeroMemory(&cs, sizeof(cs));
                    if (ClearCommError(shd, &err, &cs) == 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::updateNotifier(HANDLE h) \n"
                " -> function: ::ClearCommError(shd, &err, &cs) returned: 0. Error! \n";
#endif
                        break;
                    }
                    if (err != 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::updateNotifier(HANDLE h) \n"
                " -> in function: ::ClearCommError(shd, &err, &cs), \n"
                " output parameter - err: " << err << " \n";
#endif
                        break;
                    }
                    if (cs.cbInQue > 0)
                        emit activated(shd); break;
                }
                break;
            case Write:
                if ( (eventMask & EV_TXEMPTY) == EV_TXEMPTY )
                    emit activated(shd); break;
            default:
                ;
        }
        setEnabled(true);
    }
}

void WinSerialNotifier::setEnabled(bool enable)
{
    if (enable) {
        if (::GetCommMask(shd, &eventMask) == 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::setEnabled(bool enable) \n"
                    " -> function: ::GetCommMask(shd, &eventMask), \n "
                    " eventMask: " <<  eventMask << " returned: 0. Error! \n";
#endif
        }

        switch (wsntype) {
            case Read:
                if ( (eventMask & EV_RXCHAR) == 0 )
                    eventMask |= EV_RXCHAR;
                break;
            case Write:
                if ( (eventMask & EV_TXEMPTY) == 0 )
                    eventMask |= EV_TXEMPTY;
                break;
            default:
                ;
        }

        if (::SetCommMask(shd, eventMask) == 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::setEnabled(bool enable) \n"
                    " -> function: ::SetCommMask(shd, eventMask), \n "
                    " eventMask: " <<  eventMask << " returned: 0. Error! \n";
#endif
        return;
        }

        ::WaitCommEvent(shd, &eventMask, &ovl);
    }

    wen->setEnabled(enable);
/*
        if(::SetCommMask(shd, 0) == 0) {
#if defined (WINSERIALNOTIFIER_DEBUG)
    qDebug() << "Windows: WinSerialNotifier::setEnabled->function SetCommMask(eventMask = " << eventMask << ") returned 0. Error!";
#endif
        }
*/
    wsnenabled = enable;
}
