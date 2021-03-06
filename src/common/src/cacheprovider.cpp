#include "cacheprovider.h"

#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QStandardPaths>

CacheProvider::CacheProvider(QObject *parent) :
    QObject(parent)
{
    this->m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    const int clearInterval = 1000 * 60 * 60;

    this->m_clearTimer.setInterval(clearInterval);
    this->m_clearTimer.setSingleShot(false);
    QObject::connect(&this->m_clearTimer, &QTimer::timeout, this, &CacheProvider::clearCache);
}

bool CacheProvider::cacheFileExists(const QString &identifier)
{
    const QString filePath = getPathForIdentifier(identifier);
    QFileInfo fileInfo(filePath);
    return fileInfo.exists();
}

bool CacheProvider::isFileCurrent(const QString &identifier)
{
    if (!cacheFileExists(identifier))
        return false;

    const QString filePath = getPathForIdentifier(identifier);
    QFileInfo fileInfo(filePath);
    const bool isCurrent = (QDateTime::currentDateTime().addDays(-5) <
                            fileInfo.lastModified());
    return isCurrent;
}

QFile* CacheProvider::getCacheFile(const QString &identifier, QFile::OpenMode mode)
{
    const QString filePath = getPathForIdentifier(identifier);
    const QFileInfo fileInfo(filePath);

    QDir cacheDir = fileInfo.absoluteDir();
    if (!cacheDir.exists()) {
        const QString targetCacheDir = cacheDir.absolutePath();
        if (!cacheDir.mkpath(targetCacheDir)) {
            qWarning() << "Failed to create cache directory" << targetCacheDir;
            return Q_NULLPTR;
        }
    }

    QFile* cacheFile = new QFile(filePath, this);
    if (!cacheFile->open(mode)) {
        qWarning() << "Failed to open" << filePath << "in mode" << mode;
        return Q_NULLPTR;
    }
    return cacheFile;
}

void CacheProvider::clearCache()
{
    bool cacheFileRemoved = false;
    QDirIterator dirIterator(this->m_cacheDir, QDirIterator::Subdirectories);
    while(dirIterator.hasNext()) {
        const QString cacheFilePath = dirIterator.next();
        const QFileInfo cacheFile(cacheFilePath);
        if (!cacheFile.isFile())
            continue;

        if (isFileCurrent(cacheFilePath))
            continue;

        const bool removeSuccess = QFile::remove(cacheFilePath);
        if (!removeSuccess) {
            qWarning() << "Failed to remove" << cacheFilePath;
        }
        cacheFileRemoved = true;
    }

    if (cacheFileRemoved)
        Q_EMIT cacheCleared();
}

QString CacheProvider::getPathForIdentifier(const QString &identifier)
{
    const QString slash = QStringLiteral("/");
    return this->m_cacheDir + (!identifier.startsWith(slash) ? slash : QStringLiteral("")) + identifier;
}
