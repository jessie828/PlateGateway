////////////////////////////////////////////////////////////////////////////////////
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU
// General Public License version 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE included in the
// packaging of this file.  Please review the following information to
// ensure the GNU General Public License version 3.0 requirements will be
// met: http://www.gnu.org/copyleft/gpl.html.
///////////////////////////////////////////////////////////////////////////////////

#include "RenderVideo.h"

RenderVideo::RenderVideo(QWidget *parent)
        : QWidget(parent)
{
    elapsed = 0;

    RenderVideo::setMinimumSize(QSize(1920,1200));
}

void RenderVideo::animate(int tmp_width,int tmp_height)
{
    elapsed += 10;
    elapsed = elapsed % 1000;
    width=tmp_width;
    height=tmp_height;
    repaint();
}

void RenderVideo::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    //painter.setRenderHint(painter.Antialiasing,true);
//    m_helper->paint(&painter, event, elapsed,width,height);
    painter.end();
}

void RenderVideo::closeEvent(QCloseEvent *event)
{

}

void RenderVideo::Go()
{
//    helper->start_rendering();
}

RenderVideo::~RenderVideo()
{

}


void RenderVideo::showThumbnail(const QString &filename)
{
//    image=QImage("/scratch/videos/snaps/volvo.jpg");//,100,100,QImage::Format_RGB888);
//    QPainter painter;
//    painter.begin(this);
//    painter.drawImage(QRect(0,0,width,height),vid_image,QRect(0,0,100,100),Qt::AutoColor);
}
