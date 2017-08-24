#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/safe"));
    this->setWindowTitle(tr("登录登出"));
    m_loginparent = (MainWindow*)parent;
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_clicked()
{
    m_name =  ui->username->text();
    m_id =  ui->userid->text();

    QString temtitle = tr("人证牌合一车辆闸机系统");
    temtitle += "(";
    temtitle += m_name;
    temtitle += "_";
    temtitle += m_id;
    temtitle += ")";
    m_loginparent->setWindowTitle(temtitle);

//     if(!(temname.isEmpty() && temid.isEmpty()))
//     {
//         m_loginparent->m_loginflag = 1;
//     }
    m_loginparent->m_loginflag = 1;

/*	openSqlLog();*/
    //视频启动
    if(1 == m_loginparent->m_loginflag)
    {
//         m_loginparent->Stop1();
//         m_loginparent->Stop2();
//         m_loginparent->Stop3();
//         m_loginparent->Stop4();
// 
// 		m_loginparent->Start1();
// 		m_loginparent->Start2();
// 		m_loginparent->Start3();
// 		m_loginparent->Start4();
    }

    openSqlLog();

    saveSqlLog(1);

    this->close();
}

void LoginDialog::on_logout_clicked()
{
    openSqlLog();

    saveSqlLog(0);

    m_loginparent->m_loginflag = 0;
    if (0 == m_loginparent->m_loginflag)
    {
        m_loginparent->Stop1();
        m_loginparent->Stop2();
        m_loginparent->Stop3();
        m_loginparent->Stop4();
    }
	//m_newcar=0时开始获取身份信息，=1时开始获取车牌信息，现在另其=2，什么都不获取
	m_loginparent->m_newCar = 2;
    this->close();

	m_loginparent->close();
}

void LoginDialog::on_regist_clicked()
{

}

//! [11]
//建立一个函数来打开MySql数据库
void LoginDialog::openSqlLog()
{
    //创建一个database的对象并起名
    m_database=QSqlDatabase::addDatabase("QMYSQL");
    //设置主机名字（匹配主机名字）
    m_database.setHostName("localhost");
    //设置数据库名字（匹配数据库名字）
    m_database.setDatabaseName("cardface");
    //设置用户名字（匹配用户名字）
    m_database.setUserName("root");
    //设置用户密码（匹配用户密码）
    m_database.setPassword("123456");

    //条件判断是否可以顺利连接数据库
    if (!m_database.open())
    {
       QMessageBox msgBox;
       msgBox.setText("Open SQL Failed!");
       msgBox.exec();
    }
}
//! [11]


void LoginDialog::saveSqlLog(int index)
{
    //条件判断是否可以顺利连接数据库
    if (!m_database.open())
    {
        openSqlLog();
    }
    qDebug() << __FUNCTION__ << " " << __LINE__;
    QSqlQuery sql_query(m_database);

    sql_query.prepare("INSERT INTO LogInOut (Name, Number, Time, Log) VALUES (?, ?, ?, ?)");

    QDateTime dateTime;
    QString currentTime;
    dateTime = QDateTime::currentDateTime();
    currentTime = dateTime.toString("yyyy-MM-dd_hh:mm:ss");
    qDebug() << __FUNCTION__ << " " << __LINE__;

    if (!ui->username->text().isEmpty())
    {
        sql_query.addBindValue(ui->username->text());
    }
    else
    {
        sql_query.addBindValue(NULL);
    }

    if (!ui->userid->text().isEmpty())
    {
        sql_query.addBindValue(ui->userid->text());
    }
    else
    {
        sql_query.addBindValue(NULL);
    }

    if (!currentTime.isEmpty())
    {
        sql_query.addBindValue(currentTime);
    }
    else
    {
        sql_query.addBindValue(NULL);
    }

    if(index)
    {
		QString Log = tr("登入");
		sql_query.addBindValue(Log);
    }
    else
    {
		QString Log = tr("登出");
		sql_query.addBindValue(Log);
    }



    qDebug() << __FUNCTION__ << " " << __LINE__;
    if (!sql_query.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
            sql_query.lastError().text());
    }
    sql_query.finish();

	CommonHelper::setStyle(":/images/whiteqss");
}
