#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QSqlQuery>

class MainWindow;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    void openSqlLog();

    void saveSqlLog(int index);

private slots:
    void on_login_clicked();

    void on_logout_clicked();

    void on_regist_clicked();

private:
    Ui::LoginDialog *ui;
    //连接主界面指针
    MainWindow *m_loginparent;

    QString m_name;
    QString m_id;

    //创建数据库对象
    QSqlDatabase m_database;
};

#endif // LOGINDIALOG_H
