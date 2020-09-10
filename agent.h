#pragma once

#include <QDebug>
#include <QDBusMessage>
#include <QDBusConnection>
#include <KPasswordDialog>
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
        QPointer<KPasswordDialog> dlg = new KPasswordDialog;
        dlg->setPrompt("Please enter the root password");
        if (!dlg->exec()) {
            qWarning() << "Dialog cancelled";
            dlg->deleteLater();
            return;
        }
        if (!dlg) {
            qWarning() << "Dialog deleted during getting password";
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

        QString password = dlg->password();
        dlg->deleteLater();
        process.exec(password.toLocal8Bit());

        // clear the memory
        QRandomGenerator::system()->generate(password.begin(), password.end());
    }

public:
    QString responderPath;
};
