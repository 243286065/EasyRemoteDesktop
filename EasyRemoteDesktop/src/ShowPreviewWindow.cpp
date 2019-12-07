#include "ShowPreviewWindow.h"
#include "glog/logging.h"

#include <QPainter>

#pragma comment(lib, "opengl32.lib")

ShowPreviewWindow::ShowPreviewWindow(QWidget *parent)
    : QOpenGLWidget(parent) {}

ShowPreviewWindow::~ShowPreviewWindow() {

}

void ShowPreviewWindow::initializeGL() {
    //glClearColor(0.8, 0.8, 0.8, 1.0);   //白色背景
    //glShadeModel(GL_SMOOTH);            //启用阴影平滑
    //glClearDepth(1.0);                  //设置深度缓存
    //glEnable(GL_DEPTH_TEST);            //启动深度测试
    //glDepthFunc(GL_LEQUAL);             //深度测试的类型
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正
}

void ShowPreviewWindow::resizeGL(int w, int h) {
}

void ShowPreviewWindow::paintGL() {}

void ShowPreviewWindow::paintEvent(QPaintEvent *event)
{
    static int cnt = 0;
    if (m_pFrame) {
        cnt++;
        if (cnt % 100 == 0) {
            LOG(INFO) << "----------------" << cnt;
        }
        QPainter painter;
        painter.begin(this);
        QImage img((const uchar *)m_pFrame->m_pixels, (int)m_pFrame->m_headerInfo.biWidth, -(int)m_pFrame->m_headerInfo.biHeight, QImage::Format::Format_ARGB32);
        QImage newImg = img.scaled(width(), height());
        painter.drawImage(QPoint(0, 0), newImg);
        painter.end();
    }
}

void ShowPreviewWindow::keyPressEvent(QKeyEvent *event)
{
}

void ShowPreviewWindow::UpdateFrame(std::shared_ptr<media::CaptureBmpData> frame)
{
    if(m_pFrame){
        //media::EasyScreenCapturer::GetInstance()->FreeCaptureBmpData(*m_pFrame);
        free(m_pFrame->m_pixels);
    }

    m_pFrame = frame;
    update();
}