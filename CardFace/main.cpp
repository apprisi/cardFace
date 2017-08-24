#include "mainwindow.h"
#include "logindialog.h"
#include "dogauth.h"
#include <QApplication>
#include <QTranslator>
#ifdef WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
	//auth code check
	char str[255];
	int iRet = AuthorityMachineCode(NULL, "BJCardFace");
	if (iRet < 0)
	{
		::MessageBox(NULL, TEXT("×¢²áÊ§°Ü"),
			TEXT("ÌáÊ¾"), MB_OK | MB_ICONINFORMATION);

		return (S_FALSE);
	}

    QApplication a(argc, argv);

#ifdef WIN32
    HANDLE hJob = CreateJobObject(0, 0);
    if (hJob) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
        memset(&jeli, 0, sizeof(jeli));
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
            if (AssignProcessToJobObject(hJob, GetCurrentProcess())) {
            }
        }
    }
#endif

    QTranslator translator;
    bool b = false;
    QString cnqm = QCoreApplication::applicationDirPath() + "/ts/qtcn.qm";
    b = translator.load(cnqm);
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
// 	w.showMaximized();

    return a.exec();
}
