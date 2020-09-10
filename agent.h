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
        qDebug() << actionId << message << iconName << details << cookie;

        bool ok;
        QString password = QInputDialog::getText(nullptr, "Enter the root password", message + "\n" + actionId, QLineEdit::Password, QString(), &ok);
        if (!ok) {
            qWarning() << "user dismissed";
            return;
        }

        const QString safeCookie = QProcess::splitCommand(cookie).first(); // in case someone try to be funny
        QStringList args({
                responderPath,
                safeCookie,
                QString::number(getuid()),
            });

        KDESu::SuProcess process;
        process.setErase(true);
        process.setCommand(args.join(' ').toLocal8Bit());
        process.setUser("root");
        process.exec(password.toLocal8Bit());

        // clear the memory
        QRandomGenerator::system()->generate(password.begin(), password.end());
    }

public:
    QString responderPath;
};
