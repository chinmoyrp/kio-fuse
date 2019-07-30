/*
 * Copyright 2019 Chinmoy Ranjan Pradhan <chinmoyrp65@gmail.com>
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License or any later version accepted by the membership of
 * KDE e.V. (or its successor approved by the membership of KDE
 * e.V.), which shall act as a proxy defined in Section 14 of
 * version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kded.h"

#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QProcess>
#include <QDBusConnection>
#include <QStandardPaths>
#include <QDateTime>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KBookmarkManager>


K_PLUGIN_FACTORY_WITH_JSON(KIOFuseFactory,
                           "kiofuse.json",
                           registerPlugin<KIOFuse>();)

KIOFuse::KIOFuse(QObject *parent, const QList<QVariant> &parameters)
    : KDEDModule(parent), m_idCount(0)
{
    Q_UNUSED(parameters);

    m_mountDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/Network/kiofuse");
    QDir().mkpath(m_mountDir);
    QProcess proc;
    proc.start(QStringLiteral("fusermount"), {QStringLiteral("-zu"), m_mountDir});
    proc.waitForFinished(10000);
    proc.start(QStringLiteral("kio-fuse"), {m_mountDir});
    proc.waitForFinished(10000);
    m_controlFile.setFileName(m_mountDir + QStringLiteral("/_control"));
    m_controlFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered | QIODevice::Truncate);
    const QString file = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/user-places.xbel");
    m_manager = KBookmarkManager::managerForExternalFile(file);

}

KIOFuse::~KIOFuse()
{
    m_controlFile.close();
    QProcess fusermountProcess;
    fusermountProcess.start(QStringLiteral("fusermount"), {QStringLiteral("-u"), m_mountDir});
    fusermountProcess.waitForFinished();
    QFile::remove(m_mountDir);
}

void KIOFuse::mountUrl(const QString &remoteUrl)
{
    QUrl u = QUrl::fromUserInput(remoteUrl);
    QString cachedAuth = m_auth.value(u.host());
    if (cachedAuth.isEmpty()) {
        return;
    }
    u.setUserInfo(cachedAuth);
    QByteArray cmd = QStringLiteral("MOUNT %1").arg(u.toString()).toUtf8();
    m_controlFile.write(cmd);
    connect (this, &KIOFuse::mountInfoReceived, this, [=](const QString &ru, const QString &vp) {
        QUrl _ru = QUrl::fromUserInput(ru).adjusted(QUrl::NormalizePathSegments);
        const QString host = _ru.host();
        if (!m_handledUrls.value(host).isEmpty()) {
            return;
        }
        QString _vp = m_mountDir + vp;
        createBookmark(host, QUrl::fromLocalFile(_vp));
        _vp.remove(_ru.path());
        m_handledUrls.insert(host, _vp);
    });
}

void KIOFuse::setMountResponse(const QString &remoteUrl, const QString &virtualPath)
{
    if (virtualPath.isEmpty()) {
        return;
    }
    emit mountInfoReceived(remoteUrl, virtualPath);
}


QString KIOFuse::localUrl(const QString &remoteUrl)
{
    QUrl u = QUrl::fromUserInput(remoteUrl);
    const QString virtualPath = m_handledUrls.value(u.host());
    return virtualPath + u.path();
}

void KIOFuse::setAuthority(const QString &remoteURL, const QString &auth)
{
    m_auth.insert(remoteURL, auth);
}

void KIOFuse::createBookmark(const QString &label, const QUrl &url)
{
    KBookmarkGroup root = m_manager->root();
    if (root.isNull()) {
        return;
    }
    KBookmark bookmark = root.addBookmark(label, url, QStringLiteral("folder-network"));
    bookmark.setMetaDataItem(QStringLiteral("ID"),QString::number(QDateTime::currentSecsSinceEpoch()) + QLatin1Char('/') + QString::number(m_idCount++));
    m_manager->save();
}


#include "kded.moc"
