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

#ifndef MEDIAFRAME_H
#define MEDIAFRAME_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QHash>
#include <QFileSystemWatcher>
#include <QJSValue>
#include <QProcess>

#include <KIO/Job>

class MediaFrame : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QUrl currentUrl READ currentUrl NOTIFY currentUrlChanged)    // For cached files, this is the online source URL
    Q_PROPERTY(bool random READ random WRITE setRandom NOTIFY randomChanged)

    Q_PROPERTY(bool useCustomCommand WRITE setUseCustomCommand)
    Q_PROPERTY(QString customCommand WRITE setCustomCommand)

    public:

        enum AddOption
        {
            NON_RECURSIVE,
            RECURSIVE
        };
        Q_ENUMS(AddOption)

        MediaFrame(QObject *parent = nullptr);
        virtual ~MediaFrame();

        int count() const;
        QUrl currentUrl() const;

        bool random() const;
        void setRandom(bool random);

        void setUseCustomCommand(bool val);
        void setCustomCommand(QString val);

        Q_INVOKABLE bool isDir(const QString &path);
        Q_INVOKABLE bool isDirEmpty(const QString &path);
        Q_INVOKABLE bool isFile(const QString &path);

        Q_INVOKABLE void add(const QString &path);
        Q_INVOKABLE void add(const QString &path, AddOption option);
        Q_INVOKABLE void clear();

        Q_INVOKABLE void watch(const QString &path);

        Q_INVOKABLE bool isAdded(const QString &path);

        Q_INVOKABLE void requestNext();

        Q_INVOKABLE void pushHistory(const QString &string);
        Q_INVOKABLE QString popHistory();
        Q_INVOKABLE int historyLength();

        Q_INVOKABLE void pushFuture(const QString &string);
        Q_INVOKABLE QString popFuture();
        Q_INVOKABLE int futureLength();

        Q_INVOKABLE void showDocumentInfo();

    Q_SIGNALS:
        void countChanged();
        void currentUrlChanged();
        void randomChanged();

        void itemChanged(const QString &path);
        void nextItemGotten(const QString &filePath, const QString &errorMessage);

    private Q_SLOTS:
        void slotItemChanged(const QString &path);
        void slotDownloadFinished(KJob *job);
        void slotCommandFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void slotNextUriGotten(const QString &path);

    private:
        int random(int min, int max);
        QString getCacheDirectory();
        QString hash(const QString &str);

        QStringList m_filters;
        QHash<QString, QStringList> m_pathMap;
        QStringList m_allFiles;
        QString m_watchFile;
        QFileSystemWatcher m_watcher;

        QStringList m_history;
        QStringList m_future;

        QUrl m_currentUrl;  // often local
        QString m_filename;
        QProcess *m_customCommandProc = nullptr;

        bool m_random = false;
        int m_next = 0;

        bool m_useCustomCommand = false;
        QString m_customCommand = "";
};

#endif
