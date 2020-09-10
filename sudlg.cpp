/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 */

#include "sudlg.h"

#include <QByteArray>

#include <QPushButton>
#include <qstyle.h>

KDEsuDialog::KDEsuDialog(QByteArray user, QByteArray authUser, bool enableKeep, const QString& icon, bool withIgnoreButton)
    : KPasswordDialog(nullptr, enableKeep ? ShowKeepPassword : NoFlags)
{
    if ( !icon.isEmpty() ) {
        setIcon(QIcon::fromTheme(icon));
    }

    if ( withIgnoreButton ) {
        buttonBox()->addButton(QDialogButtonBox::Ignore);
    }

    QString superUserCommand = proc.superUserCommand();

    proc.setUser(authUser);

    setWindowTitle(tr("Run as %1").arg(QString::fromLatin1(user)));

    QString prompt;
    if (proc.useUsersOwnPassword()) {
        prompt = tr("Please enter your password below." );
    } else {
        if (authUser == "root") {
            if(withIgnoreButton)
                prompt = QStringLiteral("<qt>") + tr("The action you requested needs <b>root privileges</b>. "
        	    "Please enter <b>root's</b> password below or click "
                "Ignore to continue with your current privileges.").arg(QStringLiteral("</qt>"));
	    else
                prompt = QStringLiteral("<qt>") + tr("The action you requested needs <b>root privileges</b>. "
                "Please enter <b>root's</b> password below.") + QStringLiteral("</qt>");
        } else {
            if(withIgnoreButton)
                prompt = QStringLiteral("<qt>") + tr("The action you requested needs additional privileges. "
	                "Please enter the password for <b>%1</b> below or click "
        	        "Ignore to continue with your current privileges.").arg(QString::fromLatin1(authUser) +
                    QStringLiteral("</qt>"));
	    else
                prompt = QStringLiteral("<qt>") + tr("The action you requested needs additional privileges. "
	                "Please enter the password for <b>%1</b> below.").arg(QString::fromLatin1(authUser) +
                    QStringLiteral("</qt>"));

        }
    }
    setPrompt(prompt);

    if( withIgnoreButton ) {
        connect(buttonBox()->button(QDialogButtonBox::Ignore), SIGNAL(clicked()), SLOT(slotUser1()));
    }
}


KDEsuDialog::~KDEsuDialog()
{
}

bool KDEsuDialog::checkPassword()
{
    int status = proc.checkInstall(password().toLocal8Bit());
    switch (status)
    {
    case -1:
        showErrorMessage(tr("Conversation with su failed."), UsernameError);
        return false;

    case 0:
        return true;

    case SuProcess::SuNotFound:
        showErrorMessage(tr("The program 'su' could not be found.<br />"
                             "Ensure your PATH is set correctly."), FatalError);
        return false;

    case SuProcess::SuNotAllowed:
        // This is actually never returned, as kdesu cannot tell the difference.
        showErrorMessage(QLatin1String("The impossible happened."), FatalError);
        return false;

    case SuProcess::SuIncorrectPassword:
        showErrorMessage(tr("Permission denied.<br />"
                              "Possibly incorrect password, please try again.<br />"
                              "On some systems, you need to be in a special "
                              "group (often: wheel) to use this program."), PasswordError);
        return false;

    default:
        showErrorMessage(tr("Internal error: illegal return from "
                             "SuProcess::checkInstall()"), FatalError);
        done(Rejected);
        return false;
    }
}

void KDEsuDialog::slotUser1()
{
    done(AsUser);
}

