#ifndef INFDIALOG_H
#define INFDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QInputDialog>
#include <QDir>
class MainWindow;

namespace Ui {
class infdialog;
}

class infdialog : public QDialog
{
    Q_OBJECT

public:
    explicit infdialog(QWidget *parent = 0);
    ~infdialog();


private slots:
//     void on_Inf_add_clicked();
// 
//     void on_Inf_delete_clicked();
// 
//     void on_Inf_modify_clicked();
// 
    void on_Inf_seek_clicked();
    
    void on_Inf_flash_clicked();

    void on_Inf_previous_clicked();

    void on_Inf_next_clicked();

    void on_pushButton_clicked();

private:
    Ui::infdialog *ui;
	
	//数据model
	QSqlQueryModel m_queryModel;

	//显示、查询时初始条目位置
	int m_items;

	//设置一个flag用来判断是否显示查询之后的条目
	int m_seekFlag;

	//搜索关键字
	QString m_textSeek;

	//第几页字符串
	QString m_pageStr;

	//第几页
	int m_page;
};

#endif // INFDIALOG_H
