#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "agent.h"

int main(int argc, char **argv)
{
    QCoreApplication::setSetuidAllowed(true);
    QCoreApplication app(argc, argv);

    qRegisterMetaType<QPair<QString,QVariantMap>>("QPair<QString,QVariantMap>");
    qDBusRegisterMetaType<QPair<QString,QVariantMap>>();
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    qDBusRegisterMetaType<QMap<QString, QString>>();

    qRegisterMetaType<QMap<QString, QVariantMap>>("QMap<QString, QVariantMap>");
    qDBusRegisterMetaType<QMap<QString, QVariantMap>>();

    qRegisterMetaType<QList<QPair<QString, QVariantMap>>>("QList<QPair<QString, QVariantMap>>>");
    qDBusRegisterMetaType<QList<QPair<QString, QVariantMap>>>();
    qRegisterMetaType<QPair<QString, QVariantMap>>("QPair<QString, QVariantMap>>");
    qDBusRegisterMetaType<QPair<QString, QVariantMap>>();

    qRegisterMetaType<QList<QPair<QString, QList<QMap<QString, QVariant>>>>>("QList<QPair<QString, QList<QMap<QString, QVariant>>>>");
    qDBusRegisterMetaType<QList<QPair<QString, QList<QMap<QString, QVariant>>>>>();

    if (!QDBusConnection::systemBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }


    Auther auther;
    if (!QDBusConnection::systemBus().registerObject("/com/iskrembilen/polkitAuthAgent", &auther, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register auther";
        return 1;
    }

    QDBusMessage regMessage = QDBusMessage::createMethodCall(
            "org.freedesktop.PolicyKit1",
                "/org/freedesktop/PolicyKit1/Authority",
                "org.freedesktop.PolicyKit1.Authority",
                "RegisterAuthenticationAgent"
            );
    QVariantMap polkitshit1;
    polkitshit1["session-id"] = "1";
    QPair<QString, QVariantMap> polkitshit2("unix-session", polkitshit1);
    QVariantList polkitshitargs;
    polkitshitargs.append(QVariant::fromValue(polkitshit2));
    polkitshitargs.append("en_US.UTF-8");
    polkitshitargs.append("/com/iskrembilen/polkitAuthAgent");
    regMessage.setArguments( polkitshitargs);
    QDBusConnection::systemBus().send(regMessage);

    app.exec();
    return 0;
}
