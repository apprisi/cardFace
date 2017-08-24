#ifndef ALLDATA_H
#define ALLDATA_H

#include <QDialog>
#include <QTableWidget>
class MainWindow;

namespace Ui {
class alldata;
}

class alldata : public QDialog
{
    Q_OBJECT

public:
    explicit alldata(QWidget *parent = 0);
    ~alldata();

private:
    Ui::alldata *ui;
    //连接主界面指针
    MainWindow *m_pdataparent;
};

#endif // ALLDATA_H
