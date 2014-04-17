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

#include "RenderHistogramHelper.h"

RenderHistogramHelper::RenderHistogramHelper()
{
    vid_image_data_yuv = new unsigned char[1920*1200*2];
    vid_image_data_rgb = new unsigned char[1920*1200*4];

    image_width=631;
    image_height=201;
}

RenderHistogramHelper::~RenderHistogramHelper()
{
    delete []vid_image_data_yuv;
    delete []vid_image_data_rgb;
}

void RenderHistogramHelper::paint(QPainter *painter, QPaintEvent *event, int elapsed,int width,int height)
{
    vid_image=QImage(vid_image_data_rgb,image_width,image_height,QImage::Format_RGB888);
    //vid_image=vid_image.mirrored(false,true);
    painter->drawImage(QRect(0,0,width,height),vid_image,QRect(0,0,image_width,image_height),Qt::AutoColor);
}

void RenderHistogramHelper::run()
{
    thread_active=true;

    while(thread_active)
    {
        PlateDetector::getInstance()->getHistogram(vid_image_data_rgb,631,201);
        usleep(250000);
    }
}

int RenderHistogramHelper::start_rendering()
{
    start();
    return 0;
}

//RenderHistogramHelper



