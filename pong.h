#pragma once

#include <QDebug>
#include <QDBusMessage>
#include <QDBusConnection>

extern "C" {
#include <unistd.h>
}
class Auther : public QObject
{
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.PolicyKit1.AuthenticationAgent")

    Q_OBJECT

public slots:
    Q_SCRIPTABLE void BeginAuthentication(const QString &actionId,
                                        const QString &message,
                                        const QString &iconName,
                                        const QMap<QString, QString> &details,
                                        const QString &cookie,
                                        //const QList<QPair<QString, QList<QMap<QString, QVariant>>>> &identities
                                        //const QMap<QString, QVariantMap> &identities
                                        const QList<QPair<QString, QVariantMap>> &identities
                                        //const QVariantMap &identities
                                        )
    {
        Q_UNUSED(identities);
        qDebug() << actionId << message << iconName << details << cookie;

        QDBusMessage fuckoff = QDBusMessage::createMethodCall(
                "org.freedesktop.PolicyKit1",
                "/org/freedesktop/PolicyKit1/Authority",
                "org.freedesktop.PolicyKit1.Authority",
                "AuthenticationAgentResponse2"
                );
        fuckoff.setArguments( {
                getuid(),
                cookie,
                QVariant::fromValue(identities.first())
                });
        QDBusConnection::systemBus().send(fuckoff);

    }

};
