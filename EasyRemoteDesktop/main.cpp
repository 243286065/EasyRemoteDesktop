#include "EasyRemoteDesktop.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EasyRemoteDesktop w;
    w.show();
    return a.exec();
}
