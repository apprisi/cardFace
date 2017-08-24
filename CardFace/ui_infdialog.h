/********************************************************************************
** Form generated from reading UI file 'infdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INFDIALOG_H
#define UI_INFDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_infdialog
{
public:
    QScrollArea *inf_scrollArea;
    QWidget *scrollAreaWidgetContents;
    QTableView *inf_tableView;
    QWidget *widget;
    QPushButton *Inf_seek;
    QPushButton *Inf_flash;
    QPushButton *Inf_previous;
    QPushButton *Inf_next;
    QTextEdit *Inf_page;

    void setupUi(QDialog *infdialog)
    {
        if (infdialog->objectName().isEmpty())
            infdialog->setObjectName(QStringLiteral("infdialog"));
        infdialog->resize(1200, 710);
        infdialog->setMinimumSize(QSize(1200, 710));
        infdialog->setMaximumSize(QSize(1200, 710));
        infdialog->setStyleSheet(QStringLiteral(""));
        inf_scrollArea = new QScrollArea(infdialog);
        inf_scrollArea->setObjectName(QStringLiteral("inf_scrollArea"));
        inf_scrollArea->setGeometry(QRect(-1, -1, 1200, 710));
        inf_scrollArea->setMinimumSize(QSize(1200, 710));
        inf_scrollArea->setMaximumSize(QSize(1200, 710));
        inf_scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1198, 708));
        scrollAreaWidgetContents->setStyleSheet(QLatin1String("#scrollAreaWidgetContents { \n"
"border-image: url(:/images/e6); \n"
"} "));
        inf_tableView = new QTableView(scrollAreaWidgetContents);
        inf_tableView->setObjectName(QStringLiteral("inf_tableView"));
        inf_tableView->setGeometry(QRect(0, 0, 1200, 660));
        inf_tableView->setStyleSheet(QStringLiteral(""));
        widget = new QWidget(scrollAreaWidgetContents);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 655, 1201, 51));
        widget->setStyleSheet(QStringLiteral(""));
        Inf_seek = new QPushButton(widget);
        Inf_seek->setObjectName(QStringLiteral("Inf_seek"));
        Inf_seek->setGeometry(QRect(150, 10, 70, 31));
        QPalette palette;
        QBrush brush(QColor(170, 85, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        Inf_seek->setPalette(palette);
        Inf_seek->setStyleSheet(QStringLiteral(""));
        Inf_flash = new QPushButton(widget);
        Inf_flash->setObjectName(QStringLiteral("Inf_flash"));
        Inf_flash->setGeometry(QRect(350, 10, 70, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Button, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush);
        Inf_flash->setPalette(palette1);
        Inf_flash->setStyleSheet(QStringLiteral(""));
        Inf_previous = new QPushButton(widget);
        Inf_previous->setObjectName(QStringLiteral("Inf_previous"));
        Inf_previous->setGeometry(QRect(570, 10, 70, 31));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Button, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush);
        Inf_previous->setPalette(palette2);
        Inf_previous->setStyleSheet(QStringLiteral(""));
        Inf_next = new QPushButton(widget);
        Inf_next->setObjectName(QStringLiteral("Inf_next"));
        Inf_next->setGeometry(QRect(1000, 10, 70, 31));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Button, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush);
        Inf_next->setPalette(palette3);
        Inf_next->setStyleSheet(QStringLiteral(""));
        Inf_page = new QTextEdit(widget);
        Inf_page->setObjectName(QStringLiteral("Inf_page"));
        Inf_page->setGeometry(QRect(780, 10, 100, 30));
        Inf_page->setReadOnly(true);
        inf_scrollArea->setWidget(scrollAreaWidgetContents);
        widget->raise();
        inf_tableView->raise();

        retranslateUi(infdialog);

        QMetaObject::connectSlotsByName(infdialog);
    } // setupUi

    void retranslateUi(QDialog *infdialog)
    {
        infdialog->setWindowTitle(QApplication::translate("infdialog", "Dialog", 0));
        Inf_seek->setText(QApplication::translate("infdialog", "\346\237\245\350\257\242", 0));
        Inf_flash->setText(QApplication::translate("infdialog", "\345\210\267\346\226\260", 0));
        Inf_previous->setText(QApplication::translate("infdialog", "\344\270\212\344\270\200\351\241\265", 0));
        Inf_next->setText(QApplication::translate("infdialog", "\344\270\213\344\270\200\351\241\265", 0));
    } // retranslateUi

};

namespace Ui {
    class infdialog: public Ui_infdialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFDIALOG_H
