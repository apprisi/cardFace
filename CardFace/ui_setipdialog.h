/********************************************************************************
** Form generated from reading UI file 'setipdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETIPDIALOG_H
#define UI_SETIPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SetIpDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *set_ip;
    QLineEdit *set_port;
    QLineEdit *set_user;
    QLineEdit *set_password;
    QPushButton *set_yes;
    QPushButton *set_no;
    QPushButton *set_default;

    void setupUi(QDialog *SetIpDialog)
    {
        if (SetIpDialog->objectName().isEmpty())
            SetIpDialog->setObjectName(QStringLiteral("SetIpDialog"));
        SetIpDialog->resize(250, 325);
        SetIpDialog->setMinimumSize(QSize(250, 325));
        SetIpDialog->setMaximumSize(QSize(250, 325));
        label = new QLabel(SetIpDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 42, 70, 15));
        label_2 = new QLabel(SetIpDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 102, 70, 15));
        label_3 = new QLabel(SetIpDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 162, 70, 15));
        label_4 = new QLabel(SetIpDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 222, 70, 15));
        set_ip = new QLineEdit(SetIpDialog);
        set_ip->setObjectName(QStringLiteral("set_ip"));
        set_ip->setGeometry(QRect(100, 40, 120, 20));
        set_port = new QLineEdit(SetIpDialog);
        set_port->setObjectName(QStringLiteral("set_port"));
        set_port->setGeometry(QRect(100, 100, 120, 20));
        set_user = new QLineEdit(SetIpDialog);
        set_user->setObjectName(QStringLiteral("set_user"));
        set_user->setGeometry(QRect(100, 160, 120, 20));
        set_password = new QLineEdit(SetIpDialog);
        set_password->setObjectName(QStringLiteral("set_password"));
        set_password->setGeometry(QRect(100, 220, 120, 20));
        set_yes = new QPushButton(SetIpDialog);
        set_yes->setObjectName(QStringLiteral("set_yes"));
        set_yes->setGeometry(QRect(30, 270, 51, 20));
        set_no = new QPushButton(SetIpDialog);
        set_no->setObjectName(QStringLiteral("set_no"));
        set_no->setGeometry(QRect(169, 270, 51, 20));
        set_default = new QPushButton(SetIpDialog);
        set_default->setObjectName(QStringLiteral("set_default"));
        set_default->setGeometry(QRect(100, 270, 51, 20));

        retranslateUi(SetIpDialog);

        QMetaObject::connectSlotsByName(SetIpDialog);
    } // setupUi

    void retranslateUi(QDialog *SetIpDialog)
    {
        SetIpDialog->setWindowTitle(QApplication::translate("SetIpDialog", "Dialog", 0));
        label->setText(QApplication::translate("SetIpDialog", "\346\221\204\345\203\217\345\244\264ip\357\274\232", 0));
        label_2->setText(QApplication::translate("SetIpDialog", "\347\253\257\345\217\243\345\217\267\347\240\201\357\274\232", 0));
        label_3->setText(QApplication::translate("SetIpDialog", "\347\224\250\346\210\267\345\247\223\345\220\215\357\274\232", 0));
        label_4->setText(QApplication::translate("SetIpDialog", "\347\224\250\346\210\267\345\257\206\347\240\201\357\274\232", 0));
        set_ip->setText(QString());
        set_yes->setText(QApplication::translate("SetIpDialog", "\347\241\256\350\256\244", 0));
        set_no->setText(QApplication::translate("SetIpDialog", "\345\217\226\346\266\210", 0));
        set_default->setText(QApplication::translate("SetIpDialog", "\351\273\230\350\256\244", 0));
    } // retranslateUi

};

namespace Ui {
    class SetIpDialog: public Ui_SetIpDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETIPDIALOG_H
