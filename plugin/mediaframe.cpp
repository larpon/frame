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

#include "mediaframe.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <QImageReader>
#include <QTime>
#include <QRegularExpression>

//#include <KUrl>
#include <KIO/StoredTransferJob>
#include <KIO/Job>

MediaFrame::MediaFrame(QObject *parent) : QObject(parent)
{
    qsrand(QTime::currentTime().msec());
    
    QList<QByteArray> list = QImageReader::supportedImageFormats();
    //qDebug() << "List" << list;
    for(int i=0; i<list.count(); ++i){
        QString str(list[i].constData());
        m_filters << "*."+str;
    }
    qDebug() << "Added" << list.count() << "filters";
    //qDebug() << m_filters;
    m_watchFile = "";
    QObject::connect(&m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(slotItemChanged(QString)));
    QObject::connect(&m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(slotItemChanged(QString)));
}

MediaFrame::~MediaFrame()
{
}

int MediaFrame::count() const {
    return m_allFiles.count();
}

int MediaFrame::random(int min, int max)
{
    if (min > max)
    {
        int temp = min;
        min = max;
        max = temp;
    }
    
    qDebug() << "random" << min << "<->" << max << "=" << ((qrand()%(max-min+1))+min);
    return ((qrand()%(max-min+1))+min);
}

bool MediaFrame::isDir(const QString &path)
{
    return QDir(path).exists();
}

bool MediaFrame::isDirEmpty(const QString &path)
{
    return (isDir(path) && QDir(path).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0);
}

bool MediaFrame::isFile(const QString &path)
{
    QFileInfo checkFile(path);
    // Check if the file exists and not a directory
    return (checkFile.exists() && checkFile.isFile());
}

void MediaFrame::add(const QString &path)
{
    add(path, false);
}

void MediaFrame::add(const QString &path, bool recursive)
{
    if(has(path))
    {
        qWarning() << "Path" << path << "already exists";
        return;
    }
    
    QUrl url = QUrl(path);
    QString localPath = url.toString(QUrl::PreferLocalFile);
    qDebug() << "Local path" << localPath;
    
    QStringList paths;
    QString filePath;
    
    if(isDir(localPath)) {
        
        if(!isDirEmpty(localPath))
        {
            QDirIterator dirIterator(localPath, m_filters, QDir::Files, (recursive ? QDirIterator::Subdirectories | QDirIterator::FollowSymlinks : QDirIterator::NoIteratorFlags));
            
            while (dirIterator.hasNext()) {
                dirIterator.next();
                
                filePath = dirIterator.filePath();
                paths.append(filePath);
                m_allFiles.append(filePath);
                qDebug() << "Appended" << filePath;
                emit countChanged();
            }
            if(paths.count() > 0)
            {
                m_pathMap[path] = paths;
                qDebug() << "Added" << paths.count() << "files from" << path;
            }
            else
            {
                qWarning() << "No images found in directory" << path;
            }
        }
        else
        {
            qWarning() << "Not adding empty directory" << path;
        }
    
        // the pictures have to be sorted before adding them to the list,
        // because the QDirIterator sorts them in a different way than QDir::entryList
        //paths.sort();
        
    }
    else if(isFile(localPath))
    {
        paths.append(path);
        m_pathMap[path] = paths;
        m_allFiles.append(path);
        qDebug() << "Added" << paths.count() << "files from" << path;
        emit countChanged();
    }
    else
    {
        if (url.isValid() && !url.isLocalFile())
        {
            qDebug() << "Adding" << url.toString() << "as remote file";
            paths.append(path);
            m_pathMap[path] = paths;
            m_allFiles.append(path);
            emit countChanged();
        }
        else
        {
            qWarning() << "Path" << path << "is not a valid file url or directory";
        }
    }
    
}

void MediaFrame::clear()
{
    m_pathMap = QHash<QString, QStringList>();
    m_allFiles = QStringList();
    emit countChanged();
}

void MediaFrame::watch(const QString &path)
{
    QUrl url = QUrl(path);
    QString localPath = url.toString(QUrl::PreferLocalFile);
    if(isFile(localPath))
    {
        if(m_watchFile != "")
        {
            qDebug() << "Removing" << m_watchFile << "from watch list";
            m_watcher.removePath(m_watchFile);
        }
        else
        {
            qDebug() << "Nothing in watch list";
        }
        
        qDebug() << "watching" << localPath << "for changes";
        m_watcher.addPath(localPath);
        m_watchFile = QString(localPath);
    }
    else
    {
        qWarning() << "Can't watch" << path << "for changes";
    }
}

bool MediaFrame::has(const QString &path)
{
    return (m_pathMap.contains(path));
}

void MediaFrame::get(QJSValue callback)
{
    get(callback, false);
}

void MediaFrame::get(QJSValue callback, bool random)
{
    int size = m_allFiles.count() - 1;
    if(size < 1)
    {
        if(size == 0)
        {
            QJSValueList args;
            args << QJSValue(m_allFiles.at(0));
            callback.call(args);
            return;
            //return m_allFiles.at(0);
        }
            
        qWarning() << "No files, returning";
        return;
    }
    
    if(!callback.isCallable())
    {
        qCritical() << "No valid callback supplied, returning";
        return;
    }
    
    QString path = m_allFiles.at(this->random(0, size));
    QUrl url = QUrl(path);
    
    if (url.isValid() && !url.isLocalFile())
    {
        // Try to load the URL
        m_callback = callback;
        m_fileExt = path.section('/', -1);
        qDebug() << "extension" << m_fileExt;
        KIO::StoredTransferJob * job = KIO::storedGet( url, KIO::NoReload, KIO::HideProgressInfo);
        connect(job, SIGNAL(finished(KJob*)), this, SLOT(slotFinished(KJob*)));
        
        //emit pictureLoaded(defaultPicture(i18n("Loading image...")));
    }
    else
    {
        QJSValueList args;
        args << QJSValue(path);
        callback.call(args);
    }
}

void MediaFrame::slotItemChanged(const QString &path)
{
    emit itemChanged(path);
}

void MediaFrame::slotFinished(KJob *job)
{
    QJSValueList args;
    if (job->error()) {
        qCritical() << "Error loading image:" << job->errorString();
        
        args << QJSValue("file://error.png");
        m_callback.call(args);
        //image = defaultPicture(i18n("Error loading image: %1", job->errorString()));
    } else if (KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob *>(job)) {
        QImage image;
        QString path = QDir::temp().absolutePath()+"/"+m_fileExt;
        image.loadFromData(transferJob->data());
        qDebug() << "Successfully downloaded, saving image to" << path;
        image.save(path);
        
        qDebug() << "Saved to" << path;
        
        args << QJSValue(path);
        m_callback.call(args);
    } else
        qDebug() << "Super yuck!";
}
