#ifndef SEGMENTHEADER_HPP
#define SEGMENTHEADER_HPP

#include <QtGlobal>
#include <QVector>

class QString;
class QFile;

class SegmentHeader {

public:
    SegmentHeader();
    ~SegmentHeader();

    static SegmentHeader* readHeader(QString, bool&);
    static void writeHeader(SegmentHeader*, QString, bool&);

    QString fFile_Original_Name;
    quint64 fFile_Original_Size;

    quint16 fSegment_Count;
    QVector<quint32> fSegment_Matchers;
    QVector<quint32> fSegment_Sizes;

private:
    SegmentHeader(const SegmentHeader&) = delete;
    SegmentHeader(SegmentHeader&&) = delete;
    SegmentHeader& operator=(const SegmentHeader&) = delete;
    SegmentHeader& operator=(SegmentHeader&&) = delete;
};

#endif // SEGMENTHEADER_HPP
