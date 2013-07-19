#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_actionNouveau_triggered();
    loadRecentFiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNouveau_triggered()
{
    QTextEdit* textEdit=new QTextEdit();
    int index=ui->tabWidget->addTab(textEdit,"nouveau");
    ui->tabWidget->setCurrentIndex(index);
    textEdits[index]=textEdit;
}

void MainWindow::saveFileIfAsked()
{
    if(currentTextEdit()!=NULL && currentTextEdit()->document()->isModified())
    {
        if(QMessageBox::question(this,"Sauvegarder ?","Voulez vous sauvegarder le document "+reducedFileName(currentTextEdit()->windowFilePath())+" ?",
                                 QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
        {
            on_actionEnregistrer_triggered();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveAllFilesIfAsked();
    event->accept();
}

QString MainWindow::reducedFileName(QString fileName)
{
    QStringList l=fileName.split("/");
    if(l.length()>5) return "/"+l[1]+"/"+l[2]+"/.../"+l[l.length()-2]+"/"+l[l.length()-1];
    else return fileName;
}

QString MainWindow::veryReducedFileName(QString fileName)
{
    QStringList l=fileName.split("/");
    return l[l.length()-1];
}

QTextEdit* MainWindow::currentTextEdit()
{
   return qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
}

void MainWindow::saveFile()
{
    QFile file(currentTextEdit()->windowFilePath());
    file.open(QIODevice::WriteOnly);
    QTextStream textStream(&file);
    textStream<<currentTextEdit()->toPlainText();
    currentTextEdit()->document()->setModified(false);
    addRecentFile();
}



void MainWindow::on_actionEnregistrer_sous_triggered()
{
    QString t_fileName=QFileDialog::getSaveFileName(this,"Sauvegarder le fichier sous");
    if(t_fileName=="") return;
    currentTextEdit()->setWindowFilePath(t_fileName);
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),veryReducedFileName(t_fileName));
    saveFile();
}

void MainWindow::on_actionEnregistrer_triggered()
{
    if(currentTextEdit()->windowFilePath()=="") on_actionEnregistrer_sous_triggered();
    else saveFile();
}

void MainWindow::on_actionFermer_triggered()
{
    saveFileIfAsked();
    textEdits.remove(ui->tabWidget->currentIndex());
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::saveAllFilesIfAsked()
{
    for(QMap<int, QTextEdit*>::const_iterator i = textEdits.constBegin();i != textEdits.constEnd();++i)
    {
        ui->tabWidget->setCurrentIndex(i.key());
        saveFileIfAsked();
    }
}

void MainWindow::on_actionQuitter_triggered()
{
   close();
}

void MainWindow::loadRecentFiles()
{
    settings=new QSettings("SimpleEditor","SimpleEditor");
    for (int i = 1; i <= settings->allKeys().size(); ++i)
    {
        addRecentFileAction(settings->value(QString::number(i)).toString());
    }
}

void MainWindow::addRecentFileAction(QString fileName)
{
    QAction *newRecentFile=ui->menuDocuments_recents->addAction(reducedFileName(fileName));
    newRecentFile->setData(QVariant(fileName));
    connect(newRecentFile,SIGNAL(triggered()),this,SLOT(openRecentFile()));
}

void MainWindow::addRecentFileActionFirst(QString fileName)
{
    QList<QAction *> la=ui->menuDocuments_recents->actions();
    QAction *newRecentFile=new QAction(reducedFileName(fileName),this);
    newRecentFile->setData(QVariant(fileName));
    if(la.length()>0) ui->menuDocuments_recents->insertAction(la[0],newRecentFile);
    else ui->menuDocuments_recents->addAction(newRecentFile);
    connect(newRecentFile,SIGNAL(triggered()),this,SLOT(openRecentFile()));
}

void MainWindow::addRecentFile()
{

    for(QStringListIterator i(settings->allKeys());i.hasNext();) if(currentTextEdit()->windowFilePath()==settings->value(i.next())) return;

    addRecentFileActionFirst(currentTextEdit()->windowFilePath());
    if(settings->allKeys().length()==5)
    {
        QList<QAction *> la=ui->menuDocuments_recents->actions();
        ui->menuDocuments_recents->removeAction(la[5]);
    }

    QStringList l;
    for(QStringListIterator i(settings->allKeys());i.hasNext();) l<<settings->value(i.next()).toString();
    for (int i = 1; i <= l.size(); ++i)
    {
        if(i<5) settings->setValue(QString::number(i+1),l[i-1]);
    }
    settings->setValue("1",currentTextEdit()->windowFilePath());
}

void MainWindow::openRecentFile()
{
    const QAction * action = qobject_cast<const QAction *>(this->sender());
    openFile(action->data().value<QString>());
}

void MainWindow::openFile(QString fileName)
{
    for(QMap<int, QTextEdit*>::const_iterator i = textEdits.constBegin();i != textEdits.constEnd();++i)
    {
        if(i.value()->windowFilePath()==fileName)
        {
            ui->tabWidget->setCurrentIndex(i.key());
            return;
        }
    }
    if(currentTextEdit() && currentTextEdit()->windowFilePath()=="" && !(currentTextEdit()->document()->isModified()) && ui->tabWidget->count()==1) on_actionFermer_triggered();
    QTextEdit* textEdit=new QTextEdit();
    textEdit->setWindowFilePath(fileName);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream textStream(&file);
    QString content="";
    content=textStream.readAll();
    textEdit->setPlainText(content);
    textEdit->document()->setModified(false);
    int index=ui->tabWidget->addTab(textEdit,veryReducedFileName(fileName));
    textEdits[index]=textEdit;
    ui->tabWidget->setCurrentIndex(index);
    addRecentFile();
}

void MainWindow::on_actionOuvrir_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(this,"Ouvrir le fichier");
    if(fileName=="") return;
    openFile(fileName);
}

void MainWindow::on_actionCouper_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->cut();
}

void MainWindow::on_actionCopier_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->copy();
}

void MainWindow::on_actionColler_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->paste();
}

void MainWindow::on_actionSelectionner_tout_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->selectAll();
}

void MainWindow::on_actionRefaire_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->redo();
}

void MainWindow::on_actionDefaire_triggered()
{
    if(currentTextEdit()!=NULL) currentTextEdit()->undo();
}
