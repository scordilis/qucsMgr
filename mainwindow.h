#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //============== utility fucntions
    void init();

    void refreshListe();

    QString whoLockFile(QString folderName);

    QString evalSysVar(QString variable);

    void lockFile(QString pathName);

    void unLockFile(QString pathName);

    //============== UI related functions

    void on_quitBtn_clicked();

    void on_teamRadio_toggled(bool checked);

    void on_myPath_textChanged(const QString &arg1);

    void on_goBtn_clicked();

    void on_projectList_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;



};

#endif // MAINWINDOW_H
