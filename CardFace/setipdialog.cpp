#include "setipdialog.h"
#include "ui_setipdialog.h"
#include "mainwindow.h"
#include "nelvhkglobalmgr.h"
#include "hksdkvc.h"

#include <QMessageBox>

SetIpDialog::SetIpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetIpDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/safe"));
    m_setParent = (MainWindow*)parent;

    ui->set_ip->setText(m_setParent->m_carCameraIP);
    ui->set_port->setText(m_setParent->m_carCameraPort);
    ui->set_user->setText(m_setParent->m_carCameraUser);
    ui->set_password->setText(m_setParent->m_carCameraPass);

	CommonHelper::setStyle(":/images/whiteqss");
}

SetIpDialog::~SetIpDialog()
{
    delete ui;
}

void SetIpDialog::on_set_no_clicked()
{
    this->close();
}

void SetIpDialog::on_set_yes_clicked()
{
	m_setParent->m_carCameraIP = ui->set_ip->text();
	m_setParent->m_carCameraPort = ui->set_port->text();
	m_setParent->m_carCameraUser = ui->set_user->text();
	m_setParent->m_carCameraPass = ui->set_password->text();
	m_setParent->resetLicenseCamera();
	//ÔÝÊ±È¡Ïû
/*    m_setParent->setLicenseCamera();*/
    QMessageBox setBox;
    setBox.setWindowTitle(tr("setting"));
    setBox.setText(tr("set finish"));
    setBox.exec();
    this->close();
}

void SetIpDialog::on_set_default_clicked()
{
    m_setParent->resetLicenseCamera();
    QMessageBox setBox;
    setBox.setWindowTitle(tr("default"));
    setBox.setText(tr("default finish"));
    setBox.exec();
    this->close();
}
