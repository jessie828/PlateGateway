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

#include "PlateDetector.h"
#include "opencv2/opencv.hpp"

#include <stdio.h>

using namespace cv;

PlateDetector::PlateDetector()
{

}


PlateDetector::~PlateDetector()
{

}


QPixmap PlateDetector::getThumbnail(const QString &filename)
{
    VideoCapture cap(filename.toAscii().data());
    if(!cap.isOpened())
    {
        return QPixmap();
    }

    Mat frame;
    cap >> frame; // get first frame from video
    QPixmap thumbnail;
    cv::Mat temp(frame.cols,frame.rows,frame.type());
    cvtColor(frame, temp,CV_BGR2RGB);
    QImage dest= QImage((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    thumbnail.convertFromImage(dest, Qt::ThresholdDither);
    return thumbnail;
}
