/********************************************************************************
** Form generated from reading UI file 'alldata.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALLDATA_H
#define UI_ALLDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_alldata
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QTableWidget *information;

    void setupUi(QDialog *alldata)
    {
        if (alldata->objectName().isEmpty())
            alldata->setObjectName(QStringLiteral("alldata"));
        alldata->resize(270, 403);
        alldata->setMinimumSize(QSize(270, 0));
        alldata->setMaximumSize(QSize(270, 16777215));
        gridLayout = new QGridLayout(alldata);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new QWidget(alldata);
        widget->setObjectName(QStringLiteral("widget"));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        information = new QTableWidget(widget);
        information->setObjectName(QStringLiteral("information"));

        gridLayout_2->addWidget(information, 0, 0, 1, 1);


        gridLayout->addWidget(widget, 0, 0, 1, 1);


        retranslateUi(alldata);

        QMetaObject::connectSlotsByName(alldata);
    } // setupUi

    void retranslateUi(QDialog *alldata)
    {
        alldata->setWindowTitle(QApplication::translate("alldata", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class alldata: public Ui_alldata {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALLDATA_H
