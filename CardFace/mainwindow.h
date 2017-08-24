#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTableView>
#include <QStringListModel>
#include <QListView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QAbstractTableModel>
#include <QDir>
#include <QImage>
#include <QMouseEvent>
#include <QThread>
#include <QMediaPlayer>
#include <QList>
#include <QFont>
#include <QTableWidget>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QUdpSocket>
#include <QFile>
#include <QApplication>

#include "infdialog.h"
#include "netdataapi.h"
#include "netdatamgr.h"
#include "alldata.h"
#include "setipdialog.h"
#include "carusernotify.h"
#include "facedata.h"
#include "string.h"
#include "facedatanet.h"
#include "logindialog.h"

//设置一个全局函数用来调用qss
class CommonHelper
{
public:
    static void setStyle(const QString &style) {
        QFile qss(style);
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    }
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

        friend class alldata;
        friend class infdialog;
        friend class SetIpDialog;
        friend class LoginDialog;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //总的显示各个信息（接收完并分类好信号之后）
    void showImage(Facedata & data);
    //获取配置文件路径和值
    static QString GetIniPath(QString iniName);
    static QString GetIniValues(QString strIniName, QString key, QString defaultValue);
    //添加视频状态，开始和清除进程
    int Start1();
    int Stop1();
    int Start2();
    int Stop2();
    int Start3();
    int Stop3();
    int Start4();
    int Stop4();



protected:
    //重写鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    //重写获取各位置信息
    void GetFaceData(void*);
    void slotResetLicenseCamera();
    void slotReHeart();
    void slotReLogin(int);
    void slotAuto();
	void slotVedio();

private slots:
    //通过按钮
    void on_pass_clicked();
    //禁止按钮
    void on_fail_clicked();
    //获取摄像头车牌信息
    void slotGetCarData(int type, QStringList carID, QByteArray littleImage, QByteArray largeImage);
    //发送语音播报开始信息
    void sendClientCmd(int index, QString text);
    //手动语音播报（未通过成员）
    void on_voiceNotice_clicked();
    //手动开始获取信息，不能获取车牌信息（测试用）
    void on_startByHand_clicked();
    //显示各客户端是否连接
    void faceConnectClient(int index, int con);
    //设置车牌摄像头ip等基本信息
    void on_action_ip_triggered();
    //显示数据库信息
    void on_action_information_triggered();
    //根据勾选框改变状态控制显示不同信息和显示检测模式
    void on_checkBox_1_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);
    void on_checkBox_4_stateChanged(int arg1);
    //工具栏登录按钮
    void on_action_login_triggered();
    //only test 点击后显示红色
    void on_show_mode_red_clicked();

    void on_pass1_information_clicked();

    void on_pass2_information_clicked();

    void on_pass3_information_clicked();

    void on_pass4_information_clicked();

    void on_carCamSet_clicked();

    void on_sqlInf_clicked();

    void on_logInf_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    //打开数据库
    void openSql();
    //存入数据库
	void saveSql(int index);
	void saveSqlDetial(int index);
	void saveSqlBrief(int index);
    //重置（恢复默认）车牌摄像头基本信息
    void resetLicenseCamera();
    //设置车牌摄像头基本信息
    void setLicenseCamera();
    //更新获取网络mac地址并在主界面显示获取几个mac地址
    int updateNetMes();
    //设置音频文件
    void playaudio(QUrl url);
    //获取音频文件
    QString GetAudioFile(QString name);
    //设置配置文件中勾选检测位置的值
    int SetIniValues1(QString strIniName, int value);
    int SetIniValues2(QString strIniName, int value);
    int SetIniValues3(QString strIniName, int value);
    int SetIniValues4(QString strIniName, int value);
    //检测4个客户端是否提取到相同身份证号
    int checkID();
    //控制亮灯
	int redLight(QString lightip, quint16 lightport);
	int yellowLight(QString lightip, quint16 lightport);
	int greenLight(QString lightip, quint16 lightport);
	int closeLight(QString lightip, quint16 lightport);

    QString GetTestPath(QString ext, Facedata& mydata);
    void TestSaveJpg(Facedata& mydata);
    void TestSaveImg(Facedata& mydata);
    void TestSaveScore(Facedata& mydata);

    void pass();
    void fail();


    //创建本地数据对象用来接收传入数据
    Facedata m_curShowData[4];
    
    //创建服务器
    void* m_faceserver;
    
    //创建一个flag表示最终是否通过,=1允许通过，=0不允许通过
    int m_pass;
    
    //创建mac地址数据指针
    void* m_netmonitor;
    void* m_ser_process;
    
    //现场捕获照片list
    int m_listImageMaxSize[4];
    
    //创建4个flag对应四个摄像头，当不点击确定或者禁止时不接受新的信号，=1时接收信号，=0时不接收信号
    int m_newMessage[4];
    
    //创建1个flag对应车身摄像头扫描的车牌，为1时接受新信号
    int m_newCar;
    
    //获取手机mac地址
    QStringList m_dataMacStrList;
    QStringList m_dataMacStrListSub;
    QString m_strNetMonitorMes;
    QString m_dataText;
    QString m_maxMacNum;
    
    //设置音频文件信息
    QMediaPlayer m_player;  
    void *m_speaker;
    
    //获取各摄像头匹配度，用来播放语音
    int m_perPass[4];

    QString m_passScore[4];
    
    //设置接收信息状态，=1时持续接受信息，=0时不接受信息
    int m_receiveMessage;
   
    //区分用户使用语音
    CarUserNotify m_notify;
    
    //设置flag用来切换检测状态，=0不录信息，=1时允许录入信息
    int m_check[4];
    
    //设置flag保证成功读取到信息后不再继续读取，未成功读取时持续读取信息，=0不接收新信息，=1时接收新信息
    int m_checkpass[4];
    
    //设置可以循环检查一个位置是否通过来判定整个通过与否，=0时不允许通过，=1时允许通过
    int m_lightpass[4];
    
    //记录哪些客户端连接
    int m_connect[MAX_CARDFACE_NOTIFY_USER];    

    //定义一个UDP
    QUdpSocket *m_Socket[4];

    //定义一个记录是否登入的flag，=1显示信息，=0不显示
    int m_loginflag;

    //定义4个刷卡计数器对应四个客户端，=3时发送信号告知远端不再发送新数据
    int m_cardNotify[4];

    //记录每个客户端身份证号是否一致
    QString m_curID[4];

    //设置进程启动4个视频框
    QProcess m_netProcess[4];

    //定时器
    QTimer m_carTimer;

    int m_conSuccess;
    int m_doSetSuccess;

    QTimer m_clientTimer;    

	QTimer m_vedioTimer;
    
    //=1时可以自动通行，=0时手动通行
    int m_auto;

    //初始化为0，=4时自动通过
    int m_passauto;

    //初始化为0，=4时自动播报禁止语音
    int m_failauto;
    int m_failpassenger[4];

    QTimer m_autoPass;

    //=0时不执行pass，=1时执行pass
    int m_autolight;

	//定义flag用来判断是第几张的图片
	int m_savePicture[4];

	//开始刷卡和捕捉到车牌信息时定义flag为允许显示信息
	int m_flag;

	//记录车牌摄像头基本信息
	QString m_carCameraIP;
	QString m_carCameraPort;
	QString m_carCameraUser;
	QString m_carCameraPass;
	QString m_carLicense;
	QByteArray m_littleImage;
	QByteArray m_largeImage;

	//设置一个flag，当身份证相同时=1，该轮永远不通过
	int m_idPass;

	//创建数据库对象
	QSqlDatabase m_database;

	//暂存现场捕获照片和其评分
	QByteArray m_curImage[4][3];
	QString m_curPer[4][3];
	int m_curTimes[4];

	//添加不同车道分别
	int VehicleLane;

	//添加记录mysql中一个统一时间，方便查询
	QString m_unifiedTime;

    //设置换肤flag
    int m_bgcolor;
};


#endif // MAINWINDOW_H
