#include "weblookupdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebLookupDialog w;
    w.show();
    return a.exec();
}
