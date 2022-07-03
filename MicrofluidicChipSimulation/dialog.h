#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0, int reset = 0);
    int getrow(){return r;}
    int getin1(){return i1;}
    int getin2(){return i2;}
    int getout1(){return o1;}
    int getout2(){return o2;}
    int getout3(){return o3;}   //对外接口
    bool ok(int i);     //判断输入数据是否合法
    int reset;          //是否为重设对话框的标志，并存下当前的行列数的值
    ~Dialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog *ui;
    int r, i1, i2, o1, o2, o3;  //用于存输入的信息
};

#endif // DIALOG_H
