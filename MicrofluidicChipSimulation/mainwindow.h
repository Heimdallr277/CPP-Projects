#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QPainter>
#include <dialog.h>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <vector>
#include <set>
#include <QColor>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

#include "calculator.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow   //继承自QMainwindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setup(int a, int b, int c, int d, int e, int f);
                            //用于从Dialog处接收输入的数据并存储
    int chosen;             //单选模式里被选中管道的序号
    set<int> selected;      //多选模式里存储被选中管道序号的set

private slots:
    void on_del_clicked();      //移除管道键按下连接的槽函数
    void on_add_clicked();      //添加管道键按下连接的槽函数
    void on_reverse_clicked();  //一键反转键按下连接的槽函数
    void on_multiselect_toggled(bool checked);
                                //多选模式按钮切换连接的槽函数
    void on_resetbtn_clicked(); //重设输入/出管道键按下连接的槽函数
    void calc();                //进行计算键按下连接的槽函数
    void on_checkspeed_clicked();//查看管道流速键按下连接的槽函数
    void on_showspeed_stateChanged(int arg1);//流速示意图切换连接的槽函数
    void on_undo_clicked();     //撤销按钮的槽函数
    void on_redo_clicked();     //重做按钮的槽函数

private:
    Ui::MainWindow *ui;     //图形界面指针
    Calculator *calculator; //存储了一个Calculator的指针成员，
                            //并在构造函数中分配内存，为所需要的计算进行准备
    QAction *inputaction;   //表示输入数据的动作
    QAction *openaction;    //表示读入芯片数据的动作
    QAction *saveaction;    //表示存储芯片数据的动作
    void input();           //用于输入数据的函数（调出对话框）
    void open();            //用于读取芯片数据的函数
    void save();            //用于存储芯片数据的函数
    bool init;				//用于表示数据输入是否完成
    bool calculated;        //用于表示计算是否完成，可以选择管道查看流速
    int r, i1, i2, o1, o2, o3;          //储存管道图的相关数据
    int undotype;           //暂存上一步操作类型，1为单选添加，2为单选删除，
                            //3为多选添加，4为多选删除，5为反转，用于撤销
    bool redoable;          //撤销操作完成的标识，表示可以进行重做
    int undonum;            //暂存上一步操作选中的一条管道
    set<int> undobuffer;    //暂存上一步操作选中的多条管道
    void paintEvent(QPaintEvent* e);	//绘制事件
    void closeEvent(QCloseEvent *event);//关闭窗口的事件
    vector<double> leng;                //存储各管道长度，用于计算
    void click(const QPoint &p);        //鼠标点击事件中调用的处理坐标的函数
    void mousePressEvent(QMouseEvent*); //鼠标点击事件
    void keyPressEvent(QKeyEvent* e);	//键盘按下事件
    QColor setcolorbyspeed(double s);  //根据流速设置在流速示意图中应显示的颜色
};


#endif // MAINWINDOW_H
