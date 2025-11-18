/********************************************************************************
** Form generated from reading UI file 'transparency_slider.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSPARENCY_SLIDER_H
#define UI_TRANSPARENCY_SLIDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_transparency_dialog
{
public:
    QVBoxLayout *verticalLayout;
    QSlider *horizontalSlider;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *transparency_dialog)
    {
        if (transparency_dialog->objectName().isEmpty())
            transparency_dialog->setObjectName(QString::fromUtf8("transparency_dialog"));
        transparency_dialog->resize(417, 65);
        verticalLayout = new QVBoxLayout(transparency_dialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalSlider = new QSlider(transparency_dialog);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(horizontalSlider);

        buttonBox = new QDialogButtonBox(transparency_dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Close);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(transparency_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), transparency_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), transparency_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(transparency_dialog);
    } // setupUi

    void retranslateUi(QDialog *transparency_dialog)
    {
        transparency_dialog->setWindowTitle(QApplication::translate("transparency_dialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class transparency_dialog: public Ui_transparency_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSPARENCY_SLIDER_H
