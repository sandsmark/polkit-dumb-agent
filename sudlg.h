/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 */

#ifndef __SuDlg_h_Included__
#define __SuDlg_h_Included__

#include <QByteArray>

#include <KPasswordDialog>
#include <kdesu/su.h>

using namespace KDESu;

class KDEsuDialog
    : public KPasswordDialog
{
    Q_OBJECT

public:
    KDEsuDialog(QByteArray user, QByteArray authUser, bool enableKeep, const QString& icon , bool withIgnoreButton);
    ~KDEsuDialog() override;

    enum ResultCodes { AsUser = 10 };

private slots:
    void slotUser1();
protected:
    bool checkPassword() override;

private:
    SuProcess proc;
};


#endif // __SuDlg_h_Included__
