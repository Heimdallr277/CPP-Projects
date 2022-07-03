#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    chosen(-1),
    ui(new Ui::MainWindow),
    init(false), calculated(false),
    r(0), i1(0), i2(0), o1(0), o2(0), o3(0),
    undotype(-1), redoable(false), undonum(-1)
{
    ui->setupUi(this);

    inputaction = new QAction(QIcon(":/images/input.png"),tr("&Input info..."),this);
    inputaction->setShortcut(QKeySequence::New);
    inputaction->setStatusTip(tr("Input info..."));

    openaction = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openaction->setShortcut(QKeySequence::Open);
    openaction->setStatusTip(tr("Open an existing file..."));

    saveaction = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveaction->setShortcut(QKeySequence::Save);
    saveaction->setStatusTip(tr("Save current data..."));

    QMenu *input = menuBar()->addMenu(tr("&Input"));
    input->addAction(inputaction);
    QMenu *open = menuBar()->addMenu(tr("&Open"));
    open->addAction(openaction);
    QMenu *save = menuBar()->addMenu(tr("&Save"));
    save->addAction(saveaction);

    connect(inputaction, &QAction::triggered, this, &MainWindow::input);
    connect(openaction, &QAction::triggered, this, &MainWindow::open);
    connect(saveaction, &QAction::triggered, this, &MainWindow::save);

    ui->mainToolBar->addAction(inputaction);
    ui->mainToolBar->addAction(openaction);
    ui->mainToolBar->addAction(saveaction);

    statusBar();

    setCentralWidget(ui->layout);
    ui->showspeed->setCheckable(false);

    ui->undo->setEnabled(false);
    ui->redo->setEnabled(false);

    calculator = new Calculator;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        bool exit = QMessageBox::question(this,
                                      tr("退出程序"),
                                      tr("你真的要退出程序吗？"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No) == QMessageBox::Yes;
        if (exit) {
            event->accept();
        } else {
            event->ignore();
        }

}

void MainWindow::setup(int a, int b, int c, int d, int e, int f) {
    r = a; i1 = b; i2 = c; o1 = d; o2 = e; o3 = f;
    init = true;
    leng.assign(2*r*r-2*r+5, 0);
    for (int i = 2*r*r-2*r; i< 2*r*r-2*r+5; i++) {
        leng[i] = 1;
    }
    ui->ans1->setText("1. ");
    ui->ans2->setText("2. ");
    ui->ans3->setText("3. ");
    ui->speed->clear();
    ui->pipenum->clear();
    chosen = -1;
    undonum = -1;
    undotype = -1;
    redoable = 0;
    undobuffer.clear();
    selected.clear();
    calculated = false;
    ui->showspeed->setCheckable(false);
    statusBar()->clearMessage();
    update();
}

void MainWindow::input() {
    Dialog *dialog = new Dialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        setup(dialog->getrow(), dialog->getin1(), dialog->getin2(),
              dialog->getout1(), dialog->getout2(), dialog->getout3());
    }
}

void MainWindow::open() {
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("打开文件"),
                                                ".",
                                                tr("Chip Data(*.chip)"));
    if(!path.isEmpty()) {//通过判断 path 是否为空，可以确定用户是否选择了某一文件
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            //打开这个文件，使用的是 QFile::open()，其参数是指定的打开方式，这里我们使用只读方式和文本方式打开这个文件，打开成功则返回true
            QMessageBox::warning(this, tr("Warning!"),
                                 tr("无法读取:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);
        in >> r >> i1 >> i2 >> o1 >> o2 >> o3;
        setup(r, i1, i2, o1, o2, o3);
        for (int i = 0; i < 2*r*r-2*r+5; i++) {
            in >> leng[i];
        }
        QMessageBox::information(this, tr("Done!"), tr("读取数据成功！"));
        file.close();
    } else {
        QMessageBox::warning(this, tr("Error!"),
                             tr("未选中任何文件！"));
    }
}

void MainWindow::save() {
    if (init==false) {
        QMessageBox::warning(this, tr("Error!"),
                             tr("芯片信息不完整！"));
        return;
    }
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("Chip Data(*.chip)"));
    if(!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Warning!"),
                                       tr("无法写入:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << r << "\n";
        out << i1 << "\n";
        out << i2 << "\n";
        out << o1 << "\n";
        out << o2 << "\n";
        out << o3 << "\n";
        for (int i = 0; i < 2*r*r-2*r+5; i++) {
            out << leng[i] << "\n";
        }
        QMessageBox::information(this, tr("Done!"), tr("存储数据成功！"));
        file.close();
    } else {
        QMessageBox::warning(this, tr("Error!"),
                             tr("未选中任何文件！"));
    }
}

QColor MainWindow::setcolorbyspeed(double s) {
    if (s <= 100) {
        return QColor(0,(1-s/100)*255,255);
    }
    else if (s <= 200) {
        return QColor(0,255,(200-s)/100*255);
    }
    else if (s <= 300) {
        return QColor((s-200)/100*255,255,0);
    }
    else if (s <= 400) {
        return QColor(255,(400-s)/100*255,0);
    }
}

void MainWindow::paintEvent(QPaintEvent*) {
    if (init == false) {
        return;
    }
    QPainter painter(this);
    painter.translate(150, 150);
    painter.drawRect(0, 0, (r-1)*50+10, (r-1)*50+10);
    int sum = 0;

    if (ui->showspeed->isChecked()==false) {
        for (int i = 0; i <= r-1; i++) {
            for (int j = 0; j < r-1; j++) {
                if (ui->multiselect->isChecked()&&selected.find(sum)!= selected.end()) {//多选模式
                    painter.setBrush(QBrush(Qt::red));
                }
                else if (ui->multiselect->isChecked()==false&&sum==chosen) {//单选模式
                    painter.setBrush(QBrush(Qt::red));
                } else {
                    if (leng[sum]>0) {
                        painter.setBrush(QBrush(Qt::blue));
                    } else {
                        painter.setBrush(QBrush(Qt::green));
                    }
                }
                painter.drawRect(50*i, 50*j+10, 10, 40);//画纵向管道
                sum++;
            }
        }

        for (int i = 0; i < r-1; i++) {
            for (int j = 0; j <= r-1; j++) {
                if (ui->multiselect->isChecked()&&selected.find(sum)!= selected.end()) {
                    painter.setBrush(QBrush(Qt::red));
                }
                else if (ui->multiselect->isChecked()==false&&sum==chosen) {
                    painter.setBrush(QBrush(Qt::red));
                } else {
                    if (leng[sum]>0) {
                        painter.setBrush(QBrush(Qt::blue));
                    } else {
                        painter.setBrush(QBrush(Qt::green));
                    }
                }
                painter.drawRect(50*i+10, 50*j, 40, 10);//画横向管道
                sum++;
            }
        }

        painter.setBrush(QBrush(Qt::blue));
        painter.drawRect(50*(i1-1), -40, 10, 40);
        painter.drawRect(50*(i2-1), -40, 10, 40);
        painter.drawRect(50*(o1-1), (r-1)*50+10, 10, 40);
        painter.drawRect(50*(o2-1), (r-1)*50+10, 10, 40);
        painter.drawRect(50*(o3-1), (r-1)*50+10, 10, 40);

        if (undotype>0) {
            if (redoable) {
                ui->undo->setEnabled(false);
                ui->redo->setEnabled(true);
            } else {
                ui->undo->setEnabled(true);
                ui->redo->setEnabled(false);
            }
        } else {
            ui->undo->setEnabled(false);
            ui->redo->setEnabled(false);
        }

    } else { //显示流速示意图
        QLinearGradient linearGradient(-50, 0, -50, (r-1)*50+10);
        linearGradient.setColorAt(0.0, QColor(0, 0, 255));
        linearGradient.setColorAt(0.25, QColor(0, 255, 255));
        linearGradient.setColorAt(0.5, QColor(0, 255, 0));
        linearGradient.setColorAt(0.75, QColor(255, 255, 0));
        linearGradient.setColorAt(1.0, QColor(255, 0, 0));

        painter.setBrush(QBrush(linearGradient));
        painter.drawRect(-50, 0, 10, (r-1)*50+10);
        painter.drawText(QPoint(-50, -10), "low");
        painter.drawText(QPoint(-50, (r-1)*50+30), "high");

        for (int i = 0; i <= r-1; i++) {
            for (int j = 0; j < r-1; j++) {
                painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum])));
                painter.drawRect(50*i, 50*j+10, 10, 40);//画纵向管道
                sum++;
            }
        }
        for (int i = 0; i < r-1; i++) {
            for (int j = 0; j <= r-1; j++) {
                painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum])));
                painter.drawRect(50*i+10, 50*j, 40, 10);//画横向管道
                sum++;
            }
        }
        painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum])));
        painter.drawRect(50*(i1-1), -40, 10, 40);
        painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum+1])));
        painter.drawRect(50*(i2-1), -40, 10, 40);
        painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum+2])));
        painter.drawRect(50*(o1-1), (r-1)*50+10, 10, 40);
        painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum+3])));
        painter.drawRect(50*(o2-1), (r-1)*50+10, 10, 40);
        painter.setBrush(QBrush(setcolorbyspeed(calculator->speed[sum+4])));
        painter.drawRect(50*(o3-1), (r-1)*50+10, 10, 40);
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    click(event->pos());
}

void MainWindow::click(const QPoint &p) {
    int x = p.x()-150;
    int y = p.y()-150;
    if (x<0||x>(r-1)*50+10||y<0||y>(r-1)*50+10) {
        return;
    }
    int a = x/50 + 1;
    int b = y/50 + 1;
    int m = x % 50;
    int n = y % 50;
    int choice = -1;
    if (m>0&&m<10&&n>10&&n<50) {
        choice = (r-1)*(a-1) + b-1;
    }
    if (m>10&&m<50&&0<n&&n<10) {
        choice = r*(r-1) + r*(a-1) + b-1;
    }
    if (choice<0)
        return;
    if (ui->multiselect->isChecked()==false) {//单选模式
        if (chosen == choice) {
            chosen = -1;
            statusBar()->clearMessage();
        } else {
            chosen = choice;
            statusBar()->showMessage("Pipe No."+QString::number(chosen)+" selected.");
        }
        update();
    } else if (ui->multiselect->isChecked()) {//多选模式
        if (selected.empty()) {
            selected.insert(choice);
            statusBar()->showMessage("1 pipes selected.");
        } else {
            if (selected.find(choice)== selected.end()) {//未选中
                selected.insert(choice);
            } else {//选中了
                selected.erase(choice);
            }
            statusBar()->showMessage(QString::number(selected.size())+" pipes selected.");
        }
        update();
    }
}


void MainWindow::on_del_clicked()
{
    if (ui->multiselect->isChecked()==false) {//单选模式
        if (chosen < 0) {
            QMessageBox::information(this, tr("Error!"), tr("请选择一条管道进行操作！"));
            return;
        }
        if (leng[chosen] <= 0) {
            QMessageBox::information(this, tr("Error!"), tr("该管道不存在或已被删除！"));
            return;
        }
        leng[chosen] = 0;
        QMessageBox::information(this, tr("Done!"), tr("删除成功！"));
        undotype = 2;
        undonum = chosen;
        chosen = -1;
        update();
    } else {
        if (selected.empty()) {
            QMessageBox::information(this, tr("Error!"), tr("请至少选择一条管道进行操作！"));
            return;
        }
        for (auto it = selected.begin(); it != selected.end(); it++) {
            if (leng[(*it)] <= 0) {
                QMessageBox::information(this, tr("Error!"), tr("选中的有不存在或已被删除的管道！"));
                return;
            }
        }
        for (auto it = selected.begin(); it != selected.end(); it++) {
            leng[(*it)] = 0;
        }
        QMessageBox::information(this, tr("Done!"), tr("批量删除成功！"));
        undotype = 4;
        undobuffer.clear();
        for (auto it = selected.begin(); it != selected.end(); it++) {
            undobuffer.insert(*it);
        }
        selected.clear();
        update();
    }

}

void MainWindow::on_add_clicked()
{
    if (ui->multiselect->isChecked()==false) {//单选模式
        if (chosen < 0) {
            QMessageBox::information(this, tr("Error!"), tr("请选择一条管道进行操作！"));
            return;
        }
        if (leng[chosen] > 0) {
            QMessageBox::information(this, tr("Error!"), tr("该管道已存在！"));
            return;
        }
        leng[chosen] = 1;
        QMessageBox::information(this, tr("Done!"), tr("添加成功！"));
        undotype = 1;
        undonum = chosen;
        chosen = -1;
        update();
    } else {
        if (selected.empty()) {
            QMessageBox::information(this, tr("Error!"), tr("请至少选择一条管道进行操作！"));
            return;
        }
        for (auto it = selected.begin(); it != selected.end(); it++) {
            if (leng[(*it)] > 0) {
                QMessageBox::information(this, tr("Error!"), tr("选中的有已经存在的管道！"));
                return;
            }
        }

        for (auto it = selected.begin(); it != selected.end(); it++) {
            leng[(*it)] = 1;
        }
        QMessageBox::information(this, tr("Done!"), tr("批量添加成功！"));
        undotype = 3;
        undobuffer.clear();
        for (auto it = selected.begin(); it != selected.end(); it++) {
            undobuffer.insert(*it);
        }
        selected.clear();
        update();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (chosen < 0 && ui->multiselect->isChecked()==false) {
        return;
    } else if (selected.empty() && ui->multiselect->isChecked()==true) {
        return;
    } else {
        if (e->key()==Qt::Key_Delete) {
            on_del_clicked();
            return;
        }
        if (e->key()==Qt::Key_Insert) {
            on_add_clicked();
            return;
        }
    }
    if (chosen < r*(r-1)) {
        if (e->key()==Qt::Key_Up&&chosen%(r-1)!=0) {
            chosen--;
            update();
        }
        if (e->key()==Qt::Key_Down&&chosen%(r-1)!=r-2) {
            chosen++;
            update();
        }
        if (e->key()==Qt::Key_Left&&chosen/(r-1)!=0) {
            chosen = chosen - (r-1);
            update();
        }
        if (e->key()==Qt::Key_Right&&chosen/(r-1)!=r-1) {
            chosen = chosen + (r-1);
            update();
        }
    } else if (chosen >= r*(r-1)) {
        if (e->key()==Qt::Key_Up&&chosen%r!=0) {
            chosen--;
            update();
        }
        if (e->key()==Qt::Key_Down&&chosen%r!=r-1) {
            chosen++;
            update();
        }
        if (e->key()==Qt::Key_Left&&chosen/r!=r-1) {
            chosen = chosen - r;
            update();
        }
        if (e->key()==Qt::Key_Right&&chosen/r!=2*r-3) {
            chosen = chosen + r;
            update();
        }
    }
    statusBar()->showMessage("Pipe No."+QString::number(chosen)+" selected.");

}

void MainWindow::calc() {
    if (!init)
        return;
//    for (int i = 0; i < r*(r-1); i++) {
//        qDebug() << leng[i];
//    }
//    for (int i = r*(r-1); i < 2*r*r-2*r+5; i++) {
//        qDebug() << leng[i];
//    }
    vector<double> ans = calculator->caluconspeed(r,leng,i1-1,i2-1,o1-1,o2-1,o3-1);
    double a1 = ans[0];
    double a2 = ans[1];
    double a3 = ans[2];
//  qDebug() << a1 << " " << a2 << " "<< a3;
    ui->ans1->setText("1. "+QString::number(a1));
    ui->ans2->setText("2. "+QString::number(a2));
    ui->ans3->setText("3. "+QString::number(a3));
    calculated = true;
    ui->showspeed->setCheckable(true);
}

void MainWindow::on_reverse_clicked()
{
    if (init == false)
        return;
    for (int i = 0; i < 2*r*r-2*r; i++) {
        if (leng[i]>0) {
            leng[i]=0;
        } else {
            leng[i]=1;
        }
    }
    undotype = 5;
    update();
}

void MainWindow::on_multiselect_toggled(bool checked)
{
    if (checked) {
        chosen = -1;
    } else {
        selected.clear();
    }
    update();
}

void MainWindow::on_resetbtn_clicked()
{
    if (init==false)
        return;
    Dialog* dialog = new Dialog(this, r);
    if (dialog->exec() == QDialog::Accepted) {
        i1 = dialog->getin1();
        i2 = dialog->getin2();
        o1 = dialog->getout1();
        o2 = dialog->getout2();
        o3 = dialog->getout3();
        update();
    }
}

void MainWindow::on_checkspeed_clicked()
{
    if (init == false) {
        return;
    }
    if (calculated == false) {
        QMessageBox::information(this, tr("Error!"), tr("请完成计算后再操作！"));
        return;
    }
    if (ui->multiselect->isChecked()&&selected.size()>1) {
        QMessageBox::information(this, tr("Error!"), tr("请至多选择一条管道进行查看！"));
        return;
    }
    if ((ui->multiselect->isChecked()&&selected.empty()) || (ui->multiselect->isChecked()==false&&chosen<0)) {
        QMessageBox::information(this, tr("Error!"), tr("请选择一条管道进行查看！"));
        return;
    }
    if (ui->multiselect->isChecked()&&selected.size()==1) {
        ui->pipenum->setText(QString::number(*(selected.begin())));
        ui->speed->setText(QString::number(calculator->speed[*(selected.begin())]));
    } else if (ui->multiselect->isChecked()==false&&chosen>0) {
        ui->pipenum->setText(QString::number(chosen));
        ui->speed->setText(QString::number(calculator->speed[chosen]));
    }
}

void MainWindow::on_showspeed_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        if (calculated == false || init == false) {
            QMessageBox::information(this, tr("Error!"), tr("请完成计算后再操作！"));
            return;
        } else {
            ui->add->setEnabled(false);
            ui->del->setEnabled(false);
            ui->resetbtn->setEnabled(false);
            ui->reverse->setEnabled(false);
            ui->calc->setEnabled(false);
            ui->checkspeed->setEnabled(false);
            ui->undo->setEnabled(false);
            ui->redo->setEnabled(false);
            update();
        }
    } else if (arg1 == Qt::Unchecked) {
        ui->add->setEnabled(true);
        ui->del->setEnabled(true);
        ui->resetbtn->setEnabled(true);
        ui->reverse->setEnabled(true);
        ui->calc->setEnabled(true);
        ui->checkspeed->setEnabled(true);
        update();
    }
}

void MainWindow::on_undo_clicked()
{
    if (undotype <= 0) {
        return;
    } else {
        if (undotype == 1) {//如果是添加，则撤销操作为删除
            leng[undonum] = 0;
        }
        if (undotype == 2) {
            leng[undonum] = 1;
        }
        if (undotype == 3) {
            for (auto it = undobuffer.begin(); it != undobuffer.end(); it++) {
                leng[(*it)] = 0;
            }
        }
        if (undotype == 4) {
            for (auto it = undobuffer.begin(); it != undobuffer.end(); it++) {
                leng[(*it)] = 1;
            }
        }
        if (undotype == 5) {
            undonum = -1;
            undobuffer.clear();
            on_reverse_clicked();
        }
        redoable = true;
        update();
        QMessageBox::information(this, tr("Done!"), tr("撤销操作成功！"));
    }
}

void MainWindow::on_redo_clicked()
{
    if (redoable==false) {
        return;
    } else {
        if (undotype == 1) {//如果是添加，则重做操作仍为添加
            leng[undonum] = 1;
        }
        if (undotype == 2) {
            leng[undonum] = 0;
        }
        if (undotype == 3) {
            for (auto it = undobuffer.begin(); it != undobuffer.end(); it++) {
                leng[(*it)] = 1;
            }
        }
        if (undotype == 4) {
            for (auto it = undobuffer.begin(); it != undobuffer.end(); it++) {
                leng[(*it)] = 0;
            }
        }
        if (undotype == 5) {
            undonum = -1;
            undobuffer.clear();
            on_reverse_clicked();
        }
        redoable = false;
        update();
        QMessageBox::information(this, tr("Done!"), tr("重做操作成功！"));
    }
}
