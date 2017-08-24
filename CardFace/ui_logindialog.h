/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

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

class Ui_LoginDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *login;
    QPushButton *logout;
    QLineEdit *username;
    QLineEdit *userid;
    QPushButton *regist;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(290, 350);
        LoginDialog->setMinimumSize(QSize(290, 350));
        LoginDialog->setMaximumSize(QSize(290, 350));
        LoginDialog->setStyleSheet(QLatin1String("#LoginDialog { \n"
"border-image: url(:/images/e6); \n"
"} "));
        label = new QLabel(LoginDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 10, 196, 81));
        label_2 = new QLabel(LoginDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 124, 70, 20));
        label_3 = new QLabel(LoginDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 205, 70, 20));
        login = new QPushButton(LoginDialog);
        login->setObjectName(QStringLiteral("login"));
        login->setGeometry(QRect(120, 290, 51, 31));
        QPalette palette;
        QBrush brush(QColor(170, 85, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        login->setPalette(palette);
        login->setStyleSheet(QStringLiteral(""));
        logout = new QPushButton(LoginDialog);
        logout->setObjectName(QStringLiteral("logout"));
        logout->setGeometry(QRect(199, 290, 51, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Button, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush);
        logout->setPalette(palette1);
        logout->setStyleSheet(QStringLiteral(""));
        username = new QLineEdit(LoginDialog);
        username->setObjectName(QStringLiteral("username"));
        username->setGeometry(QRect(110, 120, 140, 30));
        userid = new QLineEdit(LoginDialog);
        userid->setObjectName(QStringLiteral("userid"));
        userid->setGeometry(QRect(110, 200, 140, 30));
        regist = new QPushButton(LoginDialog);
        regist->setObjectName(QStringLiteral("regist"));
        regist->setGeometry(QRect(40, 290, 51, 31));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Button, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush);
        regist->setPalette(palette2);
        regist->setStyleSheet(QStringLiteral(""));

        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Dialog", 0));
        label->setText(QApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:18pt; color:#000000;\">\347\224\250\346\210\267\347\231\273\345\275\225\347\231\273\345\207\272</span></p></body></html>", 0));
        label_2->setText(QApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:11pt; color:#000000;\">\347\224\250\346\210\267\345\247\223\345\220\215\357\274\232</span></p></body></html>", 0));
        label_3->setText(QApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:11pt; color:#000000;\">\347\224\250\346\210\267\345\267\245\345\217\267\357\274\232</span></p></body></html>", 0));
        login->setText(QApplication::translate("LoginDialog", "\347\231\273\345\275\225", 0));
        logout->setText(QApplication::translate("LoginDialog", "\347\231\273\345\207\272", 0));
        regist->setText(QApplication::translate("LoginDialog", "\346\263\250\345\206\214", 0));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
