#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QProgressBar;
class FileSplitter;


class Widget : public QWidget {

    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void filePathChanged(QString);
    void interruptCurrentProcess();

public slots:
    void processComplete(quint8);

private slots:
    void clear();
    void splitFile();
    void startSplitProcess();

    void assembleFile();
    void startAssembleProcess();

    void openFileDialog();
    void openFolderDialog();
    void cancelCurrentProcess();

private:
    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;


    QWidget* fCurrent_Window;
    QLabel* fCurrent_Process_Status;
    QLineEdit* fCurrent_Line_Edit;
    QProgressBar* fCurrent_Progress_Bar;
    QPushButton* fCurrent_Start_Button;
    QPushButton* fCurrent_Browse_Button;
    QPushButton* fCurrent_Cancel_Button;

    FileSplitter* fSplit_Engine;
    QString fFile_Folder_Path;

};

#endif // WIDGET_HPP
