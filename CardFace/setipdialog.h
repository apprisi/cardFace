#ifndef SETIPDIALOG_H
#define SETIPDIALOG_H

#include <QDialog>

class MainWindow;

namespace Ui {
class SetIpDialog;
}

class SetIpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetIpDialog(QWidget *parent = 0);
    ~SetIpDialog();

private slots:
    void on_set_no_clicked();

    void on_set_yes_clicked();

    void on_set_default_clicked();

private:
    Ui::SetIpDialog *ui;
    //连接主界面指针
    MainWindow *m_setParent;
};

#endif // SETIPDIALOG_H
