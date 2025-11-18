/********************************************************************************
** Form generated from reading UI file 'tedycja_przezroczystosci_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEDYCJA_PRZEZROCZYSTOSCI_DLG_H
#define UI_TEDYCJA_PRZEZROCZYSTOSCI_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_Tedycja_przezroczystosci_dlg
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Tedycja_przezroczystosci_dlg)
    {
        if (Tedycja_przezroczystosci_dlg->objectName().isEmpty())
            Tedycja_przezroczystosci_dlg->setObjectName(QString::fromUtf8("Tedycja_przezroczystosci_dlg"));
        Tedycja_przezroczystosci_dlg->resize(426, 151);
        Tedycja_przezroczystosci_dlg->setModal(false);
        buttonBox = new QDialogButtonBox(Tedycja_przezroczystosci_dlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(9, 63, 160, 25));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel);

        retranslateUi(Tedycja_przezroczystosci_dlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), Tedycja_przezroczystosci_dlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Tedycja_przezroczystosci_dlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(Tedycja_przezroczystosci_dlg);
    } // setupUi

    void retranslateUi(QDialog *Tedycja_przezroczystosci_dlg)
    {
        Tedycja_przezroczystosci_dlg->setWindowTitle(QApplication::translate("Tedycja_przezroczystosci_dlg", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Tedycja_przezroczystosci_dlg: public Ui_Tedycja_przezroczystosci_dlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEDYCJA_PRZEZROCZYSTOSCI_DLG_H
