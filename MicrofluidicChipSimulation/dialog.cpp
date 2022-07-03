#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent, int reset) :
    QDialog(parent),
    ui(new Ui::Dialog),
    reset(reset)
{
    ui->setupUi(this);
    if (reset>0) {
        ui->row->setText(QString::number(reset));
        ui->row->setReadOnly(true);
        setWindowTitle("Reset info...");
    } else {
        setWindowTitle("Input info...");
    }
}

Dialog::~Dialog()
{
    delete ui;
}

bool Dialog::ok(int i) {
    return (i>0&&i<=r);
}

void Dialog::on_buttonBox_accepted()
{
    int row = ui->row->text().toInt();
    if (row>=5&&row<=8)
        r = row;
    int in1 = ui->in1->text().toInt()+1;
    int in2 = ui->in2->text().toInt()+1;
    int out1 = ui->out1->text().toInt()+1;
    int out2 = ui->out2->text().toInt()+1;
    int out3 = ui->out3->text().toInt()+1;
    bool flag = false;
    if (row>=5&&row<=8&&ok(in1)&&ok(in2)&&ok(out1)&&ok(out2)&&ok(out3)) {
        if (in1!=in2&&out1!=out2&&out2!=out3&&out1!=out3) {
            flag = true;
        }
    }
    if (flag) {
        if (in1 < in2) {
            i1 = in1;
            i2 = in2;
        } else {
            i1 = in2;
            i2 = in1;
        }
        if (out1 < out2 && out1 < out3) {
            o1 = out1;
            if (out2<out3){o2=out2; o3=out3;}
            else {o2=out3; o3=out2;}
        } else {
            if (out2<out3){
                o1=out2;
                if (out1<out3){o2=out1; o3=out3;}
                else {o2=out3; o3=out1;}
            }
            else {
                o1=out3;
                if (out1<out2){o2=out1; o3=out2;}
                else {o2=out2; o3=out1;}
            }
        }
        accept();
    } else {
        QMessageBox::information(this, tr("Error!"), tr("输入有误！请重新输入。"));
    }
}
