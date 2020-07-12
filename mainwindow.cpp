#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    selectRow = -1; // 默认的没有选择
    conn = new Connection();

    int row = 0;
    int col = 4;
    ui->tableWidget->setColumnCount(col);
    ui->tableWidget->setRowCount(row);

    QStringList header;
    header << "序号" << "事情" << "日期" << "操作";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(1);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中一行，
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    ui->tableWidget->show();

    // http://cn.voidcc.com/question/p-csrholjn-qd.html
    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(displayTodo(int,int)));

    // 新增数据
    inputDialog = new InputDialog();
    connect(inputDialog, SIGNAL(sendInfo(Info*)), this, SLOT(receviedData(Info*)));
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    aboutDialog = new AboutDialog(this);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::openAboutDialog);

    // 修改数据
    editDialog = new EditDialog(this);
    connect(conn, SIGNAL(sendAllData(QVector<Info*>)), this, SLOT(receviedAllData(QVector<Info*>)));


    setWindowTitle("办事清单");
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::openAboutDialog()
{
    aboutDialog->show();
}
void MainWindow::displayTodo(int row, int/* col*/)
{
    selectRow = row;
    qDebug() << "=>" << selectRow;

    QTableWidgetItem *itemId = new QTableWidgetItem;
    QTableWidgetItem *itemDescription = new QTableWidgetItem;
    QTableWidgetItem *itemOperation = new QTableWidgetItem;
    QTableWidgetItem *itemDate = new QTableWidgetItem;

    itemId = ui->tableWidget->item(row, 0);
    itemDescription = ui->tableWidget->item(row, 1);
    itemOperation = ui->tableWidget->item(row, 2);
    itemDate = ui->tableWidget->item(row, 3);

    QString str = QString("序号: %1\n描述：%2\n操作：%3\n时间：%4\n")
             .arg(itemId->text())
             .arg(itemDescription->text())
             .arg(itemOperation->text())
             .arg(itemDate->text());

    ui->label->setText(str);
}


void MainWindow::on_pushButton_clicked()
{
    inputDialog->initInfo();
    inputDialog->exec();
}

void MainWindow::receviedData(Info *info)
{
    // 插入数据
    Connection *conn = new Connection();
    conn->insertDb(info);

    info->toString();
    QString id = info->id;
    QString description = info->description;
    QString date = info->createDate;
    QString operation = info->operation;


    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount); // 增加一行
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(id));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(description));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(date));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(operation));
}

void MainWindow::on_buttonEdit_clicked()
{
    // 如果，用户没有 选择中的任何数据，那么就不触发 更新对话框
    if (selectRow == -1) {
        QMessageBox::warning(this, "警告", "你没有选中数据");
        return;
    }

    editDialog->initInfo();
    editDialog->exec();
}

void MainWindow::on_buttonLoadData_clicked()
{
    conn->loadData();
}

//
void MainWindow::receviedAllData(QVector<Info *> infos)
{
   qDebug() << infos;
}
