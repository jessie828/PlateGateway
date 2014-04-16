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

#ifndef PLATEDETECTOR_H_
#define PLATEDETECTOR_H_

//tesseract/ leptonica
#include <baseapi.h>
#include <allheaders.h>

//opencv
#include "opencv2/opencv.hpp"

//QT
#include <QPixmap>
#include <QDateTime>

using namespace cv;

class PlateDetector
{

public:
    static PlateDetector *getInstance();

    int getImage(unsigned char *img,int dimx,int dimy);
    bool plateDetect(const QString &filename);
    bool stopPlateDetect();

protected:
    PlateDetector();
    ~PlateDetector();

private:
    int DistanceTransform(Mat &img_in , Mat &plate, QStringList &str_list);

private:
    static PlateDetector *m_instance;
    VideoCapture m_videoCapture;
    Mat m_frame;
    tesseract::TessBaseAPI *m_tAPI;
    bool m_detectFlag;
    unsigned char m_frame_image[1920*1200*3];
    unsigned char m_histogram_image[640*480*3];
    QList<QString> m_storage;
    int m_plateThresh;
    int m_symbolMin;
    int m_symbolMax;
    int m_symbolCharacters;
    int m_plateGoodForOcr;
    double m_detectionTime;
    Point m_pt[4];

    typedef struct _MBlobs{
        int BlobId;
        int MaxX;
        int MaxY;
        int MinX;
        int MinY;
    }MBlobs;

    MBlobs MBlobsFinded[256];
    MBlobs MBlobsFindedPlate[256];
    MBlobs MBlobsFindedCharacter[256];
    MBlobs MBlobsFindedCharacterTmp[256];;
};

#endif /* PLATEDETECTOR_H_ */
