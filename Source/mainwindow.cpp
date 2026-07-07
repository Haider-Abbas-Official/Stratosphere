#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QUrl>
#include <QDateTime>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QScreen>
#include <fstream>
#include <QCloseEvent>
#include <QTimer>
using namespace std;

QString main_folder = QCoreApplication::applicationDirPath();
QString uploads = QDir(main_folder).filePath("Cloud/uploads");


void MainWindow::TableViewSet(QString path)
{
    folderModel = new QFileSystemModel(this);

    folderModel->setRootPath(path);

    ui->tableView->setModel(folderModel);
    ui->tableView->setRootIndex(folderModel->index(path));
    ui->tableView->hideColumn(2);
    ui->tableView->setColumnWidth(0, 475); // Name
    ui->tableView->setColumnWidth(1, 100); // Size
    ui->tableView->setColumnWidth(3, 150); // Modified
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tableView->setAcceptDrops(true);
    ui->tableView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->tableView->setDefaultDropAction(Qt::CopyAction);
    ui->tableView->setDropIndicatorShown(true);
    ui->tableView->setDragEnabled(true);


    connect(ui->tableView,
            &QTableView::doubleClicked,
            this,
            [=](const QModelIndex &index)
            {
                QString path = folderModel->filePath(index);

                QDesktopServices::openUrl(
                    QUrl::fromLocalFile(path)
                    );
            });

    folderModel->setReadOnly(false);

    //-----------Styling Table---------------

    QHeaderView *header = ui->tableView->horizontalHeader();

    // Center text
    header->setDefaultAlignment(Qt::AlignCenter);

    // Font
    QFont font;
    font.setFamily("Segoe UI");
    font.setPointSize(10);
    font.setBold(true);

    header->setFont(font);

}


void MainWindow::TrayIconSet()
{
    trayIcon = new QSystemTrayIcon(this);

    trayIcon->setIcon(windowIcon());
    trayIcon->show();

    connect(
        trayIcon,
        &QSystemTrayIcon::activated,
        this,
        [=](QSystemTrayIcon::ActivationReason reason)
        {
            if(reason ==
                QSystemTrayIcon::Trigger)
            {
                showNormal();
                activateWindow();
            }
        });
}


QString GetIP()
{
    QString port="";
    QString filePath = QCoreApplication::applicationDirPath() + "/Cloud/PORT.txt";

    QFile file(filePath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        port = QString::fromUtf8(file.readLine().trimmed());
        file.close();
    }


    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for (const QHostAddress &address : addresses)
    {
        if (address.protocol() ==
                QAbstractSocket::IPv4Protocol &&
            address !=
                QHostAddress::LocalHost)
        {
            return address.toString()+":"+port;
        }
    }


    return "127.0.0.1:"+port;
}


void MainWindow::saveDataRealtime()
{
    // Fetch the current text
    QString currentText = ui->textEdit->toPlainText();

    // Save to a file
    QString path = QCoreApplication::applicationDirPath() + "/Cloud";
    QFile file(path + "/Text.txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << currentText;
        file.close();
    }
}

void MainWindow::loadData()
{
    QString path = QCoreApplication::applicationDirPath() + "/Cloud";
    QFile file(path + "/Text.txt");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        ui->textEdit->blockSignals(true);
        ui->textEdit->setPlainText(in.readAll());
        ui->textEdit->blockSignals(false);

        file.close();
    }
}


void MainWindow::loadHtml()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/Cloud/Pocket.html";
    QFile file(filePath);

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString html = in.readAll();

        ui->textEdit_2->setHtml(html);

        file.close();
    }
}

void MainWindow::saveHtml()
{
    QString html = ui->textEdit_2->toHtml();

    QString filePath = QCoreApplication::applicationDirPath() + "/Cloud/Pocket.html";
    QFile file(filePath);

    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out << html;
        file.close();
    }
}


//_______________MAIN WINDOW____________________

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //_____________Scaling for Laptop___________________

    QSize screen = QGuiApplication::primaryScreen()->size();

    if(screen.width() <= 1366)
    {
        resize(1100,630);

        ui->pushButton_28->move(180, 550);
        ui->pushButton_27->move(480, 550);

        ui->textEdit_2->resize(721, 430);
        ui->textEdit->resize(721, 490);

        ui->pushButton_23->move(610, 250);
        ui->pushButton_24->move(420, 250);
        ui->pushButton_22->move(230, 250);
        ui->pushButton_25->move(40, 250);

        ui->tableView->resize(770, 191);

    }

    //________________________________


    setWindowIcon(QIcon(":/files/logo.ico"));
    this->setWindowTitle("Stratosphere");
    this->setFixedSize(this->size());

    nodeProcess = new QProcess(this);

    TableViewSet(uploads);
    TrayIconSet();
    loadData();

    ui->label_2->setText("Inactive");
    ui->label_3->setText("");
    ui->stackedWidget->setCurrentWidget(ui->page_7);

    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::saveDataRealtime);
}




//____________Buttons________________

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->widget(0));
    ui->pushButton_6->setChecked(1);
    ui->pushButton_7->setChecked(0);
    ui->pushButton_8->setChecked(0);
}
void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->widget(1));
    ui->pushButton_6->setChecked(0);
    ui->pushButton_8->setChecked(0);
    ui->pushButton_7->setChecked(1);

    loadData();
}


void MainWindow::on_pushButton_8_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->widget(2));
    ui->pushButton_6->setChecked(0);
    ui->pushButton_7->setChecked(0);
    ui->pushButton_8->setChecked(1);

    loadHtml();
}




//________________MAIN___________________


//------FOLDER--------
void MainWindow::on_pushButton_25_clicked()
{
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(uploads)
        );
}


//------DELETE--------
void MainWindow::on_pushButton_22_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();

    if(index.isValid())
        folderModel->remove(index);

}

//------HIDE--------
void MainWindow::on_pushButton_24_clicked()
{
    hide();
}


//------Browser--------
void MainWindow::on_pushButton_23_clicked()
{
    QString link = ui->label_3->text();

    if (!link.isEmpty())
        QDesktopServices::openUrl(QUrl("http://" + link));
}


//------CLOUD ON/OFF--------
void MainWindow::on_pushButton_26_clicked(bool checked)
{
    if(checked)
    {
        QString path = QCoreApplication::applicationDirPath() + "/Cloud";
        nodeProcess->setWorkingDirectory(path);

        QString nodePath = path + "/node.exe";
        QString serverPath = path + "/server.js";

        nodeProcess->setWorkingDirectory(path);

        nodeProcess->start(nodePath, QStringList() << serverPath);


        ui->label_2->setText("Runing ...");
        QTimer::singleShot(1000, this, [=]()
        {
            ui->label_3->setText(GetIP());
            ui->label_2->setText("Active");
            ui->label_4->setStyleSheet("border-image: url(:/files/GUI/green_ball.png) 0 0 0 0 stretch;");
        });

    }
    else
    {
        nodeProcess->kill();

        ui->label_2->setText("Inactive");
        ui->label_3->setText("");
        ui->label_4->setStyleSheet("border-image: url(:/files/GUI/red_ball.png) 0 0 0 0 stretch;");
    }
}


//------Save Pocket-------
void MainWindow::on_pushButton_27_clicked()
{
    saveHtml();
}


//------Browser Pocket-------
void MainWindow::on_pushButton_28_clicked()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/Cloud/Pocket.html";

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(filePath)
        );
}















/*
 *        else
        {
            QProcess::startDetached(
                "node",
                QStringList() << "server.js",
                path,
                &pid
                );

            PermissionFile("true");
        }
 */


