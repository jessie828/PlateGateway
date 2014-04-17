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
#include <QList>

using namespace cv;

class PlateDetector
{

#define MAX_VAL(a,b)        (((a) > (b)) ? (a) : (b))
#define MIN_VAL(a,b)        (((a) < (b)) ? (a) : (b))

public:
    static PlateDetector *getInstance();

    int getImage(unsigned char *img,int dimx,int dimy);
    bool plateDetect(const QString &filename);
    bool stopPlateDetect();
    int getHistogram(unsigned char *img,int dimx,int dimy);
    void getBestPlate(char *value);
    double getTime();

protected:
    PlateDetector();
    ~PlateDetector();

private:
    static PlateDetector *m_instance;
    VideoCapture m_videoCapture;
    Mat m_frame;
    tesseract::TessBaseAPI *m_tAPI;
    bool m_detectFlag;
    unsigned char m_frame_image[1920*1200*3];
    unsigned char m_histogram_image[640*480*3];
    unsigned char *m_src_image;
    unsigned char m_blob_coloring_id[20000000];
    char m_best_plate[256];
    int m_plateThresh;
    int m_thresh;
    int m_symbolMin;
    int m_symbolMax;
    int m_symbolCharacters;
    int m_plateGoodForOcr;
    int m_blob_size;
    int m_xminmargin;
    int m_xmaxmargin;
    int m_yminmargin;
    int m_ymaxmargin;
    int m_cSrcStep;
    int m_minx;
    int m_maxx;
    int m_miny;
    int m_maxy;
    int m_count_step;
    int m_src_x;
    int m_src_y;
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
    MBlobs MBlobsFindedCharacterTmp[256];

private:
    int DistanceTransform(Mat &img_in , Mat &plate, QStringList &str_list);
    vector<vector<Point> > findSquares4(const Mat &img);
    double angle(const Point &pt1, const Point &pt2, const Point &pt0);
    void TrExtract_SubImage(const Mat &srcimg, Mat &dstimg, int xmin, int ymin, int xmax, int ymax);
    void TrPaste_SubImage(const Mat &srcimg, Mat &dstimg, int xmin, int ymin);
    int BlobColoring(unsigned char src[],unsigned char dst[]);
    int BlobColoring(unsigned char src[],MBlobs MBlobsVTemp[],int dimx,int dimy,int minarea,int maxarea,int size,int xsize,int ysize, int ratio,int merging);
    int mer_overlapped(MBlobs MBlobsVTemp[],int size);
    int cleardeletedblobs(MBlobs MBlobsVTemp[],int size);
    void _blob_coloring();
    bool overlapped(MBlobs b1, MBlobs b2);
    int Rgb2Gbr(unsigned char *src,unsigned char *dst, int dimx,int dimy);
};

#endif /* PLATEDETECTOR_H_ */
