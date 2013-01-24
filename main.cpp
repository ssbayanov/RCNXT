#include <QtGui>
#include <QTextCodec>
#include "rcform.h"



int main(int argc, char *argv[])
{
    QResource::registerResource("rc.rcc");
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);


    rcForm w;
    w.show();

    return a.exec();
}
