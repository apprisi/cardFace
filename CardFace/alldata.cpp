#include "alldata.h"
#include "ui_alldata.h"
#include "mainwindow.h"

#include <QImage>
alldata::alldata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::alldata)
{
    ui->setupUi(this);
    m_pdataparent = (MainWindow*)parent;
    this->setWindowTitle(tr("macinf"));

    ui->information->setColumnCount(2);
    ui->information->setRowCount(m_pdataparent->m_dataMacStrList.size());
    QStringList headers;
    headers << tr("mac") << tr("intensity");
    ui->information->setHorizontalHeaderLabels(headers);

    for (int i = 0; i<m_pdataparent->m_dataMacStrList.size(); i++)
    {
        m_pdataparent->m_dataMacStrListSub = m_pdataparent->m_dataMacStrList[i].split("|", QString::SkipEmptyParts);
        if (m_pdataparent->m_dataMacStrListSub.size() == 4)
        {
            ui->information->setItem(i, 0, new QTableWidgetItem(m_pdataparent->m_dataMacStrListSub[2]));
            ui->information->setItem(i, 1, new QTableWidgetItem(m_pdataparent->m_dataMacStrListSub[3] + "%"));
            ui->information->resizeColumnToContents (0);
        }
    }

	CommonHelper::setStyle(":/images/whiteqss");
}

alldata::~alldata()
{
    
    delete ui;
}
