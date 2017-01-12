#include <QFile>
#include <QDataStream>
#include "SegmentHeader.hpp"

SegmentHeader::SegmentHeader():
   fFile_Original_Name(""),
   fFile_Original_Size(0),
   fSegment_Count(0)
{}


SegmentHeader::~SegmentHeader(){}


SegmentHeader* SegmentHeader::readHeader(QString path, bool& success) {

    SegmentHeader* new_header = new SegmentHeader;

    QFile header_file(path + "MANIFEST");
    if(!header_file.open(QIODevice::ReadOnly)) {

        success = false;
        return new_header;
    }

    QDataStream read(&header_file);
    read.setVersion(QDataStream::Qt_5_4);

    read>>new_header->fFile_Original_Name;
    read>>new_header->fFile_Original_Size;
    read>>new_header->fSegment_Count;


    for(quint16 i = 0; i < new_header->fSegment_Count; ++i) {
        quint32 temp;
        read>>temp;
        new_header->fSegment_Matchers.append(temp);
    }


    for(quint16 i = 0; i < new_header->fSegment_Count; ++i) {
        quint32 temp;
        read>>temp;
        new_header->fSegment_Sizes.append(temp);
    }
    header_file.close();
    success = true;

    return new_header;
}


void SegmentHeader::writeHeader(SegmentHeader* new_header, QString path, bool& success) {

    QFile header_file(path + "MANIFEST");
    if(!header_file.open(QIODevice::WriteOnly)) {

        success = false;
        return;
    }

    QDataStream write(&header_file);
    write.setVersion(QDataStream::Qt_5_4);

    write<<new_header->fFile_Original_Name;
    write<<new_header->fFile_Original_Size;
    write<<new_header->fSegment_Count;


    for(quint16 i = 0; i < new_header->fSegment_Count; ++i)
        write<<new_header->fSegment_Matchers.at(i);


    for(quint16 i = 0; i < new_header->fSegment_Count; ++i)
        write<<new_header->fSegment_Sizes.at(i);
    header_file.close();
    success = true;

    return;
}

