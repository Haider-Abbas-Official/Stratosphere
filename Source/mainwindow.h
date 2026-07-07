#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QSystemTrayIcon>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void TableViewSet(QString path);
    void TrayIconSet();
    void saveDataRealtime();
    void loadData();
    void loadHtml();
    void saveHtml();

private slots:
    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_26_clicked(bool checked);

    void on_pushButton_8_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_28_clicked();


private:
    Ui::MainWindow *ui;

    QFileSystemModel *folderModel;
    QSystemTrayIcon *trayIcon;
    QProcess *nodeProcess;
};
#endif // MAINWINDOW_H
