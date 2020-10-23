/*
    Copyright (C) 2020 Martin Sandsmark

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QDebug>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QInputDialog>
#include <QPointer>
#include <QProcess>
#include <QRandomGenerator>
#include <KDESu/SuProcess>

extern "C" {
#include <unistd.h>
}

class Agent : public QObject
{
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.PolicyKit1.AuthenticationAgent")

    Q_OBJECT

public slots:
    Q_SCRIPTABLE void BeginAuthentication(const QString &actionId,
                                        const QString &message,
                                        const QString &iconName,
                                        const QMap<QString, QString> &details,
                                        const QString &cookie,
                                        const QList<QPair<QString, QVariantMap>> &identities
                                        )
    {
        qDebug() << actionId << message << iconName << details << cookie << identities;


        const QString safeCookie = QProcess::splitCommand(cookie).first(); // in case someone try to be funny
        QStringList args({ responderPath, safeCookie, QString::number(getuid()) });
        for (int num=0;
                num<3 && !tryAuth(args.join(' ').toLocal8Bit(), message, actionId);
                num++
            ) {}
    }

public:
    QString responderPath;

private:
    bool tryAuth(const QByteArray &command, const QString &message, const QString &actionId)
    {
        bool ok;
        QString password = QInputDialog::getText(nullptr, "Enter the root password", message + "\n" + actionId, QLineEdit::Password, QString(), &ok);
        if (!ok) {
            qWarning() << "user dismissed";
            return true;
        }

        KDESu::SuProcess process;
        process.setErase(true);
        process.setCommand(command);
        process.setUser("root");
        const int result = process.exec(password.toLocal8Bit());
        switch(result) {
        case KDESu::SuProcess::SuNotFound:
            qWarning() << "su not found!";
            break;
        case KDESu::SuProcess::SuNotAllowed:
            qWarning() << "su not allowed!";
            break;
        case KDESu::SuProcess::SuIncorrectPassword:
            qWarning() << "Wrong password";
            break;
        default:
            qDebug() << "su finished" << command << result;
        }

        // clear the memory
        QRandomGenerator::system()->generate(password.begin(), password.end());
        return result != KDESu::SuProcess::SuIncorrectPassword;
    }
};
