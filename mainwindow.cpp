#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QListView>
#include <QTextStream>



#include "mainwindow.h"
#include "ui_mainwindow.h"


#define STR_LEN 512

QString projectFileName;
QString lastSimulationType;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//==================================================
void MainWindow::init(){
    ui->projectList->clear();
    MainWindow::refreshListe();
}
//==================================================
//  Function: refreshListe()
//      refresh the list of task to follow the tracker
//==================================================
void MainWindow::refreshListe()
{
QString myCmd ;
QString myMessage ;
QStringList lines;
QFile file( "pjtList.txt" );
QString line;

ui->projectList->clear();

if (ui->teamRadio->isChecked()){
    myCmd = "ls $QUCS_REPOSITORY | grep _prj > pjtList.txt";
    ui->projectLocation->setText("$QUCS_REPOSITORY");
    }
else {
    myCmd = "ls ";
    myCmd += ui->myPath->text();
    myCmd += " | grep _prj > pjtList.txt";
    ui->projectLocation->setText(ui->myPath->text());
    }

system(myCmd.toLocal8Bit().data());

if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream myStream( &file );
        while ( !myStream.atEnd() ) {
            line = myStream.readLine(); // line of text excluding '\n'
            //fprintf(stdout, "%3d: %s\n", i++, line.latin1() );
            ui->projectList->insertItem(-1,line);
            lines += line;
        }
        file.close();
    }
ui->projectList->setCurrentItem(0);
}
//==================================================
void MainWindow::on_quitBtn_clicked()
{
    QString myMessage;
    myMessage = " vers: 3.0\n";
    myMessage += " build 2014.11.12 dvlpt \n";
    myMessage += " by Tss (2013-14), thanks";
    QMessageBox::information(this,"Qucs Mamager",myMessage );

    system("rm -rf pjtList.txt");
    close();
}
//==================================================
void MainWindow::on_teamRadio_toggled(bool checked)
{
    if (checked){}; // to remove warning during compilation
    MainWindow::refreshListe();
}

void MainWindow::on_myPath_textChanged(const QString &arg1)
{QString temp;
    temp = arg1; // to remove marning during compilation
    ui->teamRadio->setChecked(false);
    ui->localLocationRadio->setChecked(true);
    MainWindow::refreshListe();
}



/*-------------------------------------------------------------------------------
  Function: whoLockFile()
      check weither or not the fiolder is locked and if yes by who
-----------------------------------------------------------------------------------*/

QString MainWindow::whoLockFile(QString folderName){
QString fileName;
QString qUser;
char userName[255];
QString fullName;

fullName = folderName + "/lock";
FILE	*fid;

fid = fopen(fullName.toLocal8Bit().data(),"r+t");

if ( fid == NULL ){
    qUser="free";
    }
else {
    fscanf(fid,"%s",&userName[0]);
    fclose(fid);
    qUser = QString(userName);
    }
return qUser;
}

/*-------------------------------------------------------------------------------
  Function: evalSysVar()
      evaluate a variable system
-----------------------------------------------------------------------------------*/

QString MainWindow::evalSysVar(QString variable){
QString myCmd;
QString qValeur;
FILE *fid;
char valeur[255];

myCmd = "echo " + variable + " > eval";
system(myCmd.toLocal8Bit().data());
fid=fopen("eval","r+t");
fscanf(fid,"%s",&valeur[0]);
fclose(fid);
system("rm -rf eval");
qValeur = QString(valeur);
return qValeur;
}

/*-------------------------------------------------------------------------------
  Function: lockFile()
      evaluate a variable system
-----------------------------------------------------------------------------------*/
void MainWindow::lockFile(QString pathName){
QString myCmd;

myCmd = "echo $USER > ";
myCmd += pathName;
myCmd += "/lock";
system(myCmd.toLocal8Bit().data());
}
/*-------------------------------------------------------------------------------
  Function: unLockFile()
      evaluate a variable system
-----------------------------------------------------------------------------------*/

void  MainWindow::unLockFile(QString pathName){
QString myCmd;

myCmd = "rm -rf ";
myCmd += pathName;
myCmd += "/lock";
system(myCmd.toLocal8Bit().data());
}

void MainWindow::on_goBtn_clicked()
{
    QString myCmd ;
    QString currentUser;
    QString currentProject;
    QString fullNamePath;
    QListWidgetItem *currentItem;

    //========================= delete the working link
    myCmd = "rm -rf $HOME/.qucs";
    system(myCmd.toLocal8Bit().data());
 //QMessageBox::information(this,"Qucs Mamager",".qucs removed" );
    //========================= remap .qucs to the needed folder
    if (ui->teamRadio->isChecked() == true)
        myCmd = "ln -s $QUCS_REPOSITORY";
    else {
        myCmd = "ln -s ";
        myCmd += ui->myPath->text();
        }
    myCmd+=" $HOME/.qucs";
    system(myCmd.toLocal8Bit().data());
    //======================== select the correct authorization on the requested project
    currentProject = "empty";
    currentItem = ui->projectList->currentItem();
    currentProject = currentItem->text();
 //QMessageBox::information(this,"Selected project for RW access : ",currentProject );


    fullNamePath += " $HOME/.qucs/";
    fullNamePath += currentProject;
    fullNamePath = MainWindow::evalSysVar(fullNamePath);

    currentUser = MainWindow::whoLockFile(fullNamePath );

    if (currentUser == "free") { //========== project is FREE
        myCmd = "chmod +w ";
        myCmd += fullNamePath ;
        system(myCmd.toLocal8Bit().data());

        QMessageBox::information(this,"WARNING : ", "set RW access to : " + currentProject );
        //========================= launch qucs
        MainWindow::lockFile(fullNamePath);
        myCmd = "qucs";
        system(myCmd.toLocal8Bit().data());

        //========================= restore authorization
        MainWindow::unLockFile(fullNamePath);
        myCmd = "chmod -w ";
        myCmd += fullNamePath;
        system(myCmd.toLocal8Bit().data());
        }
    else { //=============================== project is LOCKED
        QMessageBox::information(this,"IMPOSSIBLE "," already locked by : " + currentUser );
        }
}

void MainWindow::on_projectList_doubleClicked(const QModelIndex &index)
{QModelIndex temp;
    temp = index; // to remove warning for compilation
    MainWindow::on_goBtn_clicked();
}
