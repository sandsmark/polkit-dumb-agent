/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "pong.h"

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

#if 0
    if (!QDBusConnection::systemBus().registerService("org.freedesktop.ScreenSaver")) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::systemBus().lastError().message()));
        exit(1);
    }

    if (!QDBusConnection::systemBus().registerService("org.gnome.SessionManager")) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::systemBus().lastError().message()));
        exit(1);
    }
    if (!QDBusConnection::systemBus().registerService("org.gnome.ScreenSaver")) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::systemBus().lastError().message()));
        exit(1);
    }

    if (!QDBusConnection::systemBus().registerService("org.freedesktop.PowerManagement")) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::systemBus().lastError().message()));
        exit(1);
    }
#endif
    //if (!QDBusConnection::sessionBus().registerService("com.iskrembilen.PolicyKit1.AuthenticationAgent")) {
    //    fprintf(stderr, "%s\n",
    //            qPrintable(QDBusConnection::systemBus().lastError().message()));
    //    return 1;
    //}

//    method call time=1599653439.595062 sender=:1.14901 -> destination=:1.14499 serial=8 path=/org/freedesktop/PolicyKit1/Authority; interface=org.freedesktop.PolicyKit1.Authority; member=RegisterAuthenticationAgent
//   struct {
//      string "unix-session"
//      array [
//         dict entry(
//            string "session-id"
//            variant                string "1"
//         )
//      ]
//   }
//   string "en_US.UTF-8"
//   string "/com/iskrembilen/polkitAuthAgent"
    Auther auther;
    auther.setObjectName("auther");
    if (!QDBusConnection::systemBus().registerObject("/com/iskrembilen/polkitAuthAgent", &auther, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register auther";
        return 1;
    }
    //Auther auther2;
    //if (!QDBusConnection::sessionBus().registerObject("/com/iskrembilen/polkitAuthAgent", &auther2, QDBusConnection::ExportAllSlots)) {
    //    qWarning() << "Failed to register auther";
    //    return 1;
    //}
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

#if 0
    Pong pong;
    pong.setObjectName("orgfreedesktopScreenSaver");
    if (!QDBusConnection::systemBus().registerObject("/org/freedesktop/ScreenSaver", &pong, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register freedesktop";
        return 1;
    }
    GnomeSessionPong gnomepong1;
    gnomepong1.setObjectName("orggnomeSessionManager");
    if (!QDBusConnection::systemBus().registerObject("/org/gnome/SessionManager", &gnomepong1, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register gnome session manager";
        return 1;
    }
    GnomeScreensaverPong gnomepong2;
    gnomepong2.setObjectName("orggnomeScreenSaver");
    if (!QDBusConnection::systemBus().registerObject("/org/gnome/ScreenSaver", &gnomepong2, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register gnome screensaver";
        return 1;
    }
    XdgPowerPong xdg;
    xdg.setObjectName("orgfreedesktopPowerManagementInhibit");
    if (!QDBusConnection::systemBus().registerObject("/org/freedesktop/PowerManagement/Inhibit", &xdg, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register xdg power manager";
        return 1;
    }
#endif


    app.exec();
    return 0;
}
