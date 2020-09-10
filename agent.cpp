#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>
#include <QTimer>
#include <QtDBus>

#include "agent.h"

int main(int argc, char **argv)
{
    QCoreApplication::setSetuidAllowed(true);
    QCoreApplication app(argc, argv);

    // Details
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    qDBusRegisterMetaType<QMap<QString, QString>>();

    // Identity
    qRegisterMetaType<QPair<QString, QVariantMap>>("QPair<QString, QVariantMap>>");
    qDBusRegisterMetaType<QPair<QString, QVariantMap>>();

    // Identity list
    qRegisterMetaType<QList<QPair<QString, QVariantMap>>>("QList<QPair<QString, QVariantMap>>>");
    qDBusRegisterMetaType<QList<QPair<QString, QVariantMap>>>();

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
    QString sessionId = qgetenv("XDG_SESSION_ID");
    bool isInt;
    sessionId.toInt(&isInt);

    if (!isInt) {
        qDebug() << "XDG_SESSION_ID " << sessionId << "is not a valid integer, defaulting to 1";
        sessionId = "1";
    }

    QDBusMessage regMessage = QDBusMessage::createMethodCall(
            "org.freedesktop.PolicyKit1",
            "/org/freedesktop/PolicyKit1/Authority",
            "org.freedesktop.PolicyKit1.Authority",
            "RegisterAuthenticationAgent"
        );
    regMessage.setArguments({
            QVariant::fromValue(QPair<QString, QVariantMap>("unix-session", {{"session-id", sessionId}})),
            "en_US.UTF-8",
            "/com/iskrembilen/polkitAuthAgent"
        });
    QDBusConnection::systemBus().send(regMessage);

    app.exec();
    return 0;
}
