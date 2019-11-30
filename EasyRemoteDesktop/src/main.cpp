#include "EasyRemoteDesktop.h"
#include <QtWidgets/QApplication>

#include "glog/logging.h"
#pragma comment(lib, "glog.lib")

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, "log_");
    QApplication a(argc, argv);
    EasyRemoteDesktop w;
    w.show();
    LOG(INFO) << "Start running...";
    return a.exec();
}
