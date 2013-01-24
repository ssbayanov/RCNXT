/*
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

#ifndef SERIALDEVICEWATCHER_H
#define SERIALDEVICEWATCHER_H

class SerialDeviceWatcherPrivate;
class SerialDeviceWatcher : public QObject
{
    Q_OBJECT
    
Q_SIGNALS:
    void hasChanged(const QStringList &list);
    
public:
    explicit SerialDeviceWatcher(QObject *parent = 0);
    virtual ~SerialDeviceWatcher();
    
    void setEnabled(bool enable);
    bool isEnabled() const;
    QStringList devicesAvailable() const;
    
protected:
    SerialDeviceWatcherPrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(SerialDeviceWatcher)
    Q_DISABLE_COPY(SerialDeviceWatcher)
    Q_PRIVATE_SLOT(d_func(),void _q_processAvailableNotifier())
};

#endif // SERIALDEVICEWATCHER_H
