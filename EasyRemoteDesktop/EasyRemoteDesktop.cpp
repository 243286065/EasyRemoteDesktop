#include "EasyRemoteDesktop.h"

#include "ServerMainWindow.h"

#include <QIcon>

EasyRemoteDesktop::EasyRemoteDesktop(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setFixedSize(this->width(), this->height());    //禁止动态调整窗口大小
    setWindowIcon(QIcon("Resources/desktop.ico"));  //设置图标
    setWindowTitle(QStringLiteral("远程桌面"));     //设置标题

    ui.tabWidget->setTabText(0, QStringLiteral("发送端设置"));
    ui.tabWidget->setTabText(1, QStringLiteral("接收端设置"));

    //发送端和接收端分离
    m_pServerWindow.reset(new ServerMainWindow(&ui));

}
