#include "Widget.hpp"
#include "FileSplitter.hpp"

#include <QLabel>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    fCurrent_Window(nullptr),
    fCurrent_Process_Status(nullptr),
    fCurrent_Line_Edit(nullptr),
    fCurrent_Progress_Bar(nullptr),
    fCurrent_Start_Button(nullptr),
    fCurrent_Browse_Button(nullptr),
    fCurrent_Cancel_Button(nullptr),
    fSplit_Engine(nullptr),
    fFile_Folder_Path("")
{


    setFixedSize(QSize(250, 200));
    setWindowIcon(QIcon(QPixmap::fromImage(QImage(":/images/split.png"))));
    setWindowTitle("FAT32 FILE SPLITTER");

    QPushButton* split_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/split.png"))), tr("SPLIT A FILE"));
    split_button->setFixedSize(QSize(150, 30));
    connect(split_button, SIGNAL(clicked()), this, SLOT(splitFile()));
    QPushButton* assemble_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/assemble.jpg"))), tr("ASSEMBLE A FILE"));
    assemble_button->setFixedSize(QSize(150, 30));
    connect(assemble_button, SIGNAL(clicked()), this, SLOT(assembleFile()));
    QPushButton* quit_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/quit.png"))), tr("QUIT"));
    quit_button->setFixedSize(QSize(150, 30));
    connect(quit_button, SIGNAL(clicked()), this, SLOT(close()));

    if(this->layout())
        delete this->layout();

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addWidget(split_button, 0, Qt::AlignCenter);
    vlayout->addWidget(assemble_button, 0, Qt::AlignCenter);
    vlayout->addWidget(quit_button, 0, Qt::AlignCenter);

}


Widget::~Widget() {}


void Widget::processComplete(quint8 code) {

    switch(code)
    {
        case COMPLETE: {
            QMessageBox::information(fCurrent_Window, tr("100% COMPLETE"), tr("The Process completed successfully"));
            delete fCurrent_Window;
            delete fSplit_Engine;
        break;
        }

        case WRONG_FILE: {
            QMessageBox::critical(fCurrent_Window, tr("WRONG FILE"), tr("Some segment file(s) needed to perform the file assembling"
                                                                        " operation have been tampered with or corrupted"));
            connect(fSplit_Engine, SIGNAL(clear()), this, SLOT(clear()));
            fCurrent_Process_Status->setText(tr("Rolling back changes..."));
            QTimer::singleShot(1, fSplit_Engine, SLOT(rollBackChanges()));
        break;
        }

        case MISSING_FILE: {
            QMessageBox::critical(fCurrent_Window, tr("MISSING FILES"), tr("Unable to locate some segment file(s needed to perform"
                                                                           " the file assembling operation"));
            connect(fSplit_Engine, SIGNAL(clear()), this, SLOT(clear()));
            fCurrent_Process_Status->setText(tr("Rolling back changes..."));
            QTimer::singleShot(1, fSplit_Engine, SLOT(rollBackChanges()));
        break;
        }

        case FILE_SIZE_OK: {
            QMessageBox::information(fCurrent_Window, tr("FILE SIZE LESS THAN 4GB"), tr("The size of the selected file can "
                                                                                "be transfered to a FAT32 file system. Select a file "
                                                                                "that has a size greater than 4GB for splitting"));
            delete fCurrent_Window;
            delete fSplit_Engine;
        break;
        }

        case OPEN_FILE_ERROR: {
            QMessageBox::critical(fCurrent_Window, tr("FILE OPERATION ERROR"), tr("Unable to open some files for read or"
                                                                                  " write operations"));
            connect(fSplit_Engine, SIGNAL(clear()), this, SLOT(clear()));
            fCurrent_Process_Status->setText(tr("Rolling back changes..."));
            QTimer::singleShot(1, fSplit_Engine, SLOT(rollBackChanges()));
        break;
        }
    }

    return;
}


void Widget::clear() {

    delete fCurrent_Window;
    delete fSplit_Engine;
    return;
}


void Widget::splitFile() {

    QWidget* split_window = new QWidget;
    split_window->setWindowIcon(QIcon(QPixmap::fromImage(QImage(":/images/split.png"))));
    split_window->setWindowModality(Qt::ApplicationModal);
    split_window->setWindowTitle("SPLIT A FILE");
    split_window->setFixedSize(QSize(400, 200));

    QHBoxLayout* file_path_layout = new QHBoxLayout;
    QHBoxLayout* button_layout = new QHBoxLayout;
    QVBoxLayout* central_layout = new QVBoxLayout;

    QPushButton* start_button = new QPushButton(QIcon(":/images/start.ico"), tr("Start"), split_window);
    start_button->setFixedSize(QSize(60, 30));
    connect(start_button, SIGNAL(clicked()), this, SLOT(startSplitProcess()));
    QPushButton* cancel_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/quit.png"))), tr("Cancel"), split_window);
    cancel_button->setFixedSize(QSize(70, 30));
    cancel_button->setDisabled(true);
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancelCurrentProcess()));
    QPushButton* browse_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/browse.png"))), tr("Browse"), split_window);
    browse_button->setFixedSize(QSize(75, 30));
    connect(browse_button, SIGNAL(clicked()), this, SLOT(openFileDialog()));

    QLabel* file_path_label = new QLabel(tr("File path:"),split_window);
    QLineEdit* file_path = new QLineEdit(split_window);
    file_path->setFixedWidth(split_window->width() - (file_path_label->width() + browse_button->width()));
    connect(this, SIGNAL(filePathChanged(QString)), file_path, SLOT(setText(QString)));

    file_path_layout->addWidget(file_path_label, 2, Qt::AlignLeft);
    file_path_layout->addWidget(file_path, 5, Qt::AlignCenter);
    file_path_layout->addWidget(browse_button, 1, Qt::AlignRight);

    button_layout->addSpacing(split_window->width() - (start_button->width() + cancel_button->width() +20));
    button_layout->addWidget(start_button);
    button_layout->addWidget(cancel_button);

    central_layout->addLayout(file_path_layout);
    central_layout->addLayout(button_layout);

    QProgressBar* progress = new QProgressBar(split_window);
    progress->setRange(0, 100);
    progress->hide();

    QLabel* status = new QLabel(tr("Splitting file..."), split_window);
    status->hide();

    central_layout->addSpacing(20);
    central_layout->addWidget(status, 0, Qt::AlignLeft);
    central_layout->addWidget(progress);

    if(split_window->layout())
        delete split_window->layout();
    split_window->setLayout(central_layout);
    split_window->show();
    fCurrent_Window = split_window;
    fCurrent_Start_Button = start_button;
    fCurrent_Browse_Button = browse_button;
    fCurrent_Cancel_Button = cancel_button;
    fCurrent_Process_Status = status;
    fCurrent_Progress_Bar = progress;
    fCurrent_Line_Edit = file_path;

    return;
}


void Widget::startSplitProcess() {

    if(fFile_Folder_Path.size()) {

        fSplit_Engine = new FileSplitter(fFile_Folder_Path, SPLIT);
        connect(this, SIGNAL(interruptCurrentProcess()), fSplit_Engine, SLOT(interruptCurrentProcess()));
        connect(fSplit_Engine, SIGNAL(progress(int)), fCurrent_Progress_Bar, SLOT(setValue(int)));
        connect(fSplit_Engine, SIGNAL(processComplete(quint8)), this, SLOT(processComplete(quint8)));
        fCurrent_Progress_Bar->show();
        fCurrent_Process_Status->show();
        fCurrent_Line_Edit->setDisabled(true);
        fCurrent_Start_Button->setDisabled(true);
        fCurrent_Browse_Button->setDisabled(true);
        fCurrent_Cancel_Button->setEnabled(true);
        fSplit_Engine->startProcess();
    }

    return;
}


void Widget::assembleFile() {

    QWidget* split_window = new QWidget;
    split_window->setWindowIcon(QIcon(QPixmap::fromImage(QImage(":/images/assemble.jpg"))));
    split_window->setWindowModality(Qt::ApplicationModal);
    split_window->setWindowTitle("ASSEMBLE A FILE");
    split_window->setFixedSize(QSize(400, 200));

    QHBoxLayout* file_path_layout = new QHBoxLayout;
    QHBoxLayout* button_layout = new QHBoxLayout;
    QVBoxLayout* central_layout = new QVBoxLayout;

    QPushButton* start_button = new QPushButton(QIcon(":/images/start.ico"), tr("Start"), split_window);
    start_button->setFixedSize(QSize(60, 30));
    connect(start_button, SIGNAL(clicked()), this, SLOT(startAssembleProcess()));
    QPushButton* cancel_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/quit.png"))), tr("Cancel"), split_window);
    cancel_button->setFixedSize(QSize(70, 30));
    cancel_button->setDisabled(true);
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancelCurrentProcess()));
    QPushButton* browse_button = new QPushButton(QIcon(QPixmap::fromImage(QImage(":/images/browse.png"))), tr("Browse"), split_window);
    browse_button->setFixedSize(QSize(75, 30));
    connect(browse_button, SIGNAL(clicked()), this, SLOT(openFolderDialog()));

    QLabel* file_path_label = new QLabel(tr("File path:"),split_window);
    QLineEdit* file_path = new QLineEdit(split_window);
    file_path->setFixedWidth(split_window->width() - (file_path_label->width() + browse_button->width()));
    connect(this, SIGNAL(filePathChanged(QString)), file_path, SLOT(setText(QString)));

    file_path_layout->addWidget(file_path_label, 2, Qt::AlignLeft);
    file_path_layout->addWidget(file_path, 5, Qt::AlignCenter);
    file_path_layout->addWidget(browse_button, 1, Qt::AlignRight);

    button_layout->addSpacing(split_window->width() - (start_button->width() + cancel_button->width() +20));
    button_layout->addWidget(start_button);
    button_layout->addWidget(cancel_button);

    central_layout->addLayout(file_path_layout);
    central_layout->addLayout(button_layout);

    QProgressBar* progress = new QProgressBar(split_window);
    progress->setRange(0, 100);
    progress->hide();

    QLabel* status = new QLabel(tr("Assembling file..."), split_window);
    status->hide();

    central_layout->addSpacing(20);
    central_layout->addWidget(status, 0, Qt::AlignLeft);
    central_layout->addWidget(progress);

    if(split_window->layout())
        delete split_window->layout();
    split_window->setLayout(central_layout);
    split_window->show();
    fCurrent_Window = split_window;
    fCurrent_Start_Button = start_button;
    fCurrent_Browse_Button = browse_button;
    fCurrent_Cancel_Button = cancel_button;
    fCurrent_Process_Status = status;
    fCurrent_Progress_Bar = progress;
    fCurrent_Line_Edit = file_path;

    return;
}


void Widget::startAssembleProcess() {

    if(fFile_Folder_Path.size()) {

        fSplit_Engine = new FileSplitter(fFile_Folder_Path, ASSEMBLE);
        connect(this, SIGNAL(interruptCurrentProcess()), fSplit_Engine, SLOT(interruptCurrentProcess()));
        connect(fSplit_Engine, SIGNAL(progress(int)), fCurrent_Progress_Bar, SLOT(setValue(int)));
        connect(fSplit_Engine, SIGNAL(processComplete(quint8)), this, SLOT(processComplete(quint8)));
        fCurrent_Progress_Bar->show();
        fCurrent_Process_Status->show();
        fCurrent_Line_Edit->setDisabled(true);
        fCurrent_Start_Button->setDisabled(true);
        fCurrent_Browse_Button->setDisabled(true);
        fCurrent_Cancel_Button->setEnabled(true);
        fSplit_Engine->startProcess();
    }
    return;
}


void Widget::openFileDialog() {

    fFile_Folder_Path = QFileDialog::getOpenFileName(fCurrent_Window, tr("Split a file"));
    emit filePathChanged(fFile_Folder_Path);
    return;
}


void Widget::openFolderDialog() {

    fFile_Folder_Path = QFileDialog::getExistingDirectory(fCurrent_Window, tr("Assemble a file"));
    emit filePathChanged(fFile_Folder_Path);
    return;
}


void Widget::cancelCurrentProcess() {

    int choice = QMessageBox::question(fCurrent_Window, tr("Cancel operation"),
                                       tr("Are you sure you want to cancel the present operation?"));
    if(choice == QMessageBox::Yes) {
        connect(fSplit_Engine, SIGNAL(clear()), this, SLOT(clear()));
        emit interruptCurrentProcess();
        fCurrent_Process_Status->setText(tr("Rolling back changes..."));
        QTimer::singleShot(1, fSplit_Engine, SLOT(rollBackChanges()));
    }
    return;
}
