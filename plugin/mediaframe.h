/*
 *  Copyright 2015  Lars Pontoppidan <dev.larpon@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

#ifndef PICTURESLIDE_H
#define PICTURESLIDE_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QHash>
#include <QFileSystemWatcher>

class MediaFrame : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
    public:
        MediaFrame(QObject *parent = 0);
        ~MediaFrame();
        
        int count() const;
        
        Q_INVOKABLE bool isDir(const QString &path);
        Q_INVOKABLE bool isDirEmpty(const QString &path);
        Q_INVOKABLE bool isFile(const QString &path);
        
        Q_INVOKABLE void add(const QString &path);
        Q_INVOKABLE void add(const QString &path, bool recursive);
        Q_INVOKABLE void clear();
        
        Q_INVOKABLE void watch(const QString &path);
        
        Q_INVOKABLE bool has(const QString &path);
        
        Q_INVOKABLE QString getRandom();
        
    Q_SIGNALS:
        void countChanged();
        void itemChanged(const QString &path);
    
    public Q_SLOTS:
        void slotWrapItemChanged(const QString &path);
        
    private:
        int random(int min, int max);
        
        QStringList m_filters;
        QHash<QString, QStringList> m_pathMap;
        QStringList m_allFiles;
        QString m_watchFile;
        QFileSystemWatcher m_watcher;

};

#endif
