#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QIODevice>
#include <QTextStream>
#include <QSettings>
#include <iostream>
#include <QCloseEvent>

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
    void on_actionNouveau_triggered();
    void on_actionEnregistrer_sous_triggered();
    void on_actionEnregistrer_triggered();
    void on_actionFermer_triggered();
    void on_actionQuitter_triggered();
    void on_actionOuvrir_triggered();
    void openRecentFile();


    void on_actionCouper_triggered();

    void on_actionCopier_triggered();

    void on_actionColler_triggered();

    void on_actionSelectionner_tout_triggered();

    void on_actionRefaire_triggered();

    void on_actionDefaire_triggered();

private:
    Ui::MainWindow *ui;
    QAction *recentFiles[5];
    QSettings *settings;

    void saveFile();
    void saveFileIfAsked();
    void saveAllFilesIfAsked();
    void loadRecentFiles();
    void addRecentFile();
    void addRecentFileAction(QString fileName);
    void addRecentFileActionFirst(QString fileName);
    void openFile(QString fileName);
    QTextEdit* currentTextEdit();
    QString reducedFileName(QString fileName);
    QString veryReducedFileName(QString fileName);
    QMap<int,QTextEdit*> textEdits;
    void closeEvent(QCloseEvent * event);
};

#endif // MAINWINDOW_H
