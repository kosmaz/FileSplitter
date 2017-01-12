#ifndef FILESPLITTER_HPP
#define FILESPLITTER_HPP

#include <QObject>
#include "Process.hpp"


class FileSplitter : public QObject {

    Q_OBJECT
public:
    explicit FileSplitter(QString, quint8, QObject* parent = 0);
    ~FileSplitter();
    void startProcess();


signals:
    void clear();
    void progress(int);
    void processComplete(quint8);

public slots:
    void rollBackChanges();
    void progressRelay(int);
    void processCompleteRelay(quint8);
    void interruptCurrentProcess();

private:
    FileSplitter(const FileSplitter&) = delete;
    FileSplitter(FileSplitter&&) = delete;
    FileSplitter& operator=(const FileSplitter&) = delete;
    FileSplitter& operator=(FileSplitter&&) = delete;


    Process* fProcess;
    QString fFile_Folder_Path;
    quint8 fOperation;
};

#endif // FILESPLITTER_HPP
