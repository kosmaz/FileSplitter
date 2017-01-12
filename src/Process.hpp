#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <QThread>

#define SPLIT 0x01
#define ASSEMBLE 0x02

#define SEG_MAX_SIZE 3072000000

#define COMPLETE 0x00
#define WRONG_FILE 0x01
#define MISSING_FILE 0x02
#define FILE_SIZE_OK 0x03
#define OPEN_FILE_ERROR 0x04



class Process : public QThread {

    Q_OBJECT
public:
    explicit Process(QString, quint8);
    ~Process();
    void run() Q_DECL_OVERRIDE;

signals:
    void progress(int);
    void processComplete(quint8);

private:
    Process(const Process&) = delete;
    Process(Process&&) = delete;
    Process& operator=(const Process&) = delete;
    Process& operator=(Process&&) = delete;


    void startSplitProcess();
    void startAssembleProcess();

    QString fFile_Folder_Path;
    quint8 fOperation;
};

#endif // PROCESS_HPP
