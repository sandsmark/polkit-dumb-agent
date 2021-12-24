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
#include <QInputDialog>
#include <QRandomGenerator>
#include <KDESu/SuProcess>
#include <QProcess>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <sys/stat.h>
#include <unistd.h>

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
        const QByteArray command = QStringList({ responderPath, safeCookie, QString::number(getuid()) }).join(' ').toLocal8Bit();

        QString home = std::getenv("HOME");
        QString config = home.append("/.config/polkit-dumb-agent/");
        QString stylesheet = config.append("stylesheet.qss");

        struct stat buffer;
        QString stylesheetContent;
        if (stat(stylesheet.toLatin1().constData(), &buffer) == 0)
        {
            QFile stylesheetFile(stylesheet);

            if (stylesheetFile.open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream in(&stylesheetFile);
                stylesheetContent = in.readAll();
            }
            else
            {
                qDebug() << "Failed to open " << stylesheet;
            }
        }

        for (int num=0; num<3; num++) 
        {
            QInputDialog inputDialog(nullptr);
            inputDialog.setWindowTitle("Enter the root password");
            inputDialog.setLabelText(message + "\n" + actionId);
            inputDialog.setTextEchoMode(QLineEdit::EchoMode::Password);
            inputDialog.setTextValue("");

            if (!stylesheetContent.isEmpty())
            {
                inputDialog.setStyleSheet(stylesheetContent);
            }

            int dialogResult = inputDialog.exec();

            QString password;    
            if (dialogResult == QDialog::DialogCode::Accepted)
            {
                password = inputDialog.textValue();
            }
            else
            {
                return; //user aborted
            }

            KDESu::SuProcess process("root", command);
            process.setErase(true);
            const int result = process.exec(password.toLocal8Bit());
            if (result < 0) {
                qDebug() << "Failed to send reply" << strerror(errno);
            }

            // clear the memory
            QRandomGenerator::system()->generate(password.begin(), password.end());
            if (result != KDESu::SuProcess::SuIncorrectPassword) {
                return;
            }
        }
        return;
    }

public:
    QString responderPath;
};
