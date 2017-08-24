#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "infdialog.h"
#include <stdlib.h>
#include "nelvhkglobalmgr.h"
#include "hksdkvc.h"
#include "nelvspeaker.h"

#include <QSettings>
#include <QThread>
#include <QColor>
#include <QTimer>
#include <QPalette>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) 
{
    //创建主窗体图标
    ui->setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);
	setWindowState(Qt::WindowFullScreen);
    setWindowIcon(QIcon(":/images/safe"));
    setWindowTitle("人证牌合一车辆闸机系统");

// 	//设置9个灯的初始状态
	ui->all_green->setPixmap(QPixmap(":/images/red"));
	ui->all_green->setScaledContents(true);
	ui->pass1_face->setPixmap(QPixmap(":/images/red"));
	ui->pass1_ID->setPixmap(QPixmap(":/images/red"));
	ui->pass1_face->setScaledContents(true);
	ui->pass1_ID->setScaledContents(true);
	ui->pass2_face->setPixmap(QPixmap(":/images/red"));
	ui->pass2_ID->setPixmap(QPixmap(":/images/red"));
	ui->pass2_face->setScaledContents(true);
	ui->pass2_ID->setScaledContents(true);
	ui->pass3_face->setPixmap(QPixmap(":/images/red"));
	ui->pass3_ID->setPixmap(QPixmap(":/images/red"));
	ui->pass3_face->setScaledContents(true);
	ui->pass3_ID->setScaledContents(true);
	ui->pass4_face->setPixmap(QPixmap(":/images/red"));
	ui->pass4_ID->setPixmap(QPixmap(":/images/red"));
	ui->pass4_face->setScaledContents(true);
	ui->pass4_ID->setScaledContents(true);
// 	ui->all_green->setStyleSheet("background-color: yellow; border-radius:45px;padding:2px 4px;");
// 
// 	ui->pass1_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass1_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");

//     //初始化勾选框并显示初始模式
    m_check[0] = GetIniValues("", "checkMode/check1", "1").toInt();
//     ui->checkBox_1->setChecked(m_check[0]);
    m_check[1] = GetIniValues("", "checkMode/check2", "1").toInt();
//     ui->checkBox_2->setChecked(m_check[1]);
    m_check[2] = GetIniValues("", "checkMode/check3", "1").toInt();
//     ui->checkBox_3->setChecked(m_check[2]);
    m_check[3] = GetIniValues("", "checkMode/check4", "1").toInt();
//     ui->checkBox_4->setChecked(m_check[3]);
// 	if (0 == m_check[0] && 0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
// 	{
// 		ui->show_mode_red->setText("检测模式：未选择");
// 	}

    //启动获取mac地址进程
    m_ser_process = NELV_CreateNetMesMonotor();
    if (m_ser_process) 
	{
        NELV_NetMesMonotorStart(m_ser_process, "serial");
    }
    Sleep(1000);
    m_netmonitor = NELV_CreateNetMesMonotor();
    if (m_netmonitor) 
	{
        NELV_NetMesMonotorStart(m_netmonitor, NULL);
    }

    //hk（车牌摄像头的启动相关）    
    connect(&m_carTimer, SIGNAL(timeout()), this, SLOT(slotResetLicenseCamera()));
    
    NELV_HKSDK_Init();
    g_ReadDeviceConfig();
    NELV_HKSDK_StartWork();
    //must after NELV_HKSDK_StartWork, add need slot void slotGetCarData(QString carID, QByteArray littleImage, QByteArray largeImage)
    NELV_HKSDK_RegNotifyObject(this);
    resetLicenseCamera();

    //hk

    //刷卡器启动相关
    int localPort = GetIniValues("", "ip/remoteport", "9000").toInt();
    m_faceserver = NELV_CreateFaceServer();
    if (m_faceserver)
    {
        int a = NELV_InitFaceServer(m_faceserver, "0.0.0.0", localPort);
		if (0 == a)
		{
			qDebug() << "++++=====____" << "mainwindow faceserver fail";
		}
        NELV_RegFaceServer(m_faceserver, this);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Open server Failed!"));
        msgBox.exec();
    }

    //对按钮通过和禁止设置状态栏提示
    ui->pass->setStatusTip(tr("pass"));
    ui->fail->setStatusTip(tr("fail"));

//     //创建窗体的时候就打开数据库（节省时间）
     openSql();

    //speaker语音初始化
    NELV_SpeakerInit();
    m_speaker = NELV_CreateSpeaker();
    //建立另一种语音提示信号槽
    connect(&m_notify, SIGNAL(sendClientCmd(int, QString)), this, SLOT(sendClientCmd(int, QString)));
   
	//建立心跳机制检测4个客户端连接状态
	connect(&m_clientTimer, SIGNAL(timeout()), this, SLOT(slotReHeart()));
	m_clientTimer.start(5000);


    for (int i = 0; i < 4;i++)
    {
        m_newMessage[i] = 0;
        m_perPass[i] = 2;
        m_checkpass[i] = 1;
        m_lightpass[i] = 1;
        m_Socket[i] = new QUdpSocket;
        m_failpassenger[i] = 0;
		m_savePicture[i] = 0;
		//初始化刷卡计数器和身份证id
		m_cardNotify[i] = 0;
		m_curID[i] = "";

		for (int j = 0; j < 3; j++)
		{
			m_curImage[i][j] = NULL;
			m_curPer[i][j] = "";
		}
		m_curTimes[i] = 0;
    }

    m_pass = 1;
    m_newCar = 1;
    m_receiveMessage = 0;
    m_loginflag = 0;

    m_passauto = 0;
    m_failauto = 0;
    m_autolight = 1;
	m_flag = 0;
	m_idPass = 0;
	m_unifiedTime = "";
    m_bgcolor=0;
    

    m_auto = GetIniValues("", "auto/autopass", "0").toInt();
    connect(&m_autoPass, SIGNAL(timeout()), this, SLOT(slotAuto()));

    m_loginflag = 1;

	//在主窗口显示之前先显示登陆界面  （暂时测试取消）
	if (GetIniValues("", "index/login", "0").toInt())
	{
		LoginDialog login(this);
		login.exec();
		login.deleteLater();
	}

	connect(&m_vedioTimer, SIGNAL(timeout()), this, SLOT(slotVedio()));
	m_vedioTimer.start(2500);

    CommonHelper::setStyle(":/images/whiteqss");

    ui->pushButton->hide();

}
//! [0]

//! [1]
MainWindow::~MainWindow()
{
    delete ui;
}
//! [1]

//! [2]
//重写槽函数，获取数据
void MainWindow::GetFaceData(void* facedata)
{
    if (0 == m_newCar)
    {
        Facedata *data = (Facedata*)facedata;

        if (1 == m_check[0])
        {
            if (1 == m_checkpass[0])
            {
                m_newMessage[0] = 1;
            }
        }
        if (1 == m_check[1])
        {
            if (1 == m_checkpass[1])
            {
                m_newMessage[1] = 1;
            }
        }
        if (1 == m_check[2])
        {
            if (1 == m_checkpass[2])
            {
                m_newMessage[2] = 1;
            }
        }
        if (1 == m_check[3])
        {
            if (1 == m_checkpass[3])
            {
                m_newMessage[3] = 1;
            }
        }

        if (1 == m_newMessage[0])
		{
            if (data->m_index == "0")
			{
                m_curShowData[0] = *data;

				//清零其他客户端图片信息
				if (m_listImageMaxSize[1] != 0)
					m_listImageMaxSize[1] = -1;
				if (m_listImageMaxSize[2] != 0)
					m_listImageMaxSize[2] = -1;
				if (m_listImageMaxSize[3] != 0)
					m_listImageMaxSize[3] = -1;
			
                m_listImageMaxSize[0] = m_curShowData[0].m_listImages.size();

				if (1 == m_flag)
				{
					if (m_listImageMaxSize[0] > 0)
					{					}
					else
					{
						showImage(m_curShowData[0]);
						m_newMessage[0] = 0;
					}
					m_flag = 0;
				}
				else
				{
					showImage(m_curShowData[0]);
					m_newMessage[0] = 0;
				}
                
                
            }
        }
        if (1 == m_newMessage[1]) 
		{
            if (data->m_index == "1")
			{
                m_curShowData[1] = *data;

				//清零其他客户端图片信息
				if (m_listImageMaxSize[0] != 0)
					m_listImageMaxSize[0] = -1;
				if (m_listImageMaxSize[2] != 0)
					m_listImageMaxSize[2] = -1;
				if (m_listImageMaxSize[3] != 0)
					m_listImageMaxSize[3] = -1;

                m_listImageMaxSize[1] = m_curShowData[1].m_listImages.size();

				if (1 == m_flag)
				{
					if (m_listImageMaxSize[1] > 0)
					{
					}
					else
					{
						showImage(m_curShowData[1]);
						m_newMessage[1] = 0;
					}
					m_flag = 0;
				}
				else
				{
					showImage(m_curShowData[1]);
					m_newMessage[1] = 0;
				}
            }
        }
        if (1 == m_newMessage[2])
		{
            if (data->m_index == "2")
			{
                m_curShowData[2] = *data;

				//清零其他客户端图片信息
				if (m_listImageMaxSize[1] != 0)
					m_listImageMaxSize[1] = -1;
				if (m_listImageMaxSize[0] != 0)
					m_listImageMaxSize[0] = -1;
				if (m_listImageMaxSize[3] != 0)
					m_listImageMaxSize[3] = -1;

                m_listImageMaxSize[2] = m_curShowData[2].m_listImages.size();
                
				if (1 == m_flag)
				{
					if (m_listImageMaxSize[2] > 0)
					{
					}
					else
					{
						showImage(m_curShowData[2]);
						m_newMessage[2] = 0;
					}
					m_flag = 0;
				}
				else
				{
					showImage(m_curShowData[2]);
					m_newMessage[2] = 0;
				}
            }
        }
        if (1 == m_newMessage[3])
		{
            if (data->m_index == "3") 
			{
                m_curShowData[3] = *data;

				//清零其他客户端图片信息
				if (m_listImageMaxSize[1] != 0)
					m_listImageMaxSize[1] = -1;
				if (m_listImageMaxSize[2] != 0)
					m_listImageMaxSize[2] = -1;
				if (m_listImageMaxSize[0] != 0)
					m_listImageMaxSize[0] = -1;

                m_listImageMaxSize[3] = m_curShowData[3].m_listImages.size();
                
				if (1 == m_flag)
				{
					if (m_listImageMaxSize[3] > 0)
					{
					}
					else
					{
						showImage(m_curShowData[3]);
						m_newMessage[3] = 0;
					}
					m_flag = 0;
				}
				else
				{
					showImage(m_curShowData[3]);
					m_newMessage[3] = 0;
				}
            }
        }
    }
}
//! [2]

//! [3]
//显示各摄像头和读卡器获取的信息
void MainWindow::showImage(Facedata & data)
{
    if (1 == m_loginflag)
    {
        //配置显示匹配度参数
        bool ok;    //判断是否从字符串转为double
        double perPass_double;  //将匹配度信息转化为double型用来比较是否高于预设值
        QString perPass_string; //将匹配度信息转化为字符串用来显示
        QFont perPass_font;        //设置匹配度信息字体大小美化显示
        perPass_font.setPointSize(14);
		QPalette perPass_palette;
		perPass_palette.setColor(QPalette::WindowText, Qt::black);

        double perPass_ini = GetIniValues("", "degree/matchingDegree", "0.6").toDouble(&ok);   //从配置文件中读取预设匹配值

        //配置显示哪张图片参数
        int imagesIndex;

        int index = data.m_index.toInt();
        if (0 == index)
        {
            //显示摄像头1文字信息（先清空原有信息）
            ui->pass1_textName->clear();
            ui->pass1_textAddress->clear();
            ui->pass1_textGender->clear();
            ui->pass1_textID->clear();
            ui->pass1_textCardPart->clear();
            ui->pass1_textCardTime->clear();
            ui->pass1_textNation->clear();

            ui->pass1_textName->append(m_curShowData[0].m_name);
            ui->pass1_textAddress->append(m_curShowData[0].m_address);
            ui->pass1_textGender->append(m_curShowData[0].m_gender);
            ui->pass1_textID->append(m_curShowData[0].m_identity);
            ui->pass1_textCardPart->append(m_curShowData[0].m_carddepart);
            ui->pass1_textCardTime->append(m_curShowData[0].m_carddatestart + "-" + m_curShowData[0].m_carddateend);
            ui->pass1_textNation->append(m_curShowData[0].m_nation);

            //显示摄像头1图像信息
            ui->vedio_1->clear();
            ui->vedio_1_ref->clear();
            ui->vedio_1->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_identImage)));
            ui->vedio_1->setScaledContents(true);
            if (m_listImageMaxSize[0] > 0)
            {
                imagesIndex = m_listImageMaxSize[0] / GetIniValues("", "index/imagesIndex", "1").toInt() - 1;
                ui->vedio_1_ref->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_listImages[imagesIndex])));

                ui->vedio_1_ref->setScaledContents(true);

                //显示摄像头1匹配度
                if (m_perPass[0] = (data.m_per.toDouble(&ok) > perPass_ini))
                {
					//发送信号使信号灯亮绿灯
					QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
					quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
					greenLight(lightip1, lightport1);

                    m_passauto++;
                    m_failauto++;
// 					ui->pass1_face->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
// 					ui->pass1_ID->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");

                    ui->pass1_face->setPixmap(QPixmap(":/images/green"));
                    ui->pass1_ID->setPixmap(QPixmap(":/images/green"));
                    ui->pass1_face->setScaledContents(true);
                    ui->pass1_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[0] = perPass_string;
                    perPass_string += "%";

                    ui->pass1_face_number->setText(perPass_string);
                    ui->pass1_face_number->setFont(perPass_font);
					ui->pass1_face_number->setPalette(perPass_palette);

                    ui->pass1_ID_text->setText(tr("normal"));
                    ui->pass1_ID_text->setFont(perPass_font);
					ui->pass1_ID_text->setPalette(perPass_palette);

					//开关，是否保存图片
					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[0]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}					



                    if (1 == m_check[0])
                    {
                        m_checkpass[0] = 0;
                        m_lightpass[0] = 1;
                    }
                    else
                    {
                        if (m_pass == 0) {}
                        else { m_pass = 1; }
                    }

                    m_notify.setIndexStatus(index, cardface_notify_success);

					//开关，是否存入分表数据库
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{						
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						saveSqlDetial(index);
					}

                }
                else
                {
					//发送信号使信号灯亮黄灯
					QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
					quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
					yellowLight(lightip1, lightport1);

                    //当信息录入重复不通过时最多检测cardNotify/times次
                    if (m_curID[0] != m_curShowData[0].m_identity)
                    {
                        m_curID[0] = m_curShowData[0].m_identity;
                        m_cardNotify[0] = 0;
						m_curTimes[index] = 0;
                    }
                    m_cardNotify[0]++;

					//每不通过一次，暂存一次图片和分数（只存最近3张）		
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						m_curTimes[index]++;
						if (3 == m_curTimes[index])
						{
							m_curTimes[index] = 0;
						}
					}					


                    if (m_cardNotify[0] == GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_manual);
                        m_failauto++;
                        m_failpassenger[0] = 1;

						if (GetIniValues("", "mysql/mysql", "0").toInt())
						{
							saveSqlDetial(index);
						}

                    }

                    if (m_cardNotify[0] < GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_repeat_card);
                    }
// 					ui->pass1_face->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
// 					ui->pass1_ID->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
                    ui->pass1_face->setPixmap(QPixmap(":/images/red"));
                    ui->pass1_ID->setPixmap(QPixmap(":/images/red"));
                    ui->pass1_face->setScaledContents(true);
                    ui->pass1_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[0] = perPass_string;

                    perPass_string += "%";
                    ui->pass1_face_number->setText(perPass_string);
                    ui->pass1_face_number->setFont(perPass_font);
					ui->pass1_face_number->setPalette(perPass_palette);
                    ui->pass1_ID_text->setText(tr("warning"));
                    ui->pass1_ID_text->setFont(perPass_font);
					ui->pass1_ID_text->setPalette(perPass_palette);

                    m_lightpass[0] = 0;

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[0]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}


                }
            }
            else
            {
                m_notify.setIndexStatus(index, cardface_notify_lookat_camera);
            }                     
        }

        if (1 == index)
        {
            //显示摄像头2文字信息（先清空原有信息）
            ui->pass2_textName->clear();
            ui->pass2_textAddress->clear();
            ui->pass2_textGender->clear();
            ui->pass2_textID->clear();
            ui->pass2_textCardPart->clear();
            ui->pass2_textCardTime->clear();
            ui->pass2_textNation->clear();

            ui->pass2_textName->append(m_curShowData[1].m_name);
            ui->pass2_textAddress->append(m_curShowData[1].m_address);
            ui->pass2_textGender->append(m_curShowData[1].m_gender);
            ui->pass2_textID->append(m_curShowData[1].m_identity);
            ui->pass2_textCardPart->append(m_curShowData[1].m_carddepart);
            ui->pass2_textCardTime->append(m_curShowData[1].m_carddatestart + "-" + m_curShowData[1].m_carddateend);
            ui->pass2_textNation->append(m_curShowData[1].m_nation);

            //显示摄像头2图像信息
            ui->vedio_2->clear();
            ui->vedio_2_ref->clear();
            ui->vedio_2->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_identImage)));
            ui->vedio_2->setScaledContents(true);
            if (m_listImageMaxSize[1] > 0)
            {
                imagesIndex = m_listImageMaxSize[1] / GetIniValues("", "index/imagesIndex", "1").toInt() - 1;
                ui->vedio_2_ref->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_listImages[imagesIndex])));

                ui->vedio_2_ref->setScaledContents(true);

                //显示摄像头2匹配度
                if (m_perPass[1] = (data.m_per.toDouble(&ok) > perPass_ini))
                {
                    m_passauto++;
                    m_failauto++;
// 					ui->pass2_face->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
// 					ui->pass2_ID->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
                    ui->pass2_face->setPixmap(QPixmap(":/images/green"));
                    ui->pass2_ID->setPixmap(QPixmap(":/images/green"));
                    ui->pass2_face->setScaledContents(true);
                    ui->pass2_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[1] = perPass_string;

                    perPass_string += "%";
                    
                    ui->pass2_face_number->setText(perPass_string);
                    ui->pass2_face_number->setFont(perPass_font);
					ui->pass2_face_number->setPalette(perPass_palette);
                    ui->pass2_ID_text->setText(tr("normal"));
                    ui->pass2_ID_text->setFont(perPass_font);
					ui->pass2_ID_text->setPalette(perPass_palette);
                    
					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[1]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //发送信号使信号灯亮绿灯
					QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
					quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
					greenLight(lightip2, lightport2);

                    if (1 == m_check[1])
                    {
                        m_checkpass[1] = 0;
                        m_lightpass[1] = 1;
                    }
                    else
                    {
                        if (m_pass == 0) {}
                        else { m_pass = 1; }
                    }

                    m_notify.setIndexStatus(index, cardface_notify_success);

					//开关，是否存入分表数据库
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						saveSqlDetial(index);
					}
                }
                else
                {


                    //当信息录入重复不通过时最多检测cardNotify/times次
                    if (m_curID[1] != m_curShowData[1].m_identity)
                    {
                        m_curID[1] = m_curShowData[1].m_identity;
                        m_cardNotify[1] = 0;
						m_curTimes[index] = 0;
                    }
                    m_cardNotify[1]++;

					//每不通过一次，暂存一次图片和分数	（只存最近3张）		
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						m_curTimes[index]++;
						if (3 == m_curTimes[index])
						{
							m_curTimes[index] = 0;
						}
					}

                    if (m_cardNotify[1] == GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_manual);
                        m_failauto++;
                        m_failpassenger[1] = 1;


						if (GetIniValues("", "mysql/mysql", "0").toInt())
						{
							saveSqlDetial(index);
						}

                    }

                    if (m_cardNotify[1] < GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_repeat_card);
                    }
// 					ui->pass2_face->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
// 					ui->pass2_ID->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
                    ui->pass2_face->setPixmap(QPixmap(":/images/red"));
                    ui->pass2_ID->setPixmap(QPixmap(":/images/red"));
                    ui->pass2_face->setScaledContents(true);
                    ui->pass2_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[1] = perPass_string;

                    perPass_string += "%";
                    ui->pass2_face_number->setText(perPass_string);
                    ui->pass2_face_number->setFont(perPass_font);
					ui->pass2_face_number->setPalette(perPass_palette);
                    ui->pass2_ID_text->setText(tr("warning"));
                    ui->pass2_ID_text->setFont(perPass_font);
					ui->pass2_ID_text->setPalette(perPass_palette);

                    m_lightpass[1] = 0;

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[1]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //test 语音播放
                    int tempvoice = GetIniValues("", "voice/voice", "1").toInt();
                    if (tempvoice)
                    {
                        QString str;
                        str += perPass_string;
                        if (m_speaker)
                        {
                            NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
                        }
                    }

                    //发送信号使信号灯亮黄灯
					QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
					quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
					yellowLight(lightip2, lightport2);
                }
            }
            else
            {
                m_notify.setIndexStatus(index, cardface_notify_lookat_camera);
            }           
        }

        if (2 == index)
        {
            //显示摄像头3文字信息（先清空原有信息）
            ui->pass3_textName->clear();
            ui->pass3_textAddress->clear();
            ui->pass3_textGender->clear();
            ui->pass3_textID->clear();
            ui->pass3_textCardPart->clear();
            ui->pass3_textCardTime->clear();
            ui->pass3_textNation->clear();

            ui->pass3_textName->append(m_curShowData[2].m_name);
            ui->pass3_textAddress->append(m_curShowData[2].m_address);
            ui->pass3_textGender->append(m_curShowData[2].m_gender);
            ui->pass3_textID->append(m_curShowData[2].m_identity);
            ui->pass3_textCardPart->append(m_curShowData[2].m_carddepart);
            ui->pass3_textCardTime->append(m_curShowData[2].m_carddatestart + "-" + m_curShowData[2].m_carddateend);
            ui->pass3_textNation->append(m_curShowData[2].m_nation);

            //显示摄像头3图像信息
            ui->vedio_3->clear();
            ui->vedio_3_ref->clear();
            ui->vedio_3->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_identImage)));
            ui->vedio_3->setScaledContents(true);

            if (m_listImageMaxSize[2] > 0 )
            {
                imagesIndex = m_listImageMaxSize[2] / GetIniValues("", "index/imagesIndex", "1").toInt() - 1;
                ui->vedio_3_ref->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_listImages[imagesIndex])));

                ui->vedio_3_ref->setScaledContents(true);

                //显示摄像头3匹配度
                if (m_perPass[2] = (data.m_per.toDouble(&ok) > perPass_ini))
                {
                    m_passauto++;
                    m_failauto++;
// 					ui->pass3_face->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
// 					ui->pass3_ID->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
                    ui->pass3_face->setPixmap(QPixmap(":/images/green"));
                    ui->pass3_ID->setPixmap(QPixmap(":/images/green"));
                    ui->pass3_face->setScaledContents(true);
                    ui->pass3_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[2] = perPass_string;
                    perPass_string += "%";
                    
                    ui->pass3_face_number->setText(perPass_string);
                    ui->pass3_face_number->setFont(perPass_font);
					ui->pass3_face_number->setPalette(perPass_palette);
                    ui->pass3_ID_text->setText(tr("normal"));
                    ui->pass3_ID_text->setFont(perPass_font);
					ui->pass3_ID_text->setPalette(perPass_palette);

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[2]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //发送信号使信号灯亮绿灯
					QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
					quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
					greenLight(lightip3, lightport3);

                    if (1 == m_check[2])
                    {
                        m_checkpass[2] = 0;
                        m_lightpass[2] = 1;
                    }
                    else
                    {
                        if (m_pass == 0) {}
                        else { m_pass = 1; }
                    }

                    m_notify.setIndexStatus(index, cardface_notify_success);

					//开关，是否存入分表数据库
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						saveSqlDetial(index);
					}
                }
                else
                {

                    //当信息录入重复不通过时最多检测cardNotify/times次
                    if (m_curID[2] != m_curShowData[2].m_identity)
                    {
                        m_curID[2] = m_curShowData[2].m_identity;
                        m_cardNotify[2] = 0;
						m_curTimes[index] = 0;
                    }
                    m_cardNotify[2]++;


					//每不通过一次，暂存一次图片和分数		（只存最近3张）	
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						m_curTimes[index]++;
						if (3 == m_curTimes[index])
						{
							m_curTimes[index] = 0;
						}
					}

                    if (m_cardNotify[2] == GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_manual);
                        m_failauto++;
                        m_failpassenger[2] = 1;


						if (GetIniValues("", "mysql/mysql", "0").toInt())
						{
							saveSqlDetial(index);
						}
                    }

                    if (m_cardNotify[2] < GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_repeat_card);
                    }

// 					ui->pass3_face->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
// 					ui->pass3_ID->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
                    ui->pass3_face->setPixmap(QPixmap(":/images/red"));
                    ui->pass3_ID->setPixmap(QPixmap(":/images/red"));
                    ui->pass3_face->setScaledContents(true);
                    ui->pass3_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[2] = perPass_string;

                    perPass_string += "%";
                    ui->pass3_face_number->setText(perPass_string);
                    ui->pass3_face_number->setFont(perPass_font);
					ui->pass3_face_number->setPalette(perPass_palette);
                    ui->pass3_ID_text->setText(tr("warning"));
                    ui->pass3_ID_text->setFont(perPass_font);
					ui->pass3_ID_text->setPalette(perPass_palette);

                    m_lightpass[2] = 0;

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[2]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //发送信号使信号灯亮黄灯
					QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
					quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
					yellowLight(lightip3, lightport3);
                }
            }
            else
            {
                m_notify.setIndexStatus(index, cardface_notify_lookat_camera);
            }
        }

        if (3 == index)
        {
            //显示摄像头4文字信息（先清空原有信息）
            ui->pass4_textName->clear();
            ui->pass4_textAddress->clear();
            ui->pass4_textGender->clear();
            ui->pass4_textID->clear();
            ui->pass4_textCardPart->clear();
            ui->pass4_textCardTime->clear();
            ui->pass4_textNation->clear();

            ui->pass4_textName->append(m_curShowData[3].m_name);
            ui->pass4_textAddress->append(m_curShowData[3].m_address);
            ui->pass4_textGender->append(m_curShowData[3].m_gender);
            ui->pass4_textID->append(m_curShowData[3].m_identity);
            ui->pass4_textCardPart->append(m_curShowData[3].m_carddepart);
            ui->pass4_textCardTime->append(m_curShowData[3].m_carddatestart + "-" + m_curShowData[3].m_carddateend);
            ui->pass4_textNation->append(m_curShowData[3].m_nation);

            //显示摄像头4图像信息
            ui->vedio_4->clear();
            ui->vedio_4_ref->clear();
            ui->vedio_4->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_identImage)));
            ui->vedio_4->setScaledContents(true);

            if (m_listImageMaxSize[3] > 0)
            {
                imagesIndex = m_listImageMaxSize[3] / GetIniValues("", "index/imagesIndex", "1").toInt() - 1;
                ui->vedio_4_ref->setPixmap(QPixmap::fromImage(QImage::fromData(data.m_listImages[imagesIndex])));
                ui->vedio_4_ref->setScaledContents(true);

                //显示摄像头4匹配度
                if (m_perPass[3] = (data.m_per.toDouble(&ok) > perPass_ini))
                {
                    m_passauto++;
                    m_failauto++;
// 					ui->pass4_face->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
// 					ui->pass4_ID->setStyleSheet("background-color: lime; border-radius:20px;padding:2px 4px;");
                    ui->pass4_face->setPixmap(QPixmap(":/images/green"));
                    ui->pass4_ID->setPixmap(QPixmap(":/images/green"));
                    ui->pass4_face->setScaledContents(true);
                    ui->pass4_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[3] = perPass_string;
                    perPass_string += "%";
                    
                    ui->pass4_face_number->setText(perPass_string);
                    ui->pass4_face_number->setFont(perPass_font);
					ui->pass4_face_number->setPalette(perPass_palette);
                    ui->pass4_ID_text->setText(tr("normal"));
                    ui->pass4_ID_text->setFont(perPass_font);
					ui->pass4_ID_text->setPalette(perPass_palette);

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[3]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //发送信号使信号灯亮绿灯
					QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
					quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
					greenLight(lightip4, lightport4);

                    if (1 == m_check[3])
                    {
                        m_checkpass[3] = 0;
                        m_lightpass[3] = 1;
                    }
                    else
                    {
                        if (m_pass == 0) {}
                        else { m_pass = 1; }
                    }

                    m_notify.setIndexStatus(index, cardface_notify_success);

					//开关，是否存入分表数据库
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						saveSqlDetial(index);
					}
                }
                else
                {
                    //当信息录入重复不通过时最多检测cardNotify/times次
                    if (m_curID[3] != m_curShowData[3].m_identity)
                    {
                        m_curID[3] = m_curShowData[3].m_identity;
                        m_cardNotify[3] = 0;

						m_curTimes[index] = 0;
                    }
                    m_cardNotify[3]++;

					//每不通过一次，暂存一次图片和分数（只存最近3张）			
					if (GetIniValues("", "mysql/mysql", "0").toInt())
					{
						m_curImage[index][m_curTimes[index]] = data.m_listImages[imagesIndex];
						m_curPer[index][m_curTimes[index]] = data.m_per;
						m_curTimes[index]++;
						if (3 == m_curTimes[index])
						{
							m_curTimes[index] = 0;
						}
					}

                    if (m_cardNotify[3] == GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_manual);
                        m_failauto++;
                        m_failpassenger[3] = 1;

						if (GetIniValues("", "mysql/mysql", "0").toInt())
						{
							saveSqlDetial(index);
						}
                    }

                    if (m_cardNotify[3] < GetIniValues("", "cardNotify/times", "3").toInt())
                    {
                        m_notify.setIndexStatus(index, cardface_notify_repeat_card);
                    }

// 					ui->pass4_face->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
// 					ui->pass4_ID->setStyleSheet("background-color: red; border-radius:20px;padding:2px 4px;");
                    ui->pass4_face->setPixmap(QPixmap(":/images/red"));
                    ui->pass4_ID->setPixmap(QPixmap(":/images/red"));
                    ui->pass4_face->setScaledContents(true);
                    ui->pass4_ID->setScaledContents(true);
                    perPass_double = data.m_per.toDouble(&ok) * 100;
                    perPass_string = QString::number((int)perPass_double);
                    m_passScore[3] = perPass_string;
                    perPass_string += "%";
                    ui->pass4_face_number->setText(perPass_string);
                    ui->pass4_face_number->setFont(perPass_font);
					ui->pass4_face_number->setPalette(perPass_palette);
                    ui->pass4_ID_text->setText(tr("warning"));
                    ui->pass4_ID_text->setFont(perPass_font);
					ui->pass4_ID_text->setPalette(perPass_palette);

                    m_lightpass[3] = 0;

					if (GetIniValues("", "index/saveimage", "0").toInt())
					{
						m_savePicture[3]++;
						TestSaveImg(data);
						TestSaveJpg(data);
					}

                    //发送信号使信号灯亮黄灯
					QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
					quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
					yellowLight(lightip4, lightport4);
                }
            }
            else
            {
                m_notify.setIndexStatus(index, cardface_notify_lookat_camera);
            }
        }

        checkID();

        if (m_listImageMaxSize[0] >0 || m_listImageMaxSize[1] >0 || m_listImageMaxSize[2] >0 || m_listImageMaxSize[3] >0)
        {
            //每录入一个摄像头内容就检测一次通过与否，一旦检测到不通过就亮红灯
            if (1 == m_lightpass[0] && 1 == m_lightpass[1] && 1 == m_lightpass[2] && 1 == m_lightpass[3])
            {
                m_pass = 1;
            }
            else
            {
                m_pass = 0;
            }
            if (m_pass == 1)
            {
				if (1 == m_idPass)
				{
// 					ui->all_green->setStyleSheet("background-color: red; border-radius:45px;padding:2px 4px;");

					ui->all_green->setPixmap(QPixmap(":/images/red"));
					ui->all_green->setScaledContents(true);
				} 
				else
				{
// 					ui->all_green->setStyleSheet("background-color: lime; border-radius:45px;padding:2px 4px;");

					ui->all_green->setPixmap(QPixmap(":/images/green"));
					ui->all_green->setScaledContents(true);
				}
            }
            else
            {
// 				ui->all_green->setStyleSheet("background-color: red; border-radius:45px;padding:2px 4px;");

                ui->all_green->setPixmap(QPixmap(":/images/red"));
                ui->all_green->setScaledContents(true);
            }
        }
		else
		{
// 			ui->all_green->setStyleSheet("background-color: red; border-radius:45px;padding:2px 4px;");

			ui->all_green->setPixmap(QPixmap(":/images/red"));
			ui->all_green->setScaledContents(true);
		}

        if (1 == m_auto)
        {
            if (4 == m_passauto)
            {
				if (1 == m_idPass)
				{
					QString str;
					str += tr(" some people have the same idCard ");
					if (m_speaker)
					{
						NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
					}
				} 
				else
				{
					QString str;
					str += tr(" pass all please fang xing ");
					if (m_speaker)
					{
						NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
					}
					m_autoPass.start(4000);
				}               
            }
            else if (4 == m_failauto)
            {
                QString str;
                int temp_passenger = m_failpassenger[0] + m_failpassenger[1] + m_failpassenger[2] + m_failpassenger[3];
                QString temp_passenger1 = QString::number(temp_passenger);

                str += tr("you ");
                str += temp_passenger1;
                str += tr(" ren ji guo duo ci yan zheng wei tong guo , qing ren gong que ren");
                if (m_speaker)
                {
                    NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
                }
            }
        }
    }
}
//! [3]

//! [4]
//检测4个客户端是否提取到相同身份证号（有点问题待改进）
int MainWindow::checkID()
{
    int changable1 = 1;
    int changable2 = 1;
    int changable3 = 1;
    int changable4 = 1;

    //if pass1 exist
    if(!ui->pass1_textID->toPlainText().isEmpty())
    {
        if(!ui->pass2_textID->toPlainText().isEmpty())
        {
            if(ui->pass1_textID->toPlainText() == ui->pass2_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass2_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass2_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));                
                changable2 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass2_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass2_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass2_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass1_textID->append(temID1);
                ui->pass2_textID->append(temID2);
            }
        }

        if (!ui->pass3_textID->toPlainText().isEmpty())
        {
            if (ui->pass1_textID->toPlainText() == ui->pass3_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass3_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass3_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass3_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass1_textID->append(temID1);
                ui->pass3_textID->append(temID2);
            }
        }

        if (!ui->pass4_textID->toPlainText().isEmpty())
        {
            if (ui->pass1_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass4_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass4_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));

                }
                
                ui->pass1_textID->append(temID1);
                ui->pass4_textID->append(temID2);


            }
        }
    }

    //if pass2 exist
    if (!ui->pass2_textID->toPlainText().isEmpty())
    {
        if (!ui->pass1_textID->toPlainText().isEmpty())
        {
            if (ui->pass1_textID->toPlainText() == ui->pass2_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass2_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass2_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));
                changable2 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass2_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass2_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass2_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass1_textID->append(temID1);
                ui->pass2_textID->append(temID2);
            }
        }

        if (!ui->pass3_textID->toPlainText().isEmpty())
        {
            if (ui->pass2_textID->toPlainText() == ui->pass3_textID->toPlainText())
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass3_textID->clear();

                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));
                changable2 = 0;
                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;

                ui->pass2_textID->append(temID1);
                ui->pass3_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass3_textID->clear();

                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass2_textID->append(temID1);
                ui->pass3_textID->append(temID2);
            }
        }

        if (!ui->pass4_textID->toPlainText().isEmpty())
        {
            if (ui->pass2_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass4_textID->clear();

                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));
                changable2 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass2_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass4_textID->clear();

                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass2_textID->append(temID1);
                ui->pass4_textID->append(temID2);
            }
        }
    }

    //if pass3 exist
    if (!ui->pass3_textID->toPlainText().isEmpty())
    {
        if (!ui->pass1_textID->toPlainText().isEmpty())
        {
            if (ui->pass1_textID->toPlainText() == ui->pass3_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass3_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass3_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass3_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                
                ui->pass1_textID->append(temID1);
                ui->pass3_textID->append(temID2);
            }
        }

        if (!ui->pass2_textID->toPlainText().isEmpty())
        {
            if (ui->pass2_textID->toPlainText() == ui->pass3_textID->toPlainText())
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass3_textID->clear();

                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));
                changable2 = 0;
                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;

                ui->pass2_textID->append(temID1);
                ui->pass3_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass3_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass3_textID->clear();

                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                
                ui->pass2_textID->append(temID1);
                ui->pass3_textID->append(temID2);
            }
        }

        if (!ui->pass4_textID->toPlainText().isEmpty())
        {
            if (ui->pass3_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass3_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass3_textID->clear();
                ui->pass4_textID->clear();

                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass3_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass3_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass3_textID->clear();
                ui->pass4_textID->clear();

                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                

                ui->pass3_textID->append(temID1);
                ui->pass4_textID->append(temID2);
            }
        }
    }

    //if pass4 exist
    if (!ui->pass4_textID->toPlainText().isEmpty())
    {
        if (!ui->pass1_textID->toPlainText().isEmpty())
        {
            if (ui->pass1_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass4_textID->clear();

                ui->pass1_textID->setTextBackgroundColor(QColor(Qt::red));
                changable1 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass1_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass1_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass1_textID->clear();
                ui->pass4_textID->clear();

                if (changable1)
                {
                    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));
                }                

                ui->pass1_textID->append(temID1);
                ui->pass4_textID->append(temID2);
            }
        }

        if (!ui->pass2_textID->toPlainText().isEmpty())
        {
            if (ui->pass2_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass4_textID->clear();

                ui->pass2_textID->setTextBackgroundColor(QColor(Qt::red));
                changable2 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass2_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass2_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass2_textID->clear();
                ui->pass4_textID->clear();

                if (changable2)
                {
                    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                
                ui->pass2_textID->append(temID1);
                ui->pass4_textID->append(temID2);
            }
        }

        if (!ui->pass3_textID->toPlainText().isEmpty())
        {
            if (ui->pass3_textID->toPlainText() == ui->pass4_textID->toPlainText())
            {
                QString temID1 = ui->pass3_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass3_textID->clear();
                ui->pass4_textID->clear();

                ui->pass3_textID->setTextBackgroundColor(QColor(Qt::red));
                changable3 = 0;
                ui->pass4_textID->setTextBackgroundColor(QColor(Qt::red));
                changable4 = 0;

                ui->pass3_textID->append(temID1);
                ui->pass4_textID->append(temID2);

				m_idPass = 1;
            }
            else
            {
                QString temID1 = ui->pass3_textID->toPlainText();
                QString temID2 = ui->pass4_textID->toPlainText();

                ui->pass3_textID->clear();
                ui->pass4_textID->clear();

                if (changable3)
                {
                    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                if (changable4)
                {
                    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));
                }
                
                ui->pass3_textID->append(temID1);
                ui->pass4_textID->append(temID2);
            }
        }
    }

    return 1;
}
//! [4]

//! [5]
//获取初始化文件路径
QString MainWindow::GetIniPath(QString iniName)
{
    QString appDir = QCoreApplication::applicationDirPath();
    if (iniName.length() == 0) {
        iniName = QCoreApplication::applicationName() + ".ini";
    }
    QString iniPath = appDir + "/" + iniName;
    return iniPath;
}
//! [5]

//! [6]
//获取初始化文件内部的值
QString MainWindow::GetIniValues(QString strIniName, QString key, QString defaultValue)
{
    QString iniPath = GetIniPath(strIniName);
    QSettings sets(iniPath, QSettings::IniFormat);
    QString value = sets.value(key, defaultValue).toString();
    return value;
}
//! [6]

//! [7]
//设置勾选框1（司机位置）是否勾选
int MainWindow::SetIniValues1(QString strIniName, int value)
{
    QString iniPath = GetIniPath(strIniName);
    QSettings sets(iniPath, QSettings::IniFormat);
    sets.setValue("checkMode/check1", value);

    return 0;
}
//! [7]

//! [8]
//设置勾选框2（副驾位置）是否勾选
int MainWindow::SetIniValues2(QString strIniName, int value)
{
    QString iniPath = GetIniPath(strIniName);
    QSettings sets(iniPath, QSettings::IniFormat);
    sets.setValue("checkMode/check2", value);

    return 0;
}
//! [8]

//! [9]
//设置勾选框3（左后位置）是否勾选
int MainWindow::SetIniValues3(QString strIniName, int value)
{
    QString iniPath = GetIniPath(strIniName);
    QSettings sets(iniPath, QSettings::IniFormat);
    sets.setValue("checkMode/check3", value);

    return 0;
}
//! [9]

//! [10]
//设置勾选框4（右后位置）是否勾选
int MainWindow::SetIniValues4(QString strIniName, int value)
{
    QString iniPath = GetIniPath(strIniName);
    QSettings sets(iniPath, QSettings::IniFormat);
    sets.setValue("checkMode/check4", value);

    return 0;
}
//! [10]

//! [11]
//建立一个函数来打开MySql数据库
void MainWindow::openSql()
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

void MainWindow::saveSqlBrief(int index)
{
	//条件判断是否可以顺利连接数据库
	if (!m_database.open())
	{
		openSql();
	}
	qDebug() << __FUNCTION__ << " " << __LINE__;
	QSqlQuery sql_query(m_database);

	sql_query.prepare("INSERT INTO inf_brief (CurrentTime, UnifiedTime, CarLicense, "
		"PeopleName1, PeopleIDCode1, PeopleName2, PeopleIDCode2, "
		"PeopleName3, PeopleIDCode3, PeopleName4, PeopleIDCode4, PassOrFail ) "
		"VALUES(?, ?, ?, "
		"?, ?, ?, ?, "
		"?, ?, ?, ?, ?)");

	QDateTime dateTime;
	QString currentTime;
	dateTime = QDateTime::currentDateTime();
	currentTime = dateTime.toString("yyyy-MM-dd_hh:mm:ss");
	qDebug() << __FUNCTION__ << " " << __LINE__;
	if (!currentTime.isEmpty())
	{
		sql_query.addBindValue(currentTime);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}
	if (!m_unifiedTime.isEmpty())
	{
		sql_query.addBindValue(m_unifiedTime);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}
	if (!m_carLicense.isEmpty())
	{
		sql_query.addBindValue(m_carLicense);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}

	for (int i = 0; i < 4;i++)
	{
		if (m_curShowData[i].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[i].m_name);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
		if (m_curShowData[i].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[i].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
	}
	if (1 == index)
	{
		sql_query.addBindValue(tr("mysqlpass"));
	} 
	else
	{
		sql_query.addBindValue(tr("mysqlfail"));
	}



	qDebug() << __FUNCTION__ << " " << __LINE__;
	if (!sql_query.exec()) {
		QMessageBox::critical(0, QObject::tr("Database Error"),
			sql_query.lastError().text());
	}
	//    else {
	//        QMessageBox msgBox;
	//        msgBox.setText("Save successfully!");
	//        msgBox.exec();
	//    }
	sql_query.finish();
} 

void MainWindow::saveSqlDetial(int index)
{
	//条件判断是否可以顺利连接数据库
	if (!m_database.open())
	{
		openSql();
	}
	qDebug() << __FUNCTION__ << " " << __LINE__;
	QSqlQuery sql_query(m_database);

	sql_query.prepare("INSERT INTO inf_detial (CurrentTime, UnifiedTime, Position, LittleImage, LargeImage, CarLicense,"
		"PeopleName, PeopleSex, PeopleNation, PeopleBirthday, PeopleAddress,"
		"PeopleIDCode, Department, StartDate, EndDate, LicenseImage,"
		"CurrentImage1, ImagePer1, CurrentImage2, ImagePer2, CurrentImage3, ImagePer3)"
		"VALUES(?, ?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, ?)");

	QDateTime dateTime;
	QString currentTime;
	dateTime = QDateTime::currentDateTime();
	currentTime = dateTime.toString("yyyy-MM-dd_hh:mm:ss");
	qDebug() << __FUNCTION__ << " " << __LINE__;
	if (!currentTime.isEmpty())
	{
		sql_query.addBindValue(currentTime);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}
	if (!m_unifiedTime.isEmpty())
	{
		sql_query.addBindValue(m_unifiedTime);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}

	sql_query.addBindValue(index);


	if (index > -1 && index < 4)
	{
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		if (!m_carLicense.isEmpty())
		{
			sql_query.addBindValue(m_carLicense);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_name);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_gender != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_gender);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_nation != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_nation);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_birthday != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_birthday);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_address != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_address);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_carddepart != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_carddepart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_carddatestart != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_carddatestart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[index].m_carddateend != NULL)
		{
			sql_query.addBindValue(m_curShowData[index].m_carddateend);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[index].m_identImage).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[index].m_identImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		for (int i = 0; i < 3; i++)
		{
			if (!(m_curImage[index][i]).isEmpty())
			{
				sql_query.addBindValue(m_curImage[index][i]);
			}
			else
			{
				sql_query.addBindValue(NULL);
			}

			if (m_curPer[index][i] != NULL)
			{
				sql_query.addBindValue(m_curPer[index][i]);
			}
			else
			{
				sql_query.addBindValue(NULL);
			}
		}
	}
	else
	{
		if (!m_littleImage.isEmpty())
		{
			sql_query.addBindValue(m_littleImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
		if (!m_largeImage.isEmpty())
		{
			sql_query.addBindValue(m_largeImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
		if (!m_carLicense.isEmpty())
		{
			sql_query.addBindValue(m_carLicense);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
		sql_query.addBindValue(NULL);
	}

	qDebug() << __FUNCTION__ << " " << __LINE__;
	if (!sql_query.exec()) {
		QMessageBox::critical(0, QObject::tr("Database Error"),
			sql_query.lastError().text());
	}
	//    else {
	//        QMessageBox msgBox;
	//        msgBox.setText("Save successfully!");
	//        msgBox.exec();
	//    }
	sql_query.finish();
}

//! [12]
//建立一个函数来写入MySql数据库
void MainWindow::saveSql(int index)
{
	//条件判断是否可以顺利连接数据库
	if (!m_database.open())
	{
		openSql();
	}
	qDebug() << __FUNCTION__ << " " << __LINE__;
	QSqlQuery sql_query(m_database);
    sql_query.prepare("INSERT INTO information (CurrentTime, "
        "PeopleName1, PeopleSex1, PeopleNation1, PeopleBirthday1, PeopleAddress1, PeopleIDCode1, Department1,"
        "StartDate1, EndDate1,  CurrentImage1, LicenseImage1, ImagePer1,"
        "PeopleName2, PeopleSex2, PeopleNation2, PeopleBirthday2, PeopleAddress2, PeopleIDCode2, Department2,"
        "StartDate2, EndDate2,  CurrentImage2, LicenseImage2, ImagePer2,"
        "PeopleName3, PeopleSex3, PeopleNation3, PeopleBirthday3, PeopleAddress3, PeopleIDCode3, Department3,"
        "StartDate3, EndDate3,  CurrentImage3, LicenseImage3, ImagePer3,"
        "PeopleName4, PeopleSex4, PeopleNation4, PeopleBirthday4, PeopleAddress4, PeopleIDCode4, Department4,"
        "StartDate4, EndDate4,  CurrentImage4, LicenseImage4, ImagePer4,"
        "CarLicense, LittleImage, LargeImage, PassOrFail) VALUES(?, "
        "?,?,?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?,?,?,?,"
        "?,?,?,?,?,"
        "?,?,?,?)");
    QDateTime dateTime;
    QString currentTime;
    dateTime = QDateTime::currentDateTime();
    currentTime = dateTime.toString("yyyy-MM-dd_hh:mm:ss");
	qDebug() << __FUNCTION__ << " " << __LINE__;
	if (!currentTime.isEmpty())
	{
		sql_query.addBindValue(currentTime);
	} 
	else
	{
		sql_query.addBindValue(NULL);
	}
	
    if (m_curShowData[0].m_index == "0") {

		if (m_curShowData[0].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_name);
		} 
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_gender != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_gender);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_nation != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_nation);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_birthday != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_birthday);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_address != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_address);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_carddepart != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_carddepart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_carddatestart != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_carddatestart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_carddateend != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_carddateend);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[0].m_listImages).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[0].m_listImages[0]);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[0].m_identImage).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[0].m_identImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[0].m_per != NULL)
		{
			sql_query.addBindValue(m_curShowData[0].m_per);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
    }
    else {
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
    }
    if (m_curShowData[1].m_index == "1") {
		if (m_curShowData[1].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_name);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_gender != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_gender);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_nation != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_nation);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_birthday != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_birthday);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_address != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_address);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_carddepart != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_carddepart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_carddatestart != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_carddatestart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_carddateend != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_carddateend);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[1].m_listImages).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[1].m_listImages[0]);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[1].m_identImage).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[1].m_identImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[1].m_per != NULL)
		{
			sql_query.addBindValue(m_curShowData[1].m_per);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
	}
    else {
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
    }
    if (m_curShowData[2].m_index == "2") {
		if (m_curShowData[2].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_name);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_gender != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_gender);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_nation != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_nation);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_birthday != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_birthday);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_address != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_address);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_carddepart != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_carddepart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_carddatestart != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_carddatestart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_carddateend != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_carddateend);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[2].m_listImages).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[2].m_listImages[0]);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[2].m_identImage).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[2].m_identImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[2].m_per != NULL)
		{
			sql_query.addBindValue(m_curShowData[2].m_per);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}
	}
    else {
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
    }
    if (m_curShowData[3].m_index == "3") {
		if (m_curShowData[3].m_name != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_name);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_gender != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_gender);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_nation != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_nation);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_birthday != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_birthday);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_address != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_address);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_identity != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_identity);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_carddepart != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_carddepart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_carddatestart != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_carddatestart);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_carddateend != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_carddateend);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[3].m_listImages).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[3].m_listImages[0]);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (!(m_curShowData[3].m_identImage).isEmpty())
		{
			sql_query.addBindValue(m_curShowData[3].m_identImage);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

		if (m_curShowData[3].m_per != NULL)
		{
			sql_query.addBindValue(m_curShowData[3].m_per);
		}
		else
		{
			sql_query.addBindValue(NULL);
		}

    }
    else {
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
        sql_query.addBindValue(NULL);
    }

	if (!m_carLicense.isEmpty())
	{
		sql_query.addBindValue(m_carLicense);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}

	if (!m_littleImage.isEmpty())
	{
		sql_query.addBindValue(m_littleImage);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}

	if (!m_largeImage.isEmpty())
	{
		sql_query.addBindValue(m_largeImage);
	}
	else
	{
		sql_query.addBindValue(NULL);
	}

	if (1 == index)
	{
		sql_query.addBindValue(tr("mysqlpass"));
	}
	else
	{
		sql_query.addBindValue(tr("mysqlfail"));
	}
	qDebug() << __FUNCTION__ << " " << __LINE__;
    if (!sql_query.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
            sql_query.lastError().text());
    }
//    else {
//        QMessageBox msgBox;
//        msgBox.setText("Save successfully!");
//        msgBox.exec();
//    }
    sql_query.finish();
}
//! [12]

//待改进
#define YUNXUTONGGUO_AUDIO "yunxutongguo.wav"
#define RENGONGQUEREN_AUDIO "rengongqueren.wav"

//! [13]
//允许通行
void MainWindow::on_pass_clicked()
{
    QString str;
    str += tr(" please pass ");
	qDebug() << __FUNCTION__ << " " << __LINE__;
    if (m_speaker)
    {
        NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
    }
	qDebug() << __FUNCTION__ << " " << __LINE__;
    pass();
}
//! [13]

//! [14]
//禁止通行，但是同样存入数据库，只不过不打开通行杆
void MainWindow::on_fail_clicked()
{
    QString str;
    str += tr(" jin zhi tong xing ");
	qDebug() << __FUNCTION__ << " " << __LINE__;
    if (m_speaker)
    {
        NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
    }
	qDebug() << __FUNCTION__ << " " << __LINE__;
    fail();
}
//! [14]

//! [15]
//更新获取网络mac地址
int MainWindow::updateNetMes()
{
    //获取mac地址
    if (m_netmonitor) {
        char buf[2048] = { 0 };
        if (NELV_NetMesMonotorGetData(m_netmonitor, buf, 2048)) {
            m_strNetMonitorMes = buf;
        }
    }
    m_dataMacStrList.clear();
    m_dataMacStrList = m_strNetMonitorMes.split("\n", QString::SkipEmptyParts);
    if (m_dataMacStrList.size() >= 0) {
        for (int i = 0; i < m_dataMacStrList.size(); i++) {
            m_dataMacStrListSub = m_dataMacStrList[i].split("|", QString::SkipEmptyParts);
            if (m_dataMacStrListSub.size() == 4) {
                m_dataText += m_dataMacStrListSub[2];
                m_dataText += tr(" Intensity: ");
                m_dataText += m_dataMacStrListSub[3];
                m_dataText += tr("% ");
            }
        }
    }
    m_maxMacNum = tr("  mac number: ");
    m_maxMacNum += QString::number((int)m_dataMacStrList.size());
//     ui->mac->setText(m_maxMacNum);
    return 1;
}
//! [15]

//! [16]
//获取音频文件
QString MainWindow::GetAudioFile(QString name)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString str = appDir + "/audio/" + name;
    return str;
}
//! [16]

//! [17]
//设置音频文件
void MainWindow::playaudio(QUrl url)
{
    m_player.setMedia(url);
    m_player.setVolume(100);
    m_player.play();
}
//! [17]

//! [18]
//获取摄像头车牌信息
void MainWindow::slotGetCarData(int type, QStringList carID, QByteArray littleImage, QByteArray largeImage)
{
	m_flag = 1;

    m_autoPass.stop();
    m_autolight = 1;
    updateNetMes();
    if(1 == m_newCar)
    {
		//检测到车牌亮红灯
		QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
		quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
		redLight(lightip1, lightport1);

		QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
		quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
		redLight(lightip2, lightport2);

		QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
		quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
		redLight(lightip3, lightport3);

		QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
		quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
		redLight(lightip4, lightport4);

        QString strText1 = carID[0];
        QString strText2 = carID[1];
        ui->car_license->setText("  "  + strText1 + "   " + strText2);
		m_carLicense = strText1 + strText2;
        QFont license_font;        //设置匹配度信息字体大小美化显示
        license_font.setPointSize(12);
		QPalette temPal;
		temPal.setColor(QPalette::WindowText, Qt::black);
        ui->car_license->setFont(license_font); 
		ui->car_license->setAlignment(Qt::AlignCenter);
		ui->car_license->setPalette(temPal);
        ui->car_license_picture->setPixmap(QPixmap::fromImage(QImage::fromData(littleImage)));
        ui->car_license_picture->setScaledContents(true);
        ui->car_picture->setPixmap(QPixmap::fromImage(QImage::fromData(largeImage)));
        ui->car_picture->setScaledContents(true);
		m_littleImage = littleImage;
		m_largeImage = largeImage;

        int carConfig;
        carConfig = GetIniValues("", "carconfig/carConfig", "1").toInt();
        if (carConfig == 1)
        {
            m_newCar = 0;
        }
        if (carConfig == 0)
        {
            m_newCar = 1;
        }
        //语音播报刷卡成功与否
        if (m_faceserver)
        {
            int einterval = GetIniValues("", "carid/einterval", "1500").toInt();
            int interval = GetIniValues("", "carid/interval", "10000").toInt();
            int repeatNum = GetIniValues("", "carid/repeatnum", "5").toInt();
            m_notify.setNextDelay(einterval);
            m_notify.setInterval(interval);
            m_notify.setRepeatNum(repeatNum);
            m_notify.setIndexStatus(-1, cardface_notify_start_card);
        }

		//存车牌信息
		QDateTime dt;
		dt = QDateTime::currentDateTime();
		m_unifiedTime = dt.toString("yyyy-MM-dd_hh:mm:ss");

		saveSqlDetial(5);
    }
}
//! [18]

//! [19]暂时使用其部分功能
//双击mac图标可以看到详细mac地址
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
//     if(event->button() == Qt::LeftButton)
//     {
//         if(ui->mac->mapFromGlobal(event->globalPos()).x() > 0 &&
//            ui->mac->mapFromGlobal(event->globalPos()).x() < 160 &&
//            ui->mac->mapFromGlobal(event->globalPos()).y() > 0 &&
//            ui->mac->mapFromGlobal(event->globalPos()).y() < 40)
//         {
//             updateNetMes();
//             alldata macData(this);
//             macData.exec();
//             macData.deleteLater();
//         }
//     }
}
//! [19]

//! [20]
//only test语音播报按钮(直播不通过的)
void MainWindow::on_voiceNotice_clicked()
{
    if (m_faceserver) 
    {
        NELV_FaceServerSendEndCardCmd(m_faceserver);
    }
    NELV_SpeakerStop(m_speaker);
    if (m_pass != 1)
    {
        QString str;
        if (m_perPass[0] == 0)
        {
            bool ok;
            str += tr(" driver verify fail and the score is ");
            str += QString::number((int)(m_curShowData[0].m_per.toDouble(&ok) * 100));
            str += "%";
        }

        if (m_perPass[1] == 0)
        {
            bool ok;
            str += tr(" fu_driver verify fail and the score is ");
            str += QString::number((int)(m_curShowData[1].m_per.toDouble(&ok) * 100));
            str += "%";
        }

        if (m_perPass[2] == 0)
        {
            bool ok;
            str += tr(" left verify fail and the score is ");
            str += QString::number((int)(m_curShowData[2].m_per.toDouble(&ok) * 100));
            str += "%";
        }

        if (m_perPass[3] == 0)
        {
            bool ok;
            str += tr(" right verify fail and the score is ");
            str += QString::number((int)(m_curShowData[3].m_per.toDouble(&ok) * 100));
            str += "%";
        }

        if (m_speaker)
        {
            NELV_SpeakerSpeak(m_speaker, str.toStdWString().c_str());
        }
    }
}
//! [20]

//! [21]
//only test开始刷卡按钮，因为点击后只能获取身份信息无法获取车牌信息
void MainWindow::on_startByHand_clicked()
{
	m_autolight = 1;
	m_flag = 1;
    m_autoPass.stop();
    ui->car_picture->clear();
    ui->car_license_picture->clear();
    ui->car_license->clear();
    if (m_faceserver)
    {
        m_notify.setIndexStatus(-1, cardface_notify_idle);
    }
    int einterval = GetIniValues("", "carid/einterval", "1500").toInt();
    int interval = GetIniValues("", "carid/interval", "10000").toInt();
    int repeatNum = GetIniValues("", "carid/repeatnum", "5").toInt();
    m_notify.setNextDelay(einterval);
    m_notify.setInterval(interval);
    m_notify.setRepeatNum(repeatNum);
    m_notify.setIndexStatus(-1, cardface_notify_start_card);

    //only test点击按钮之后可以正常获取身份信息但是不获取车牌信息
    m_newCar = 0;
}
//! [21]

//! [22]显示各客户端是否连接
void MainWindow::faceConnectClient(int index, int con)
{
    m_connect[index] = con;
    m_notify.setIndexConnect(index, m_connect[index]);

    if (0 == m_connect[index])
    {
        switch (index)
        {
            case 0: ui->pass1_information->setText(tr("driver fail")); break;
            case 1: ui->pass2_information->setText(tr("fu_driver fail")); break;
            case 2: ui->pass3_information->setText(tr("left fail")); break;
            case 3: ui->pass4_information->setText(tr("right fail")); break;
        }
    }

    if (1 == m_connect[index])
    {
        if(0 == index)
        {
            ui->pass1_information->setText(tr("driver success"));
        }
        if(1 == index)
        {
            ui->pass2_information->setText(tr("fu_driver success"));
        }
        if(2 == index)
        {
            ui->pass3_information->setText(tr("left success"));
        }
        if(3 == index)
        {
            ui->pass4_information->setText(tr("right success"));
        }
    }
}
//! [22]

//! [23]重置（恢复默认）车牌摄像头基本信息
void MainWindow::resetLicenseCamera()
{
    //hk
    int g_indexev = 0;
	m_carCameraIP = GetIniValues("", "carid/ip", "192.168.6.64");
    m_carCameraPort = GetIniValues("", "carid/port", "8000");
    m_carCameraUser = GetIniValues("", "carid/user", "admin");
    m_carCameraPass = GetIniValues("", "carid/pass", "admin12345");
    QString carSetting;
	m_conSuccess = NELV_HKSDK_Login(g_indexev, m_carCameraIP.toStdString().c_str(), m_carCameraPort.toInt(), m_carCameraUser.toStdString().c_str(), m_carCameraPass.toStdString().c_str());
    if (1 == m_conSuccess)
    {
        carSetting = tr("licenseSuccess,");
    } 
	else 
    {
        carSetting = tr("licensefail,");
    }
    char bufmes[2048] = { 0 };
    m_doSetSuccess = NELV_DoSetAlarm(g_indexev, bufmes);
    if (1 == m_doSetSuccess) 
    {
        carSetting += tr("doSetSuccess");
    } 
	else 
    {
        carSetting += tr("doSetfail");

    }

	QPalette temPal;
	temPal.setColor(QPalette::WindowText, Qt::black);

	ui->car_setting->setPalette(temPal);
    ui->car_setting->setText(carSetting);
	ui->car_setting->setAlignment(Qt::AlignCenter);

    if (1 != m_conSuccess || 1 != m_doSetSuccess)
    {
//         m_carTimer.start(5000);
    }
    else
    {
        m_carTimer.stop();
    }

    g_SaveDeviceConfig();
    //hk
}
//! [23]

//! [24]发送语音播报开始信息
void MainWindow::sendClientCmd(int index, QString text)
{
    if (m_faceserver) {
        qDebug() << text << "== = ";
        NELV_FaceServerSendWords(m_faceserver, text.toStdWString().c_str(), index, 1);
    }
}
//! [24]

//! [25]工具栏添加设置车牌摄像头信息
void MainWindow::on_action_ip_triggered()
{
    SetIpDialog setIp(this);
    setIp.exec();
    setIp.deleteLater();
}
//! [25]

//! [26]设置车牌摄像头基本信息
void MainWindow::setLicenseCamera()
{
    int g_indexev = 0;
    //hk
    QString carSetting;
    int m_conSuccess = NELV_HKSDK_Login(g_indexev, m_carCameraIP.toStdString().c_str(), m_carCameraPort.toInt(), m_carCameraUser.toStdString().c_str(), m_carCameraPass.toStdString().c_str());
    if (1 == m_conSuccess) 
	{
        carSetting = tr("licenseSuccess,");
    }
	else
	{
        carSetting = tr("licensefail,");
    }
    char bufmes[2048] = { 0 };
    int m_doSetSuccess = NELV_DoSetAlarm(g_indexev, bufmes);
    if (1 == m_doSetSuccess) 
	{
        carSetting += tr("m_doSetSuccess");
    } 
	else 
	{
        carSetting += tr("doSetfail");
    }

	QPalette temPal;
	temPal.setColor(QPalette::WindowText, Qt::black);
	ui->car_setting->setPalette(temPal);
    ui->car_setting->setText(carSetting);
	ui->car_setting->setAlignment(Qt::AlignCenter);

	if (1 != m_conSuccess || 1 != m_doSetSuccess)
	{
		m_carTimer.start(5000);
	}
	else
	{
		m_carTimer.stop();
	}

    g_SaveDeviceConfig();
    //hk
}
//! [26]

//! [27]显示数据库信息
void MainWindow::on_action_information_triggered()
{
    infdialog allInformation(this);
    allInformation.exec();
    allInformation.deleteLater();
}
//! [27]

//! [28]根据勾选框改变状态控制显示不同信息和显示检测模式
void MainWindow::on_checkBox_1_stateChanged(int arg1)
{
    if (arg1)
    {
        m_check[0] = 1;
//         if (0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：司机模式");
//         }
    }
    else
    {
        m_check[0] = 0;
//         if (0 == m_check[0] && 0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：未选择");
//         }
    }

    SetIniValues1("", m_check[0]);
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if (arg1)
    {
        m_check[1] = 1;
//         ui->show_mode_red->setText("检测模式：乘客模式");
    }
    else
    {
        m_check[1] = 0;
//         if (0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：司机模式");
//         }
// 
//         if (0 == m_check[0] && 0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：未选择");
//         }
    }

    SetIniValues2("", m_check[1]);
}

void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    if (arg1)
    {
        m_check[2] = 1;
//         ui->show_mode_red->setText("检测模式：乘客模式");
    }
    else
    {
        m_check[2] = 0;
//         if (0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：司机模式");
//         }
// 
//         if (0 == m_check[0] && 0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：未选择");
//         }
    }

    SetIniValues3("", m_check[2]);
}

void MainWindow::on_checkBox_4_stateChanged(int arg1)
{
    if (arg1)
    {
        m_check[3] = 1;
//         ui->show_mode_red->setText("检测模式：乘客模式");
    }
    else
    {
        m_check[3] = 0;
//         if (0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：司机模式");
//         }
// 
//         if (0 == m_check[0] && 0 == m_check[1] && 0 == m_check[2] && 0 == m_check[3])
//         {
//             ui->show_mode_red->setText("检测模式：未选择");
//         }
    }

    SetIniValues4("", m_check[3]);
}
//! [28]

//! [29]1号位置亮绿灯
int MainWindow::greenLight(QString lightip, quint16 lightport)
{
    //发送信号使信号灯亮（绿灯)
    char socketbuf[10] = { 0xCC, 0xDD, 0xA1, 0x01, 0x00, 0x04, 0x00, 0x07, 0xAD, 0x5A };
    QHostAddress lightAddress;
/*    quint16 aaa = 50000;*/
	lightAddress.setAddress(lightip/*"192.168.6.18"*/);
//     char dataReceive[9];
//     char checkReceive[9] = { 0xCC, 0xDD, 0xB0, 0x01, 0x00, 0x00, 0x0D, 0xBE, 0x7C };

	m_Socket[0]->writeDatagram(socketbuf, 10, lightAddress, /*aaa*/lightport);
    m_Socket[0]->flush();

//     do 
//     {
//         m_Socket[0]->writeDatagram(socketbuf, 10, lightAddress, aaa);
// 
//         m_Socket[0]->readDatagram(dataReceive, 9, &lightAddress, &aaa); 
//     } 
//     while (!(0x4f == (byte)dataReceive[0] && 0x4b == (byte)dataReceive[1] && 0x21 == (byte)dataReceive[2]));
//     
//     do 
//     {
//         m_Socket[0]->writeDatagram(checkReceive, 9, lightAddress, aaa);
// 
//         m_Socket[0]->readDatagram(dataReceive, 9, &lightAddress, &aaa);
// 
//     } 
//     while (!(0xaa == (byte)dataReceive[0] && 0xbb == (byte)dataReceive[1]));
// 
//     if (!(0x00 == dataReceive[4] && 0x04 == dataReceive[5]))
//     {
//         redLight();
//     }

    return 1;
}
//! [29]

//! [30]1号位置亮黄灯
int MainWindow::yellowLight(QString lightip, quint16 lightport)
{
    //发送信号使信号灯亮（黄灯）
    char socketbuf[10] = { 0xCC, 0xDD, 0xA1, 0x01, 0x00, 0x02, 0x00, 0x07, 0xAB, 0x56 };
    QHostAddress lightAddress;
	lightAddress.setAddress(lightip);
	m_Socket[0]->writeDatagram(socketbuf, 10, lightAddress, lightport);
    m_Socket[0]->flush();

    return 1;
}
//! [30]

//! [31]1号位置亮红灯
int MainWindow::redLight(QString lightip, quint16 lightport)
{
    //发送信号使信号灯亮（红灯）
    char socketbuf[10] = { 0xCC, 0xDD, 0xA1, 0x01, 0x00, 0x01, 0x00, 0x07, 0xAA, 0x54 };
    QHostAddress lightAddress;
	lightAddress.setAddress(lightip);
	m_Socket[0]->writeDatagram(socketbuf, 10, lightAddress, lightport);
    m_Socket[0]->flush();

    return 1;
}
//! [31]

//! [32]1号位置关灯
int MainWindow::closeLight(QString lightip, quint16 lightport)
{
    //发送信号使信号灯（关灯）
    char socketbuf[10] = { 0xCC, 0xDD, 0xA1, 0x01, 0x00, 0x00, 0x00, 0x07, 0xA9, 0x52 };
    QHostAddress lightAddress;
	lightAddress.setAddress(lightip);
	m_Socket[0]->writeDatagram(socketbuf, 10, lightAddress, lightport);
    m_Socket[0]->flush();

    return 1;
}
//! [32]

//! [33]打开登录界面
void MainWindow::on_action_login_triggered()
{
    LoginDialog login(this);
    login.exec();
    login.deleteLater();
}
//! [33]

//! [34]添加视频状态，开始和清除进程
int MainWindow::Stop1()
{
    if (m_netProcess[0].state() == QProcess::Running)
    {
        m_netProcess[0].kill();
        m_netProcess[0].waitForFinished();
    }
    return 1;
}

int MainWindow::Stop2()
{
    if (m_netProcess[1].state() == QProcess::Running)
    {
        m_netProcess[1].kill();
        m_netProcess[1].waitForFinished();
    }
    return 1;
}

int MainWindow::Stop3()
{
    if (m_netProcess[2].state() == QProcess::Running)
    {
        m_netProcess[2].kill();
        m_netProcess[2].waitForFinished();
    }

    return 1;
}

int MainWindow::Stop4()
{
    if (m_netProcess[3].state() == QProcess::Running)
    {
        m_netProcess[3].kill();
        m_netProcess[3].waitForFinished();
    }
    return 1;
}

int MainWindow::Start1()
{
    QString extraIndex;
    extraIndex = GetIniValues("", "extra/extraIndex", "-analyzeduration 100000");
    QString strudp1 = "udp://192.168.6.170:7000";
	int hk = GetIniValues("", "extra/hk1", "0").toInt();
	if (1 == hk)
	{
		strudp1 = GetIniValues("", "extra/rtsp1", "rtsp://admin:admin12345@192.168.6.200:554/h264/ch1/main/av_stream -rtsp_transport tcp -analyzeduration 100000 ");
		QString player1 = GetIniValues("", "extra/player", "ffplay2.exe");
		QString strCmdExe1 = QString("%1 %2 -wid %3").arg(QString(player1)).arg(QString(strudp1)).arg(ui->vedio_1_vedio->winId());
		m_netProcess[0].start(strCmdExe1);
	}
	else
	{
		QString strCmdExe1 = QString("ffplay2.exe %1 -wid %2 -an  %3 ").arg(QString(strudp1)).arg(ui->vedio_1_vedio->winId()).arg(extraIndex);
		m_netProcess[0].start(strCmdExe1);
	}
    

    return 1;
}

int MainWindow::Start2()
{
    QString extraIndex;
    extraIndex = GetIniValues("", "extra/extraIndex", "-analyzeduration 100000");
    QString strudp2 = "udp://192.168.6.170:7002";
	int hk = GetIniValues("", "extra/hk2", "0").toInt();
	if (1 == hk)
	{
		strudp2 = GetIniValues("", "extra/rtsp2", "rtsp://admin:admin12345@192.168.6.200:554/h264/ch1/main/av_stream -rtsp_transport tcp -analyzeduration 100000  ");
		QString player2 = GetIniValues("", "extra/player", "ffplay2.exe");
		QString strCmdExe2 = QString("%1 %2 -wid %3").arg(QString(player2)).arg(QString(strudp2)).arg(ui->vedio_2_vedio->winId());
		m_netProcess[1].start(strCmdExe2);
	}
	else
	{
		QString strCmdExe2 = QString("ffplay2.exe %1 -wid %2 -an  %3 ").arg(QString(strudp2)).arg(ui->vedio_2_vedio->winId()).arg(extraIndex);
		m_netProcess[1].start(strCmdExe2);
	}


    return 1;
}

int MainWindow::Start3()
{
    QString extraIndex;
    extraIndex = GetIniValues("", "extra/extraIndex", "-analyzeduration 100000");
    QString strudp3 = "udp://192.168.6.170:7004";
	int hk = GetIniValues("", "extra/hk3", "0").toInt();
	if (1 == hk)
	{
		strudp3 = GetIniValues("", "extra/rtsp3", "rtsp://admin:admin12345@192.168.6.200:554/h264/ch1/main/av_stream -rtsp_transport tcp -analyzeduration 100000 ");
		QString player3 = GetIniValues("", "extra/player", "ffplay2.exe");
		QString strCmdExe3 = QString("%1 %2 -wid %3").arg(QString(player3)).arg(QString(strudp3)).arg(ui->vedio_3_vedio->winId());
		m_netProcess[2].start(strCmdExe3);
	}
	else
	{
		QString strCmdExe3 = QString("ffplay2.exe %1 -wid %2 -an  %3 ").arg(QString(strudp3)).arg(ui->vedio_3_vedio->winId()).arg(extraIndex);
		m_netProcess[2].start(strCmdExe3);
	}


    return 1;
}

int MainWindow::Start4()
{
    QString extraIndex;
    extraIndex = GetIniValues("", "extra/extraIndex", "-analyzeduration 100000");
    QString strudp4 = "udp://192.168.6.170:7006";
	int hk = GetIniValues("", "extra/hk4", "0").toInt();
	if (1 == hk)
	{
		strudp4 = GetIniValues("", "extra/rtsp4", "rtsp://admin:admin12345@192.168.6.200:554/h264/ch1/main/av_stream -rtsp_transport tcp -analyzeduration 100000 ");
		QString player4 = GetIniValues("", "extra/player", "ffplay2.exe");
		QString strCmdExe4 = QString("%1 %2 -wid %3").arg(QString(player4)).arg(QString(strudp4)).arg(ui->vedio_4_vedio->winId());
		m_netProcess[3].start(strCmdExe4);
	}
	else
	{
		QString strCmdExe4 = QString("ffplay2.exe %1 -wid %2 -an  %3 ").arg(QString(strudp4)).arg(ui->vedio_4_vedio->winId()).arg(extraIndex);
		m_netProcess[3].start(strCmdExe4);
	}

    return 1;
}
//! [34]

//! [35]only test 手动关灯
void MainWindow::on_show_mode_red_clicked()
{
	QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
	quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
	closeLight(lightip1, lightport1);

	QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
	quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
	closeLight(lightip2, lightport2);

	QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
	quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
	closeLight(lightip3, lightport3);

	QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
	quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
	closeLight(lightip4, lightport4);

	Stop1();
	Stop2();
	Stop3();
	Stop4();

	Start1();
	Start2();
	Start3();
	Start4();
}
//! [35]

void MainWindow::slotResetLicenseCamera()
{
    resetLicenseCamera();
}

void MainWindow::slotReHeart()
{
    for (int i = 0; i < 4;i++)
    {
        if (m_faceserver)
        {
            NELV_FaceServerSendHeart(m_faceserver, i, 1);
        }
    }
}


void MainWindow::slotReLogin(int i)
{
	QPalette temPal;
	temPal.setColor(QPalette::WindowText, Qt::black);
	ui->car_setting->setPalette(temPal);
    switch (i)
    {
    case 1:
		ui->car_setting->setText(tr("relogIn success")); 
		ui->car_setting->setAlignment(Qt::AlignCenter); break;
    case 2:
		ui->car_setting->setText(tr("bufang success")); 
		ui->car_setting->setAlignment(Qt::AlignCenter); break;
    case 3:
		ui->car_setting->setText(tr("relogin fail")); 
		ui->car_setting->setAlignment(Qt::AlignCenter); break;
    default:
        break;
    }
}

void MainWindow::slotVedio()
{
	if (m_netProcess[0].state() == QProcess::Running && m_netProcess[1].state() == QProcess::Running && m_netProcess[2].state() == QProcess::Running && m_netProcess[3].state() == QProcess::Running)
	{
		m_vedioTimer.stop();
	}
	else
	{
		if (m_netProcess[0].state() == QProcess::Running)
		{
		}
		else
		{
			Stop1();
			Start1();
		}

		if (m_netProcess[1].state() == QProcess::Running)
		{
		}
		else
		{
			Stop2();
			Start2();
		}

		if (m_netProcess[2].state() == QProcess::Running)
		{
		}
		else
		{
			Stop3();
			Start3();
		}

		if (m_netProcess[3].state() == QProcess::Running)
		{
		}
		else
		{
			Stop4();
			Start4();
		}
	}
}

QString MainWindow::GetTestPath(QString ext, Facedata& mydata)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString imgPath;
    QString temScore;
    if (0 == mydata.m_index.toInt())
    {
        imgPath = appDir + "/" + "saveimage" + "/" + "0";
        temScore = m_passScore[0];

		temScore += "_pic";
		temScore += QString::number((int)m_savePicture[0]);
    }
    if (1 == mydata.m_index.toInt())
    {
        imgPath = appDir + "/" + "saveimage" + "/" + "1";
        temScore = m_passScore[1];

		temScore += "_pic";
		temScore += QString::number((int)m_savePicture[1]);
    }
    if (2 == mydata.m_index.toInt())
    {
        imgPath = appDir + "/" + "saveimage" + "/" + "2";
        temScore = m_passScore[2];

		temScore += "_pic";
		temScore += QString::number((int)m_savePicture[2]);
    }
    if (3 == mydata.m_index.toInt())
    {
        imgPath = appDir + "/" + "saveimage" + "/" + "3";
        temScore = m_passScore[3];

		temScore += "_pic";
		temScore += QString::number((int)m_savePicture[3]);
    }
    
    QDir dir(imgPath);
    dir.mkpath(imgPath);

    QString strPath;
    int i = 0;
    while (1)
    {
        temScore += "_";
        temScore += QString::number(i);

        strPath = QString("%1/%2.%3").arg(imgPath).arg(temScore).arg(ext);
        
        if (QFileInfo::exists(strPath))
        {
            i++;
			temScore.chop(2);
            continue;
        }
        else
        {
            break;
        }

		
    }
    return strPath;
}

void MainWindow::TestSaveImg(Facedata& mydata)
{
    mydata.m_identImage;
    QString filePath = GetTestPath("bmp", mydata);
    QFile file(filePath);

    if (file.open(QIODevice::ReadWrite))
    {
        file.write(mydata.m_identImage);
    }
}

void MainWindow::TestSaveJpg(Facedata& mydata)
{
    int num = mydata.m_listImages.size();
    if (num)
    {
        for (int i = 0; i < num; i++)
        {
            QString filePath = GetTestPath("jpg", mydata);
            QFile file(filePath);

            if (file.open(QIODevice::ReadWrite))
            {
                file.write(mydata.m_listImages[i]);
            }
        }
    }
}

void MainWindow::pass()
{
    //通行之后亮绿灯
	QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
	quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
	greenLight(lightip1, lightport1);
	QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
	quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
	greenLight(lightip2, lightport2);
	QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
	quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
	greenLight(lightip3, lightport3);
	QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
	quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
	greenLight(lightip4, lightport4);
	qDebug() << __FUNCTION__ << " " << __LINE__;

	for (int i = 0; i < 4;i++)
	{
		if (m_listImageMaxSize[i] == 0)
		{
			m_curShowData[i].m_per = "";
		}
	}
	

	if (0 == GetIniValues("", "mysql/mysql", "0").toInt())
    {
		//存入数据库
		saveSql(1);
    }
	else
	{
		saveSqlBrief(1);
	}

    if (m_faceserver)
    {
        //发送重置信号
        m_notify.setIndexStatus(-1, cardface_notify_idle);
    }
	qDebug() << __FUNCTION__ << " " << __LINE__;
    //清除主界面所有人员信息
    ui->vedio_1->clear();
    ui->vedio_1_ref->clear();
    ui->pass1_textName->clear();
    ui->pass1_textAddress->clear();
    ui->pass1_textGender->clear();
    ui->pass1_textID->clear();
    ui->pass1_textCardPart->clear();
    ui->pass1_textCardTime->clear();
    ui->pass1_textNation->clear();
    ui->pass1_face->clear();
    ui->pass1_ID->clear();
    ui->pass1_face_number->setText("");
    ui->pass1_ID_text->setText("");

    ui->vedio_2->clear();
    ui->vedio_2_ref->clear();
    ui->pass2_textName->clear();
    ui->pass2_textAddress->clear();
    ui->pass2_textGender->clear();
    ui->pass2_textID->clear();
    ui->pass2_textCardPart->clear();
    ui->pass2_textCardTime->clear();
    ui->pass2_textNation->clear();
    ui->pass2_face->clear();
    ui->pass2_ID->clear();
    ui->pass2_face_number->setText("");
    ui->pass2_ID_text->setText("");

    ui->vedio_3->clear();
    ui->vedio_3_ref->clear();
    ui->pass3_textName->clear();
    ui->pass3_textAddress->clear();
    ui->pass3_textGender->clear();
    ui->pass3_textID->clear();
    ui->pass3_textCardPart->clear();
    ui->pass3_textCardTime->clear();
    ui->pass3_textNation->clear();
    ui->pass3_face->clear();
    ui->pass3_ID->clear();
    ui->pass3_face_number->setText("");
    ui->pass3_ID_text->setText("");

    ui->vedio_4->clear();
    ui->vedio_4_ref->clear();
    ui->pass4_textName->clear();
    ui->pass4_textAddress->clear();
    ui->pass4_textGender->clear();
    ui->pass4_textID->clear();
    ui->pass4_textCardPart->clear();
    ui->pass4_textCardTime->clear();
    ui->pass4_textNation->clear();
    ui->pass4_face->clear();
    ui->pass4_ID->clear();
    ui->pass4_face_number->setText("");
    ui->pass4_ID_text->setText("");

// 	ui->all_green->setStyleSheet("background-color: yellow; border-radius:45px;padding:2px 4px;");
// 
// 	ui->pass1_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass1_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
    ui->all_green->clear();
    ui->car_picture->clear();
    ui->car_license_picture->clear();
    ui->car_license->clear();
//     ui->mac->clear();
	qDebug() << __FUNCTION__ << " " << __LINE__;
	m_vedioTimer.stop();
	qDebug() << __FUNCTION__ << " " << __LINE__;
	ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
	ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
	ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
	ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));

	for (int i = 0; i < 4; i++)
	{
		m_savePicture[i] = 0;
		//清空客户端图片信息
		m_listImageMaxSize[i] = 0;
		m_failpassenger[i] = 0;
		m_cardNotify[i] = 0;
		m_lightpass[i] = 1;
		m_checkpass[i] = 1;
		m_perPass[i] = 1;
		m_newMessage[i] = 0;

		for (int j = 0; j < 3; j++)
		{
			m_curImage[i][j] = NULL;
			m_curPer[i][j] = "";
		}
		m_curTimes[i] = 0;

		m_curShowData[i].ClearData();
		m_curShowData[i].m_per = "0";
		m_curShowData[i].m_strNetData = NULL;
		m_curShowData[i].m_netdata = NULL;
	}

	m_newCar = 1;
	m_pass = 1;
	m_passauto = 0;
    m_failauto = 0;
	m_flag = 0;
	m_idPass = 0;
	m_unifiedTime = "";
	m_carLicense = "";
	m_littleImage = NULL;
	m_largeImage = NULL;
}

void MainWindow::fail()
{
    //目前添加关灯到这个按钮
	QString lightip1 = GetIniValues("", "ip/lightip1", "192.168.6.18");
	quint16 lightport1 = GetIniValues("", "ip/lightport1", "50000").toInt();
	redLight(lightip1, lightport1);
	QString lightip2 = GetIniValues("", "ip/lightip2", "192.168.6.19");
	quint16 lightport2 = GetIniValues("", "ip/lightport2", "50000").toInt();
	redLight(lightip2, lightport2);
	QString lightip3 = GetIniValues("", "ip/lightip3", "192.168.6.20");
	quint16 lightport3 = GetIniValues("", "ip/lightport3", "50000").toInt();
	redLight(lightip3, lightport3);
	QString lightip4 = GetIniValues("", "ip/lightip4", "192.168.6.21");
	quint16 lightport4 = GetIniValues("", "ip/lightport4", "50000").toInt();
	redLight(lightip4, lightport4);
    //存入数据库
	qDebug() << __FUNCTION__ << " " << __LINE__;

	for (int i = 0; i < 4; i++)
	{
		if (m_listImageMaxSize[i] == 0)
		{
			m_curShowData[i].m_per = "";
		}
	}

	if (0 == GetIniValues("", "mysql/mysql", "0").toInt())
	{
		//存入数据库
		saveSql(0);
	}
	else
	{
		saveSqlBrief(0);
	}

    if (m_faceserver)
    {
        m_notify.setIndexStatus(-1, cardface_notify_idle);
    }
	qDebug() << __FUNCTION__ << " " << __LINE__;
    //清除主界面所有人员信息
    ui->vedio_1->clear();
    ui->vedio_1_ref->clear();
    ui->pass1_textName->clear();
    ui->pass1_textAddress->clear();
    ui->pass1_textGender->clear();
    ui->pass1_textID->clear();
    ui->pass1_textCardPart->clear();
    ui->pass1_textCardTime->clear();
    ui->pass1_textNation->clear();
    ui->pass1_face->clear();
    ui->pass1_ID->clear();
    ui->pass1_face_number->setText("");
    ui->pass1_ID_text->setText("");

    ui->vedio_2->clear();
    ui->vedio_2_ref->clear();
    ui->pass2_textName->clear();
    ui->pass2_textAddress->clear();
    ui->pass2_textGender->clear();
    ui->pass2_textID->clear();
    ui->pass2_textCardPart->clear();
    ui->pass2_textCardTime->clear();
    ui->pass2_textNation->clear();
    ui->pass2_face->clear();
    ui->pass2_ID->clear();
    ui->pass2_face_number->setText("");
    ui->pass2_ID_text->setText("");

    ui->vedio_3->clear();
    ui->vedio_3_ref->clear();
    ui->pass3_textName->clear();
    ui->pass3_textAddress->clear();
    ui->pass3_textGender->clear();
    ui->pass3_textID->clear();
    ui->pass3_textCardPart->clear();
    ui->pass3_textCardTime->clear();
    ui->pass3_textNation->clear();
    ui->pass3_face->clear();
    ui->pass3_ID->clear();
    ui->pass3_face_number->setText("");
    ui->pass3_ID_text->setText("");

    ui->vedio_4->clear();
    ui->vedio_4_ref->clear();
    ui->pass4_textName->clear();
    ui->pass4_textAddress->clear();
    ui->pass4_textGender->clear();
    ui->pass4_textID->clear();
    ui->pass4_textCardPart->clear();
    ui->pass4_textCardTime->clear();
    ui->pass4_textNation->clear();
    ui->pass4_face->clear();
    ui->pass4_ID->clear();
    ui->pass4_face_number->setText("");
    ui->pass4_ID_text->setText("");

// 	ui->all_green->setStyleSheet("background-color: yellow; border-radius:45px;padding:2px 4px;");
// 
// 	ui->pass1_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass1_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass2_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass3_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_face->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
// 	ui->pass4_ID->setStyleSheet("background-color: yellow; border-radius:20px;padding:2px 4px;");
    ui->all_green->clear();
    ui->car_picture->clear();
    ui->car_license_picture->clear();
    ui->car_license->clear();
//     ui->mac->clear();
	qDebug() << __FUNCTION__ << " " << __LINE__;
	m_vedioTimer.stop();
	qDebug() << __FUNCTION__ << " " << __LINE__;
    ui->pass1_textID->setTextBackgroundColor(QColor(Qt::white));
    ui->pass2_textID->setTextBackgroundColor(QColor(Qt::white));
    ui->pass3_textID->setTextBackgroundColor(QColor(Qt::white));
    ui->pass4_textID->setTextBackgroundColor(QColor(Qt::white));

	for (int i = 0; i < 4; i++)
	{
		m_savePicture[i] = 0;
		//清零其他客户端图片信息
		m_listImageMaxSize[i] = 0;
		m_failpassenger[i] = 0;
		m_cardNotify[i] = 0;
		m_lightpass[i] = 1;
		m_checkpass[i] = 1;
		m_perPass[i] = 1;
		m_newMessage[i] = 0;

		for (int j = 0; j < 3;j++)
		{
			m_curImage[i][j] = NULL;
			m_curPer[i][j] = "";
		}
		m_curTimes[i] = 0;

		m_curShowData[i].ClearData();
		m_curShowData[i].m_per = "0";
		m_curShowData[i].m_strNetData = NULL;
		m_curShowData[i].m_netdata = NULL;
	}

    m_passauto = 0;
    m_failauto = 0;
	m_newCar = 1;
	m_pass = 1;
	m_flag = 0;
	m_idPass = 0;
	m_unifiedTime = "";
	m_carLicense = "";
	m_littleImage = NULL;
	m_largeImage = NULL;
}

void MainWindow::slotAuto()
{
    if (1 == m_autolight && 0 == m_idPass)
    {
		pass();
		m_autolight = 0;
    }    
}

void MainWindow::on_pass1_information_clicked()
{
    Stop1();

    Start1();
}

void MainWindow::on_pass2_information_clicked()
{
    Stop2();

    Start2();
}

void MainWindow::on_pass3_information_clicked()
{
    Stop3();

    Start3();
}

void MainWindow::on_pass4_information_clicked()
{
//    Stop4();

//    Start4();

    m_bgcolor++;

    ui->centralWidget->setAutoFillBackground(true);
    QPalette palette;

    QString bgstr = QString(":/images/%1").arg(m_bgcolor);

    QPixmap pixmap(bgstr);
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    ui->centralWidget->setPalette(palette);

    if(17 == m_bgcolor)
    {
        m_bgcolor=0;
    }
}

void MainWindow::on_carCamSet_clicked()
{
	SetIpDialog setIp(this);
	setIp.exec();
	setIp.deleteLater();
}

void MainWindow::on_sqlInf_clicked()
{
	infdialog allInformation(this);
	allInformation.exec();
	allInformation.deleteLater();
}

void MainWindow::on_logInf_clicked()
{
    LoginDialog login(this);
    login.exec();
    login.deleteLater();
}

void MainWindow::on_pushButton_clicked()
{
    m_bgcolor++;

    ui->centralWidget->setAutoFillBackground(true);
    QPalette palette;

	QString bgstr = QString(":/images/%1").arg(m_bgcolor);

	QPixmap pixmap(bgstr);
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    ui->centralWidget->setPalette(palette);

    if(17 == m_bgcolor)
    {
        m_bgcolor=0;
    }
}
