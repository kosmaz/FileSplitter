#include "FileSplitter.hpp"
#include <QDir>

FileSplitter::FileSplitter(QString path, quint8 operation, QObject* parent) :
    QObject(parent),
    fProcess(nullptr),
    fFile_Folder_Path(path),
    fOperation(operation) {

}

FileSplitter::~FileSplitter() {}


void FileSplitter::startProcess() {

    fProcess = new Process(fFile_Folder_Path, fOperation);
    connect(fProcess, SIGNAL(finished()), fProcess, SLOT(deleteLater()));
    connect(fProcess, SIGNAL(progress(int)), this, SLOT(progressRelay(int)));
    connect(fProcess, SIGNAL(processComplete(quint8)), this, SLOT(processCompleteRelay(quint8)));
    fProcess->start();
    return;
}


void FileSplitter::rollBackChanges() {

    switch(fOperation) {

        case SPLIT: {

            QDir dir(fFile_Folder_Path + "-split/");
            (void)dir.removeRecursively();
            break;
        }

        case ASSEMBLE: {

            QDir dir(fFile_Folder_Path);
            QString filename;

            for(quint16 i = fFile_Folder_Path.size() - 1; i > 0; --i) {

                if(fFile_Folder_Path.at(i) == '/' || fFile_Folder_Path.at(i) == '\\')
                    break;
                else
                    filename.push_front(QChar(fFile_Folder_Path.at(i)));
            }

            for(quint16 i = filename.size() - 1; i > 0; --i) {

                if(filename.at(i) == '-') {
                    filename.remove(i, 1);
                    break;
                }
                else
                    filename.remove(i, 1);
            }
            dir.remove(filename);
            break;
        }
    }
    emit clear();

    return;
}



void FileSplitter::progressRelay(int relay) {

    emit progress(relay);
    return;
}


void FileSplitter::processCompleteRelay(quint8 relay) {

    emit processComplete(relay);
    return;
}


void FileSplitter::interruptCurrentProcess() {

    fProcess->terminate();
    fProcess->wait();
    delete fProcess;
    fProcess = nullptr;
    return;
}
