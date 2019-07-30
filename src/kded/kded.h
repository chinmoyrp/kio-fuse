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

#ifndef FUSENOTIFIER_H
#define FUSENOTIFIER_H

#include <KDEDModule>
#include <QDBusAbstractAdaptor>
#include <QFile>

class KBookmarkManager;

class KIOFuse : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KIOFuse")

    public:
        KIOFuse(QObject* parent, const QList<QVariant> &parameters);
        ~KIOFuse();

    public Q_SLOTS:
        Q_SCRIPTABLE void mountUrl(const QString &remoteUrl);
        Q_SCRIPTABLE QString localUrl(const QString &remoteUrl);
        Q_SCRIPTABLE void setAuthority(const QString &remoteURL, const QString &auth);
        Q_SCRIPTABLE void setMountResponse(const QString &remoteUrl, const QString &virtualPath);

    private:
        void createBookmark(const QString &label, const QUrl &url);

    Q_SIGNALS:
        void mountInfoReceived(const QString &remoteUrl, const QString &virtualPath);

    private:
        int m_idCount;
        QString m_mountDir;
        QFile m_controlFile;
        QMap<QString, QString> m_handledUrls;
        QMap<QString, QString> m_auth;
        KBookmarkManager *m_manager;
};

#endif

