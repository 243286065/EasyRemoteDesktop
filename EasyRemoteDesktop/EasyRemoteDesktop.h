#pragma once

#include <memory>

#include <QtWidgets/QWidget>
#include "ui_EasyRemoteDesktop.h"
#include "ServerMainWindow.h"

class EasyRemoteDesktop : public QWidget
{
    Q_OBJECT

public:
    EasyRemoteDesktop(QWidget *parent = Q_NULLPTR);

private:
    Ui::EasyRemoteDesktopClass ui;

    std::unique_ptr<ServerMainWindow> m_pServerWindow;

};
