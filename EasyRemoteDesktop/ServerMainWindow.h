#pragma once
#include <qobject.h>

#include "ui_EasyRemoteDesktop.h"

class ServerMainWindow :
    public QObject
{
    Q_OBJECT

private slots:
    void onPreviewBtnClicked();
    void onSentBtnClicked();

public:
    ServerMainWindow(Ui::EasyRemoteDesktopClass *ui);
    ~ServerMainWindow();

private:
    Ui::EasyRemoteDesktopClass *m_pUi;

    bool m_bPreviewFlag = false;
    bool m_bSentFlag = false;
};

