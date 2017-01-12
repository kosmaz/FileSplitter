#include "Process.hpp"
#include "SegmentHeader.hpp"
#include <QDir>
#include <QTime>
#include <QFile>
#include <QDataStream>

Process::Process(QString path, quint8 operation) :
    fFile_Folder_Path(path),
    fOperation(operation) {}

Process::~Process() {}


void Process::run() {

    if(fOperation == SPLIT)
        startSplitProcess();
    else
        startAssembleProcess();

    return;
}


void Process::startSplitProcess() {

    QFile inputfile(fFile_Folder_Path);
    if(!inputfile.open(QIODevice::ReadOnly)) {

        emit processComplete(OPEN_FILE_ERROR);
        return;
    }

    if(inputfile.size() < 4000000000) {

        emit processComplete(FILE_SIZE_OK);
        return;
    }
    QDataStream read(&inputfile);
    read.setVersion(QDataStream::Qt_5_4);

    QString file_name = "";
    QString folder_path = fFile_Folder_Path;

    for(quint16 i = inputfile.fileName().size() - 1; i > 0; --i) {

        if(folder_path.at(i) == '/' || folder_path.at(i) == '\\')
            break;
        else
            file_name.push_front(QChar(folder_path.at(i)));
    }

    for(quint16 i = folder_path.size() - 1; i > 0; --i) {

        if(folder_path.at(i) == '/' || folder_path.at(i) == '\\')
            break;
        else
            folder_path.remove(i, 1);
    }

    SegmentHeader header;
    header.fFile_Original_Name = file_name;
    header.fFile_Original_Size = (quint64)inputfile.size();
    header.fSegment_Count = header.fFile_Original_Size / SEG_MAX_SIZE;
    for(quint16 i = 0; i < header.fSegment_Count; ++i)
        header.fSegment_Sizes.append(SEG_MAX_SIZE);
    if((SEG_MAX_SIZE * header.fSegment_Count) < header.fFile_Original_Size) {

        header.fSegment_Sizes.append(header.fFile_Original_Size - (SEG_MAX_SIZE * header.fSegment_Count));
        ++header.fSegment_Count;
    }
    for(quint16 i = 0; i < header.fSegment_Count; ++i) {

        qsrand(QTime::currentTime().msec());
        header.fSegment_Matchers.append((quint32)rand());
    }

    QDir dir(folder_path);
    dir.mkdir(folder_path += file_name + "-split/");

    bool success = false;
    SegmentHeader::writeHeader(&header, folder_path, success);
    if(!success) {

        emit processComplete(OPEN_FILE_ERROR);
        return;
    }

    quint64 index = 0;
    quint32 length = 1024000;   //1 MB
    char* buffer = new char[length];
    folder_path += "k-split";

    emit progress(index);

    for(quint16 i = 0; i < header.fSegment_Count; ++i) {

        QFile outputfile(folder_path + ('1' + i) + ".seg");
        if(!outputfile.open(QIODevice::WriteOnly)) {

            emit processComplete(OPEN_FILE_ERROR);
            return;
        }

        QDataStream write(&outputfile);
        write.setVersion(QDataStream::Qt_5_4);
        write<<header.fSegment_Matchers.at(i);

        for(quint32 j = 0; j < header.fSegment_Sizes.at(i); ) {

            write.writeRawData(buffer, read.readRawData(buffer, length));
            index += length;
            emit progress(((double)index / (double)header.fFile_Original_Size) * 100);
            j += length;
        }
        outputfile.close();
    }

    emit processComplete(COMPLETE);
    inputfile.close();
    delete []buffer;

    return;
}


void Process::startAssembleProcess() {

    fFile_Folder_Path += "/";
    bool success = false;
    SegmentHeader* header = SegmentHeader::readHeader(fFile_Folder_Path, success);

    if(!success) {
        emit processComplete(OPEN_FILE_ERROR);
        return;
    }

    QFile outputfile(fFile_Folder_Path + header->fFile_Original_Name);
    if(!outputfile.open(QIODevice::WriteOnly)) {
        emit processComplete(OPEN_FILE_ERROR);
        return;
    }
    QDataStream write(&outputfile);
    write.setVersion(QDataStream::Qt_5_4);

    quint64 index = 0;
    quint32 length = 1024000;   //1 MB
    char* buffer = new char[length];
    QString folder_path = fFile_Folder_Path + "k-split";

    emit progress(index);

    for(quint16 i = 0; i < header->fSegment_Count; ++i) {

        QFile inputfile(folder_path + ('1' + i) + ".seg");
        if(!inputfile.exists()) {
            emit processComplete(MISSING_FILE);
            return;
        }

        if(!inputfile.open(QIODevice::ReadOnly)) {

            emit processComplete(OPEN_FILE_ERROR);
            return;
        }

        QDataStream read(&inputfile);
        read.setVersion(QDataStream::Qt_5_4);

        quint32 matcher;
        read>>matcher;

        if(((inputfile.size() - sizeof(quint32)) != header->fSegment_Sizes.at(i)) || (matcher != header->fSegment_Matchers.at(i))) {
            emit processComplete(WRONG_FILE);
            return;
        }

        for(quint32 j = 0; j < header->fSegment_Sizes.at(i); ) {

            write.writeRawData(buffer, read.readRawData(buffer, length));
            index += length;
            emit progress(((double)index / (double)header->fFile_Original_Size) * 100);
            j += length;
        }
        inputfile.close();
    }

    emit processComplete(COMPLETE);
    outputfile.close();
    delete []buffer;

    return;
}
