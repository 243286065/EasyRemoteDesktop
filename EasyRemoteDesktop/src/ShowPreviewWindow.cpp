#include "ShowPreviewWindow.h"
#include "glog/logging.h"

#include "Utils.h"

#include <QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#pragma comment(lib, "opengl32.lib")

ShowPreviewWindow::ShowPreviewWindow(QWidget *parent)
    : QOpenGLWidget(parent) {}

ShowPreviewWindow::~ShowPreviewWindow() {

}

void ShowPreviewWindow::initializeGL() {
    glClearColor(0.8, 0.8, 0.8, 1.0);   //白色背景
    glShadeModel(GL_SMOOTH);            //启用阴影平滑
    glClearDepth(1.0);                  //设置深度缓存
    glEnable(GL_DEPTH_TEST);            //启动深度测试
    glDepthFunc(GL_LEQUAL);             //深度测试的类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正
}

void ShowPreviewWindow::resizeGL(int w, int h) {
}

void ShowPreviewWindow::paintGL() {}

void ShowPreviewWindow::paintEvent(QPaintEvent *event)
{
    if (m_pFrame) {
        QPainter painter;
        painter.begin(this);
        QImage img((const uchar *)m_pFrame->m_pixels.get(), (int)m_pFrame->m_headerInfo.biWidth, -(int)m_pFrame->m_headerInfo.biHeight, QImage::Format::Format_ARGB32);
        QImage newImg = img.scaled(width(), height());
        painter.drawImage(QPoint(0, 0), newImg);
        painter.end();
    }
}

void ShowPreviewWindow::keyPressEvent(QKeyEvent *event)
{
}

void ShowPreviewWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    /*QDesktopWidget* desktop = QApplication::desktop();
    if (desktop && desktop->screenCount() > 1)
    {
        if (isFullScreen())
        {
            showNormal();
            setWindowFlags(Qt::SubWindow);
            showMaximized();
        }
        else
        {
            setWindowFlag(Qt::Window);
            setGeometry(desktop->screenGeometry(0));
            showFullScreen();
        }
    }*/
}

void ShowPreviewWindow::UpdateFrame(std::shared_ptr<media::CaptureBmpData> frame)
{
    m_pFrame = frame;
    update();
}