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

#include "RenderHistogram.h"

RenderHistogram::RenderHistogram(RenderHistogramHelper *helper, QWidget *parent)
        : QWidget(parent), helper(helper)
{
    elapsed = 0;
    RenderHistogram::setMinimumSize(QSize(1920,1200));
}

void RenderHistogram::animate(int tmp_width,int tmp_height)
{
    elapsed += 10;
    elapsed = elapsed % 1000;
    width=tmp_width;
    height=tmp_height;
    repaint();
}

void RenderHistogram::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    //painter.setRenderHint(painter.Antialiasing,true);
//    helper->paint(&painter, event, elapsed,width,height);
    painter.end();
}

void RenderHistogram::closeEvent(QCloseEvent *event)
{

}

void RenderHistogram::Go()
{
//    helper->start_rendering();
}

RenderHistogram::~RenderHistogram()
{

}



