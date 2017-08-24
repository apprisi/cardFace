#include "infdialog.h"
#include "ui_infdialog.h"

//每页显示条数
static int inf_num = 20;

infdialog::infdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::infdialog)
{
	//创建窗口主体
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/safe"));
    setWindowTitle(tr("Information Of Passengers"));
    ui->inf_scrollArea->resize(this->size());

	//初始化变量
	m_items = 0;
	m_seekFlag = 0;
	m_textSeek = "";
	m_page = 1;

	//model初始化
	m_queryModel.setQuery(QString("SELECT CurrentTime, "
		"PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, Department1, StartDate1, EndDate1, ImagePer1, "
		"PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, Department2, StartDate2, EndDate2, ImagePer2, "
		"PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, Department3, StartDate3, EndDate3, ImagePer3, "
		"PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, Department4, StartDate4, EndDate4, ImagePer4, "
		"CarLicense, PassOrFail FROM information limit %1,%2").arg(m_items).arg(inf_num));

	m_queryModel.setHeaderData(0, Qt::Horizontal, tr("CurrentTime"));
	m_queryModel.setHeaderData(1, Qt::Horizontal, tr("PeopleName1"));
	m_queryModel.setHeaderData(2, Qt::Horizontal, tr("PeopleSex1"));
	m_queryModel.setHeaderData(3, Qt::Horizontal, tr("PeopleNation1"));
	m_queryModel.setHeaderData(4, Qt::Horizontal, tr("PeopleBirthday1"));
	m_queryModel.setHeaderData(5, Qt::Horizontal, tr("PeopleAddress1"));
	m_queryModel.setHeaderData(6, Qt::Horizontal, tr("PeopleIDCode1"));
	m_queryModel.setHeaderData(7, Qt::Horizontal, tr("Department1"));
	m_queryModel.setHeaderData(8, Qt::Horizontal, tr("StartDate1"));
	m_queryModel.setHeaderData(9, Qt::Horizontal, tr("EndDate1"));
	m_queryModel.setHeaderData(10, Qt::Horizontal, tr("ImagePer1"));

	m_queryModel.setHeaderData(11, Qt::Horizontal, tr("PeopleName2"));
	m_queryModel.setHeaderData(12, Qt::Horizontal, tr("PeopleSex2"));
	m_queryModel.setHeaderData(13, Qt::Horizontal, tr("PeopleNation2"));
	m_queryModel.setHeaderData(14, Qt::Horizontal, tr("PeopleBirthday2"));
	m_queryModel.setHeaderData(15, Qt::Horizontal, tr("PeopleAddress2"));
	m_queryModel.setHeaderData(16, Qt::Horizontal, tr("PeopleIDCode2"));
	m_queryModel.setHeaderData(17, Qt::Horizontal, tr("Department2"));
	m_queryModel.setHeaderData(18, Qt::Horizontal, tr("StartDate2"));
	m_queryModel.setHeaderData(19, Qt::Horizontal, tr("EndDate2"));
	m_queryModel.setHeaderData(20, Qt::Horizontal, tr("ImagePer2"));

	m_queryModel.setHeaderData(21, Qt::Horizontal, tr("PeopleName3"));
	m_queryModel.setHeaderData(22, Qt::Horizontal, tr("PeopleSex3"));
	m_queryModel.setHeaderData(23, Qt::Horizontal, tr("PeopleNation3"));
	m_queryModel.setHeaderData(24, Qt::Horizontal, tr("PeopleBirthday3"));
	m_queryModel.setHeaderData(25, Qt::Horizontal, tr("PeopleAddress3"));
	m_queryModel.setHeaderData(26, Qt::Horizontal, tr("PeopleIDCode3"));
	m_queryModel.setHeaderData(27, Qt::Horizontal, tr("Department3"));
	m_queryModel.setHeaderData(28, Qt::Horizontal, tr("StartDate3"));
	m_queryModel.setHeaderData(29, Qt::Horizontal, tr("EndDate3"));
	m_queryModel.setHeaderData(30, Qt::Horizontal, tr("ImagePer3"));

	m_queryModel.setHeaderData(31, Qt::Horizontal, tr("PeopleName4"));
	m_queryModel.setHeaderData(32, Qt::Horizontal, tr("PeopleSex4"));
	m_queryModel.setHeaderData(33, Qt::Horizontal, tr("PeopleNation4"));
	m_queryModel.setHeaderData(34, Qt::Horizontal, tr("PeopleBirthday4"));
	m_queryModel.setHeaderData(35, Qt::Horizontal, tr("PeopleAddress4"));
	m_queryModel.setHeaderData(36, Qt::Horizontal, tr("PeopleIDCode4"));
	m_queryModel.setHeaderData(37, Qt::Horizontal, tr("Department4"));
	m_queryModel.setHeaderData(38, Qt::Horizontal, tr("StartDate4"));
	m_queryModel.setHeaderData(39, Qt::Horizontal, tr("EndDate4"));
	m_queryModel.setHeaderData(40, Qt::Horizontal, tr("ImagePer4"));
	m_queryModel.setHeaderData(41, Qt::Horizontal, tr("CarLicense"));

	//view初始化
	ui->inf_tableView->setModel(&m_queryModel);
    ui->inf_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->inf_tableView->resizeColumnsToContents();
// 	ui->inf_tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

	//显示第几页信息
    m_pageStr = QString(tr("the"));
	m_pageStr += QString("%1").arg(m_page);
	m_pageStr += QString(tr("page"));
	ui->Inf_page->clear();
	ui->Inf_page->append(m_pageStr);
	ui->Inf_page->setAlignment(Qt::AlignCenter);
}

infdialog::~infdialog()
{
    delete ui;
}

// 添加信息，目前只能添加在最后
// void infdialog::on_Inf_add_clicked()
// {
//      m_tableModel.insertRow(ui->inf_tableView->selectionModel()->currentIndex().row());
// }
// 
// 删除鼠标选中的当前行
// void infdialog::on_Inf_delete_clicked()
// {
//     m_tableModel.removeRow(ui->inf_tableView->selectionModel()->currentIndex().row());
// }
// 
// 点击修改之后可以通过双击项目进行修改，如不想修改可以通过再次点击修改取消编辑
// void infdialog::on_Inf_modify_clicked()
// {
//     if(m_editable){
//         ui->inf_tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
//         m_editable = false;
//     }else{
//         ui->inf_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//         m_editable = true;
//     }
// }

//查询操作，目前只能查询一项
void infdialog::on_Inf_seek_clicked()
{
	m_seekFlag = 1;
	m_items = 0;

	//获取查询数据
    bool ok;
	m_textSeek = QInputDialog::getText(this, tr(" Seek "), tr("Search item: "), QLineEdit::Normal, QDir::home().dirName(), &ok);

	//根据查询关键字设置搜索范围
	m_queryModel.setQuery(QString("SELECT CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
		"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
		"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
		"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
		"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail "
		"FROM information  WHERE CONCAT(  CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
		"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
		"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
		"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
		"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail) LIKE '%%1%' limit %2,%3 ").arg(m_textSeek).arg(m_items).arg(inf_num));

	//重置界面大小用来匹配信息长度
	ui->inf_tableView->resizeColumnsToContents();

	//显示第1页
	m_page = 1;
	m_pageStr = QString(tr("the"));
	m_pageStr += QString("%1").arg(m_page);
	m_pageStr += QString(tr("page"));	
	ui->Inf_page->clear();
	ui->Inf_page->append(m_pageStr);
	ui->Inf_page->setAlignment(Qt::AlignCenter);	//居中显示
}

//刷新操作
void infdialog::on_Inf_flash_clicked()
{
	m_seekFlag = 0;
	m_items = 0;

	//刷新默认回到第一页初始状态
	m_queryModel.setQuery(QString("SELECT CurrentTime, "
		"PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, Department1, StartDate1, EndDate1, ImagePer1, "
		"PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, Department2, StartDate2, EndDate2, ImagePer2, "
		"PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, Department3, StartDate3, EndDate3, ImagePer3, "
		"PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, Department4, StartDate4, EndDate4, ImagePer4, "
		"CarLicense, PassOrFail FROM information limit %1,%2").arg(m_items).arg(inf_num));

	//重置大小
	ui->inf_tableView->resizeColumnsToContents();

	//显示第1页
	m_page = 1;
	m_pageStr = QString(tr("the"));
	m_pageStr += QString("%1").arg(m_page);
	m_pageStr += QString(tr("page"));
	ui->Inf_page->clear();
	ui->Inf_page->append(m_pageStr);
	ui->Inf_page->setAlignment(Qt::AlignCenter);
}

//上一页
void infdialog::on_Inf_previous_clicked()
{
	if (m_seekFlag)
	{
		if (m_items > inf_num - 1)
		{
			m_items = m_items - inf_num;

			m_queryModel.setQuery(QString("SELECT CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
				"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
				"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
				"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
				"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail "
				"FROM information  WHERE CONCAT(  CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
				"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
				"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
				"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
				"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail) LIKE '%%1%' limit %2,%3 ").arg(m_textSeek).arg(m_items).arg(inf_num));

			ui->inf_tableView->resizeColumnsToContents();

			m_page--;
			m_pageStr = QString(tr("the"));
			m_pageStr += QString("%1").arg(m_page);
			m_pageStr += QString(tr("page"));
			ui->Inf_page->clear();
			ui->Inf_page->append(m_pageStr);
			ui->Inf_page->setAlignment(Qt::AlignCenter);

		}
	} 
	else
	{
		if (m_items > inf_num - 1)
		{
			m_items = m_items - inf_num;

			m_queryModel.setQuery(QString("SELECT CurrentTime, "
				"PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, Department1, StartDate1, EndDate1, ImagePer1, "
				"PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, Department2, StartDate2, EndDate2, ImagePer2, "
				"PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, Department3, StartDate3, EndDate3, ImagePer3, "
				"PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, Department4, StartDate4, EndDate4, ImagePer4, "
				"CarLicense, PassOrFail FROM information limit %1,%2").arg(m_items).arg(inf_num));

			ui->inf_tableView->resizeColumnsToContents();

			m_page--;
			m_pageStr = QString(tr("the"));
			m_pageStr += QString("%1").arg(m_page);
			m_pageStr += QString(tr("page"));
			ui->Inf_page->clear();
			ui->Inf_page->append(m_pageStr);
			ui->Inf_page->setAlignment(Qt::AlignCenter);

		}
	}


}

//下一页
void infdialog::on_Inf_next_clicked()
{
	if (m_seekFlag)
	{
		m_items = m_items + inf_num;

		QSqlQuery temQuery(QString("SELECT CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
			"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
			"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
			"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
			"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail "
			"FROM information  WHERE CONCAT(  CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
			"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
			"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
			"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
			"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail) LIKE '%%1%' limit %2,%3 ").arg(m_textSeek).arg(m_items).arg(inf_num));

		if (temQuery.next())
		{
			m_queryModel.setQuery(QString("SELECT CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
				"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
				"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
				"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
				"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail "
				"FROM information  WHERE CONCAT(  CurrentTime, PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, "
				"Department1, StartDate1, EndDate1, ImagePer1, PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, "
				"Department2, StartDate2, EndDate2, ImagePer2, PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, "
				"Department3, StartDate3, EndDate3, ImagePer3, PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, "
				"Department4, StartDate4, EndDate4, ImagePer4, CarLicense, PassOrFail) LIKE '%%1%' limit %2,%3 ").arg(m_textSeek).arg(m_items).arg(inf_num));

			ui->inf_tableView->resizeColumnsToContents();

			m_page++;
			m_pageStr = QString(tr("the"));
			m_pageStr += QString("%1").arg(m_page);
			m_pageStr += QString(tr("page"));
			ui->Inf_page->clear();
			ui->Inf_page->append(m_pageStr);
			ui->Inf_page->setAlignment(Qt::AlignCenter);

		}
		else
		{
			m_items = m_items - inf_num;
		}

	} 
	else
	{
		m_items = m_items + inf_num;

		QSqlQuery temQuery(QString("SELECT CurrentTime FROM information limit %1,%2").arg(m_items).arg(inf_num));

		if (temQuery.next())
		{
			m_queryModel.setQuery(QString("SELECT CurrentTime, "
				"PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, Department1, StartDate1, EndDate1, ImagePer1, "
				"PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, Department2, StartDate2, EndDate2, ImagePer2, "
				"PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, Department3, StartDate3, EndDate3, ImagePer3, "
				"PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, Department4, StartDate4, EndDate4, ImagePer4, "
				"CarLicense, PassOrFail FROM information limit %1,%2").arg(m_items).arg(inf_num));

			ui->inf_tableView->resizeColumnsToContents();

			m_page++;
			m_pageStr = QString(tr("the"));
			m_pageStr += QString("%1").arg(m_page);
			m_pageStr += QString(tr("page"));
			ui->Inf_page->clear();
			ui->Inf_page->append(m_pageStr);
			ui->Inf_page->setAlignment(Qt::AlignCenter);

		}
		else
		{
			m_items = m_items - inf_num;
		}
	}
}

void infdialog::on_pushButton_clicked()
{
	//model初始化
	m_queryModel.setQuery(QString("SELECT CurrentImage1 FROM information "));

	ui->inf_tableView->resizeColumnsToContents();
}
