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

#include <KPluginFactory>
#include <KPluginLoader>
#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QProcess>
#include <QDBusConnection>
#include <QStandardPaths>


K_PLUGIN_FACTORY_WITH_JSON(KIOFuseFactory,
                           "kiofuse.json",
                           registerPlugin<KIOFuse>();)

KIOFuse::KIOFuse(QObject *parent, const QList<QVariant> &parameters)
    : KDEDModule(parent)
{
    Q_UNUSED(parameters);

    m_mountDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QStringLiteral("/Network/kiofuse");
    QDir().mkpath(m_mountDir);
    QProcess proc;
    proc.start(QStringLiteral("kio-fuse"), {m_mountDir});
    proc.waitForFinished();
    m_controlFile.setFileName(m_mountDir + QStringLiteral("/_control"));
    m_controlFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered | QIODevice::Truncate);
}

KIOFuse::~KIOFuse()
{
    m_controlFile.close();
    QProcess fusermountProcess;
    fusermountProcess.start(QStringLiteral("fusermount"), {QStringLiteral("-u"), m_mountDir});
    fusermountProcess.waitForFinished();
    QFile::remove(m_mountDir);
}

QString KIOFuse::convertToLocalPath(const QString &remotePath)
{
    QUrl u = QUrl::fromUserInput(remotePath);
    u.setPassword({});
    return m_mountDir + QStringLiteral("/%1/%2").arg(u.scheme()).arg(u.authority()) + u.path();
}

void KIOFuse::mountUrl(const QString &remoteUrl)
{   
    for (auto k : m_handledUrls.keys()) {
        if (k.startsWith(remoteUrl) || remoteUrl.startsWith(k)) {
            return;
        }
    }

    if (!m_handledUrls.value(remoteUrl).isEmpty()) {
        return;
    }

    QByteArray cmd = QStringLiteral("MOUNT %1").arg(remoteUrl).toUtf8();
    m_controlFile.write(cmd);
    const QString localPath = convertToLocalPath(remoteUrl);
    if (QFile::exists(localPath)) {
        m_handledUrls.insert(remoteUrl, localPath);
    }
}

QString KIOFuse::localUrl(const QString &remoteUrl)
{
    return QUrl::fromLocalFile(convertToLocalPath(remoteUrl)).toString();
}

#include "kded.moc"
