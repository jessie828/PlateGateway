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

#include <stdio.h>

using namespace cv;

PlateDetector *PlateDetector::m_instance = 0;

PlateDetector::PlateDetector()
{
    m_plateThresh=85;
    m_symbolMin=7;
    m_symbolMax=9;
    m_symbolCharacters=8;
    m_plateGoodForOcr = 0;
    m_detectionTime = 0.0;
    m_thresh = 50;
    m_xminmargin=0;
    m_xmaxmargin=352;
    m_yminmargin=0;
    m_ymaxmargin=288;
    m_cSrcStep=352;
    m_count_step=0;
    m_tAPI = new tesseract::TessBaseAPI();

    printf("Tesseract-ocr version: %s\n",
        tesseract::TessBaseAPI::Version());

    printf("Leptonica version: %s\n",
        getLeptonicaVersion());

    int rc = m_tAPI->Init(NULL, NULL, tesseract::OEM_DEFAULT, NULL, 0, NULL, NULL, false);

    if (rc) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    m_tAPI->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);

    m_tAPI->SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-");
}


PlateDetector::~PlateDetector()
{

}


PlateDetector* PlateDetector::getInstance()
{
    if (m_instance == 0)
    {
        m_instance = new PlateDetector;
        if (m_instance == 0)
        {
            printf("Memory Allocation Error when creating data structures\n");
        }
    }

    return m_instance;
}


bool PlateDetector::plateDetect(const QString &filename)
{
    m_detectFlag = true;
    filename.toAscii().data();
    m_videoCapture = VideoCapture(filename.toAscii().data());
    if(!m_videoCapture.isOpened())
    {
        return false;
    }

    Mat frame_reduced;
    Mat frame_gray;
    Mat frame_img;
    Mat plate;
    char plate_str[256];

    Size frameReducedSize = Size(320,240);
    Size frameImgSize = Size(640,480);
    Size plateSize = Size(631,201);

    //wait for activation
    while(m_detectFlag)
    {
        while(true)
        {
            m_videoCapture >> m_frame; // get first frame from video
            if(m_frame.data != 0)
            {
                QStringList plateStrList;
                int ret_val = DistanceTransform(m_frame, plate, plateStrList);
                printf("retval is %i\n", ret_val);

                if(ret_val==1)
                {
                    while(plateStrList.count()!=0)
                    {
                        QString str = plateStrList.takeFirst();
                        strcpy(plate_str, str.toAscii().data());
                        strcpy(m_best_plate, plate_str);
                    }
                }

                resize(m_frame, frame_img, frameImgSize);
                memcpy(m_frame_image, frame_img.data, 640*480*3);
                memcpy(m_histogram_image, plate.data, 631*201*3);
            }
            else
            {
                m_detectFlag=false;
                break;
            }

            if(!m_detectFlag)
                break;
        }
    }

    printf("Close Thread\n");
    m_tAPI->Clear();
    m_tAPI->End();

    return true;
}


bool PlateDetector::stopPlateDetect()
{
    return true;
}


int PlateDetector::getImage(unsigned char *img, int dimx, int dimy)
{
    if(!m_videoCapture.isOpened())
    {
        printf("Error VideoStream not opened\n");
        return -1;
    }

    Mat edges;
    cvtColor(m_frame, edges, CV_BGR2GRAY);
    GaussianBlur(edges, edges, Size(dimx, dimy), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);
    img = edges.data;

    return 0;
}


int PlateDetector::DistanceTransform(Mat &img_in , Mat &plate, QStringList &str_list)
{
    long t_start=getTickCount();
    int k=0;

    Mat dist;
    Mat dist8u1;
    Mat dist8u2;
    Mat dist8u;
    Mat dist32s;
    Mat gray;
    Mat edge;
    Mat img_out;
    Mat image_blob;
    Mat image_blob_orig;
    Mat m_tmp8uC1;

    m_tmp8uC1 = Mat(img_in.rows, img_in.cols, CV_8U);
    img_out = Mat(img_in.rows, img_in.cols, CV_8U);
    dist = Mat(img_in.rows, img_in.cols, CV_32F);
    dist8u = Mat(img_in.rows, img_in.cols, CV_8U);
    dist32s = Mat(img_in.rows, img_in.cols, CV_32S);

    img_out = img_in.clone();

    Mat img_wb = Mat(img_out.rows, img_out.cols, CV_8U, 3);

    unsigned char *img_wb_buffer=new unsigned char[img_out.rows*img_out.cols*3];
    unsigned char *img_out_buffer=new unsigned char[img_out.rows*img_out.cols*3];

    cvtColor(img_out, m_tmp8uC1, CV_RGB2GRAY);

    gray = m_tmp8uC1.clone();
    edge = gray.clone();
    dist8u1 = gray.clone();
    dist8u2 = gray.clone();

    unsigned char *blob_image = new unsigned char[(img_out.rows*img_out.cols*3)*16];

    vector<vector<Point> > rect_seq = findSquares4(img_out);

    int size;
    for(int i = 0; i < size; i += 4 )
    {
        // read 4 vertices
//        memcpy( m_pt, reader.ptr, rect_seq.elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq.elem_size, reader );
//        memcpy( m_pt + 1, reader.ptr, rect_seq.elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq.elem_size, reader );
//        memcpy( m_pt + 2, reader.ptr, rect_seq.elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq.elem_size, reader );
//        memcpy( m_pt + 3, reader.ptr, rect_seq.elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq.elem_size, reader );

        int xmin=10000;
        int ymin=10000;
        int xmax=0;
        int ymax=0;

        for(int j=0;j<4;j++)
        {
            if(m_pt[j].x<xmin)
                xmin=m_pt[j].x;
            if(m_pt[j].y<ymin)
                ymin=m_pt[j].y;

            if(m_pt[j].x>xmax)
                xmax=m_pt[j].x;
            if(m_pt[j].y>ymin)
                ymax=m_pt[j].y;
        }

        int area=(xmax-xmin)*(ymax-ymin);
        double result_ratio=(double)(ymax-ymin)/(double)(xmax-xmin);

        if(!((result_ratio>0.2) && (result_ratio<0.3)) || area>100000 || area<1000)
        {
            continue;
        }

        Scalar scal(255, 0, 0);
        rectangle(img_in, Point(xmin, ymin), Point(xmax, ymax), Scalar(255, 0 ,0), 2);
        rectangle(img_out, Point(xmin, ymin), Point(xmax, ymax),Scalar(255,255,255), 2 );//clean margin noise

        int _minx=xmin;
        int _miny=ymin;
        int _maxx=xmax;
        int _maxy=ymax;

        image_blob = Mat(Size(abs(_maxx-_minx),abs(_maxy-_miny)), CV_8U,1);
        image_blob_orig = Mat(Size(abs(_maxx-_minx),abs(_maxy-_miny)),CV_8U,1);
        TrExtract_SubImage(img_out, image_blob_orig,_minx,_miny,_maxx,_maxy);
        image_blob = Mat(Size(abs(_maxx-_minx),abs(_maxy-_miny)),CV_8U,3);
        TrExtract_SubImage(img_out, image_blob,_minx,_miny,_maxx,_maxy);
        char bmp_filename[256];

        Mat image_plate_inv = Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,1);
        Mat image_plate_2x = Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,1);
        Mat image_symbols = Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,1);
        Mat image_blob_2x = Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,3);
        Mat color_plate= Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,3);
        Mat image_symbols_inv = Mat(Size(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),CV_8U,1);

        memset(image_symbols.data,0,2*abs(_maxx-_minx)*2*abs(_maxy-_miny));

        resize(image_blob_orig,  image_plate_2x, image_plate_2x.size(), CV_INTER_LINEAR );

        equalizeHist(image_plate_2x,image_plate_2x);

        threshold(image_plate_2x, image_plate_inv, m_plateThresh, 255, CV_THRESH_BINARY);//VERY IMPORTANT
        threshold(image_plate_inv, image_plate_2x, 0, 255, CV_THRESH_BINARY_INV);

        erode(image_plate_2x,image_plate_2x, m_frame);
        erode(image_plate_2x,image_plate_2x, m_frame);

        dilate(image_plate_2x,image_plate_2x, m_frame);
        dilate(image_plate_2x,image_plate_2x, m_frame);

        memcpy(blob_image, image_plate_2x.data, (image_plate_2x.rows*image_plate_2x.cols));

        //plate segmentation with blob coloring
        int symbolNumber = BlobColoring((unsigned char *)blob_image,MBlobsFindedCharacter,image_plate_2x.rows,
            image_plate_2x.cols,1000,(image_plate_2x.rows*image_plate_2x.cols),4,2,2,0,0);

        sprintf(bmp_filename,"./images/image.pbm");

        //find symbols
        for(k=0;k<symbolNumber;k++)
        {
            int sizex = abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX);
            int sizey = abs(MBlobsFindedCharacter[k].MaxY-MBlobsFindedCharacter[k].MinY);

            if(!(((double)sizex/(double)sizey)<0.7 && ((double)sizex/(double)sizey)>0.0))
            {
                MBlobsFindedCharacter[k].BlobId=-1;
            }
            else
            {
                MBlobsFindedCharacter[k].BlobId=1;
            }
        }

        //delete not symbols
        int counter=0;
        for(k=0;k<symbolNumber;k++)
        {
            if(MBlobsFindedCharacter[k].BlobId!=-1)
            {
                MBlobsFindedCharacterTmp[counter].MinX=MBlobsFindedCharacter[k].MinX;
                MBlobsFindedCharacterTmp[counter].MinY=MBlobsFindedCharacter[k].MinY;
                MBlobsFindedCharacterTmp[counter].MaxX=MBlobsFindedCharacter[k].MaxX;
                MBlobsFindedCharacterTmp[counter].MaxY=MBlobsFindedCharacter[k].MaxY;
                MBlobsFindedCharacterTmp[counter].BlobId=MBlobsFindedCharacter[k].BlobId;
                counter++;
            }
        }

        symbolNumber=counter;

        //ordering finded symbols
        counter=0;

        while(1)
        {
            int min_value=1000;
            int min_value_pos=0;

            int ordered=1;

            for(k=0;k<symbolNumber;k++)
            {
                if(MBlobsFindedCharacterTmp[k].MinX<min_value && MBlobsFindedCharacterTmp[k].BlobId!=-1)
                {
                    min_value=MBlobsFindedCharacterTmp[k].MinX;
                    min_value_pos=k;
                    ordered=0;
                }
            }

            if(ordered==0)
            {
                MBlobsFindedCharacter[counter].MinX=MBlobsFindedCharacterTmp[min_value_pos].MinX;
                MBlobsFindedCharacter[counter].MinY=MBlobsFindedCharacterTmp[min_value_pos].MinY;
                MBlobsFindedCharacter[counter].MaxX=MBlobsFindedCharacterTmp[min_value_pos].MaxX;
                MBlobsFindedCharacter[counter].MaxY=MBlobsFindedCharacterTmp[min_value_pos].MaxY;
                MBlobsFindedCharacter[counter].BlobId=MBlobsFindedCharacterTmp[min_value_pos].BlobId;
                MBlobsFindedCharacterTmp[min_value_pos].BlobId=-1;
                counter++;
            }
            else
            {
                break;
            }
        }

        int space_symbols=10;

        //extract symbols and create new plate
        for(k=0;k<symbolNumber;k++)
        {
            Mat image_symbol = Mat(Size(abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX),
                abs(MBlobsFindedCharacter[k].MaxY-MBlobsFindedCharacter[k].MinY)) ,CV_8U,1);

            TrExtract_SubImage(image_plate_2x,image_symbol ,MBlobsFindedCharacter[k].MinX,
                MBlobsFindedCharacter[k].MinY,
                MBlobsFindedCharacter[k].MaxX,MBlobsFindedCharacter[k].MaxY);

            TrPaste_SubImage(image_symbol,image_symbols, space_symbols,10);
            space_symbols+=abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX)+10;
        }

        //ck for right symbol number
        if(symbolNumber > m_symbolMin && symbolNumber < m_symbolMax)
        {
            threshold(image_symbols,image_symbols_inv , 1, 255, CV_THRESH_BINARY_INV);

            cvtColor(image_symbols_inv, color_plate, CV_GRAY2RGB);
            resize(color_plate, plate, plate.size(), CV_INTER_LINEAR);

            //OCR
            char cmdline[1024];
            char platevalue[1024];
            int counter_chars=0;
            int ocrerror=0;
            char plate_str_inv[256];
            plate_str_inv[0]='\0';
            char plate_str[256];
            plate_str[0]='\0';

            m_tAPI->SetImage((const unsigned char *)image_symbols_inv.data,
                image_symbols_inv.rows,
                image_symbols_inv.cols,
                1,image_symbols_inv.rows);

            char* outText=m_tAPI->GetUTF8Text();
//            printf("Plate: %s",outText);
            printf("---------------------------------------------------------------------------\n"
                "Plate: %s\n"
                "-------------------------------------------------------------------------------",outText);

            int j=0;
            //int nt=0;

            int stringlength = strlen(outText);
            for(int u = 0; u < stringlength; u++)
            {
                if(outText[u]!='\n')
                {
                    if(outText[u+1]=='\n')
                    {
                        plate_str_inv[j]=outText[u];
                        j++;
                    }
                    else
                    {
                        plate_str_inv[j]='#';
                        j++;
                        char val='\0';
                        do
                        {
                            val=outText[u];
                            u++;
                        } while(val!='\n');
                    }
                }
            }

            plate_str_inv[j+1]='\0';

            j--;

            stringlength = strlen(plate_str_inv);
            for(int e = 0; e < stringlength; e++)
            {
                plate_str[e]=plate_str_inv[j];
                j--;
            }

            plate_str[9]='\0';

            int cc=strlen(plate_str);

            char c;
            int counter_cc=0;

            while(counter_cc<cc)
            {
                c=plate_str[counter_cc];

                if(counter_cc<2 || counter_cc>4)
                {
                    if((c>=65 && c<=90) && c!='\n')
                    {
                        platevalue[counter_chars]=c;
                        counter_chars++;
                    }
                }
                else if(counter_cc>=2 && counter_cc<=4)
                {
                    if((c>=48 && c<=57) && c!='\n')
                    {
                        platevalue[counter_chars]=c;
                        counter_chars++;
                    }

                    if(c==79 || c==81)
                    {
                        platevalue[counter_chars]='0';
                        counter_chars++;
                    }
                }

                counter_cc++;
            }

            platevalue[counter_chars]='\0';

#ifdef LEARN_OCR
            sprintf(cmdline,"gocr -m 130 %s",bmp_filename);//learn
#endif
#ifndef LEARN_OCR
            sprintf(cmdline,"gocr -m 2 -a 96 %s",bmp_filename);
#endif
            counter_chars=0;

            printf("plate:");

            FILE *platechars = popen(cmdline,"r");

            if(platechars!=NULL)
            {
                while(!feof(platechars))
                {
                    char c;

                    fread(&c,1,1,platechars);

                    if(counter_chars<2 || counter_chars>4)
                    {
                        if(c>97 && c<122)
                        {
                            c=c-32;
                        }

                        if((c>=65 && c<=90) && c!='\n')
                        {
                            platevalue[counter_chars]=c;
                            counter_chars++;
                        }
                    }
                    else if(counter_chars>=2 && counter_chars<=4)
                    {
                        if((c>=48 && c<=57) && c!='\n')
                        {
                            platevalue[counter_chars]=c;
                            counter_chars++;
                        }

                        if(c==79 || c==81)
                        {
                            platevalue[counter_chars]='0';
                            counter_chars++;
                        }
                    }

                    printf("%c",c);
                }

                platevalue[counter_chars]='\0';
                pclose(platechars);
            }

            printf("\n");

            //printf("Plate correct %s,%d\n",platevalue,counter_chars);

            if(counter_chars == m_symbolCharacters)
            {
                printf("Plate correct %s is the %d finded rect with area:%d,ratio:%f\n",platevalue,i/4,area,result_ratio);

                str_list.append((QString)platevalue);

                rectangle(img_in,
                    cvPoint(_minx,_miny),
                    cvPoint(_maxx,_maxy),
                    CV_RGB(255,0,0), 3 );

                for(k = 0; k < symbolNumber; k++)
                {
                    rectangle(img_in,
                        cvPoint(MBlobsFindedCharacter[k].MinX/2+_minx,MBlobsFindedCharacter[k].MinY/2+_miny),
                        cvPoint(MBlobsFindedCharacter[k].MaxX/2+_minx,MBlobsFindedCharacter[k].MaxY/2+_miny),
                        Scalar(255,0,0), 2 );
                }
                m_plateGoodForOcr++;
                delete []img_wb_buffer;
                delete []img_out_buffer;
                delete []blob_image;
                return 1;
            }
        }
        else
        {
            printf("Unable to see plate chars\n");
        }
    }
    delete []blob_image;
    delete []img_wb_buffer;
    delete []img_out_buffer;
    long t_stop=getTickCount();

    m_detectionTime=(t_stop-t_start)/(double)(getTickFrequency()*1000.0);

    return 0;
}


vector<vector<Point> > PlateDetector::findSquares4(const Mat &img)
{
    vector<vector<Point> > contours;
    int i, c, l, N = 11;
    Size sz = Size( img.rows & -2, img.cols & -2 );
    Mat timg = Mat(img); // make a copy of input image
    Mat gray = Mat( sz, 8, 1 );
    Mat pyr = Mat( Size(sz.width/2, sz.height/2), 8, 3 );
    Mat tgray;
    vector<vector<Point> > result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    vector<vector<Point> > squares;

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    Rect rect( 0, 0, sz.width, sz.height );
    timg = img(rect);

    // down-scale and upscale the image to filter out the noise
    pyrDown( timg, pyr, pyr.size(), 7 );
    pyrUp( pyr, timg,  timg.size(), 7 );
    tgray = Mat( sz, 8, 1 );

    // find squares in every color plane of the image
    for( c = 0; c < 3; c++ )
    {
        // extract the c-th color plane
//        cvSetImageCOI( timg, c+1 );
//        cvCopy( timg, tgray, 0 );

        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny( tgray, gray, 0, m_thresh, 3 );//not 5, faster with 3
                // dilate canny output to remove potential
                // holes between edge segments
                dilate( gray, gray, gray, Point(-1,-1), 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                threshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }

            // find contours and store them all as a list
            QVector<Point> cont;
            findContours( gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );

            // test each contour
            int contoursize = contours.size();
            for( int i = 0; i< contoursize; i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), result[i], arcLength(contours[i], true)*0.02, true);
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result.size() == 4 &&
                    fabs(contourArea(result)) > 1000 &&
                    isContourConvex(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(result[i][i], result[i][i-2], result[i][i-1]));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( s < 0.3 )
                        for( i = 0; i < 4; i++ )
                            squares[i][i] = result[i][i];
                }
            }
        }
    }

    return squares;
}


double PlateDetector::angle(const Point &pt1, const Point &pt2, const Point &pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}


void PlateDetector::TrExtract_SubImage(const Mat &srcimg, Mat &dstimg, int xmin, int ymin, int xmax, int ymax)
{
    uchar *src=NULL;
    uchar *dst=NULL;

    int i,j,q, src_step, dst_step;

    src_step=(srcimg).step;
    dst_step=(dstimg).step;

    src = (uchar*)(srcimg.data)+(ymin*src_step);
    dst = (uchar*)(dstimg.data);

    if(srcimg.channels()==3 && dstimg.channels()==3)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=3)
            {
                dst[q]=src[j];
                dst[q+1]=src[j+1];
                dst[q+2]=src[j+2];
            }
    }

    if(srcimg.channels()==3 && dstimg.channels()==1)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=1)
            {
                dst[q]=(uchar)(0.212671*src[j]+0.715160*src[j+1]+0.072169*src[j+2]);
            }
    }

    if(srcimg.channels()==1 && dstimg.channels()==1)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=xmin, q=0; j<xmax; j+=1, q+=1)
            {
                dst[q]=src[j];
            }
    }

    if(srcimg.channels()==1 && dstimg.channels()==3)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=xmin, q=0; j<xmax; j+=1, q+=3)
            {
                dst[q]=src[j];
                dst[q+1]=src[j];
                dst[q+2]=src[j];
            }
    }
}


int PlateDetector::BlobColoring(unsigned char src[],MBlobs MBlobsVTemp[],int dimx,int dimy,int minarea,int maxarea,int size,int xsize,int ysize, int ratio,int merging)
{
    int BlobFinded=0;
    int counter=0;
    int status=0;

    m_xmaxmargin=dimx;
    m_ymaxmargin=dimy;
    m_cSrcStep=dimx;

    for(int i=m_yminmargin; i<m_ymaxmargin; i++, src+=m_cSrcStep)
    {
        for(int j=m_xminmargin; j<m_xmaxmargin; j++)
        {
            if(src[j]>200)
            {
                m_minx = m_xmaxmargin;
                m_maxx = m_xminmargin;
                m_miny = m_ymaxmargin;
                m_maxy = m_yminmargin;

                m_blob_size = 0;

                m_count_step=0;
                m_src_image=src+j;
                m_src_x=j;
                m_src_y=i;
                _blob_coloring();

                if ((m_blob_size > size))
                {
                    if (((m_maxx - m_minx) > xsize) && ((m_maxy - m_miny) > ysize))
                    {
                        int area_blob=(m_maxx-m_minx)*(m_maxy-m_miny);

                        double result_ratio=(double)(m_maxx-m_minx)/(double)(m_maxy-m_miny);

                        if(area_blob>minarea && area_blob<maxarea)
                        {
                            if(ratio)
                            {
                                if((result_ratio>1.0) && (result_ratio<20.0))
                                {
                                    //printf("Blob %d (%d,%d,%3.3f)\n",
                                    //  counter,(_maxx-_minx),(_maxy-_miny),result_ratio);

                                    MBlobsVTemp[BlobFinded].MaxX=m_maxx;
                                    MBlobsVTemp[BlobFinded].MaxY=m_maxy;
                                    MBlobsVTemp[BlobFinded].MinX=m_minx;
                                    MBlobsVTemp[BlobFinded].MinY=m_miny;

                                    MBlobsVTemp[BlobFinded].BlobId = counter;

                                    counter++;
                                    BlobFinded++;
                                }
                            }
                            else
                            {
                                //printf("Blob %d (%d,%d)\n",
                                //        counter,(_maxx-_minx),(_maxy-_miny));

                                MBlobsVTemp[BlobFinded].MaxX=m_maxx;
                                MBlobsVTemp[BlobFinded].MaxY=m_maxy;
                                MBlobsVTemp[BlobFinded].MinX=m_minx;
                                MBlobsVTemp[BlobFinded].MinY=m_miny;

                                MBlobsVTemp[BlobFinded].BlobId = counter;

                                counter++;
                                BlobFinded++;
                            }
                        }
                    }
                }
            }
        }
    }

    if(merging)
    {
        return mer_overlapped(MBlobsVTemp,BlobFinded);
    }
    else
    {
        return BlobFinded;
    }
}


int PlateDetector::BlobColoring(unsigned char src[],unsigned char dst[])
{
    int i,j;
    int color=50;
    int nblobsdetected=0;
    int TmpVal=0;
    int TmpVal2=0;

    int xsize = 352;
    int ysize = 288;

    memset(dst,0L,xsize*ysize);

    for(i=(xsize*ysize);i>0;i--)
    {
        if(src[i]==255)
        {
            if(src[i+1]==255 && src[i+xsize]==0)//f(Xl)==1 && f(Xu)==0
            {
                dst[i]=dst[i+1];//color(Xc) = color(Xl)
            }

            if(src[i+1]==0 && src[i+xsize]==255)//f(Xl)==0 && f(Xu)==1
            {
                dst[i]=dst[i+xsize];//color(Xc) = color(Xu)
            }

            if(src[i+1]==255 && src[i+xsize]==255)//f(Xl)==1 && f(Xu)==1
            {
                dst[i]=dst[i+xsize]; //color(Xc) = color(Xu)

                TmpVal=dst[i+1];
                TmpVal2=dst[i+xsize];

                for(j=(xsize*ysize);j>0;j--)
                {
                    (dst[j]==TmpVal) ? dst[j]=TmpVal2:0;
                }
            }

            if(src[i+1]==0 && src[i+xsize]==0)//f(Xu)==0 && f(Xl)==0
            {
                dst[i]=color;//color(Xc)=new color
                color++;
            }
        }
    }

    return color;
}


int PlateDetector::cleardeletedblobs(MBlobs MBlobsVTemp[],int size)
{
    MBlobs *MBlobListTemp = new MBlobs[size];

    int countblob=0;
    int i=0;

    for(i=0;i<size;i++)
    {
        if(MBlobsVTemp[i].BlobId!=-1)
        {
            MBlobListTemp[countblob].MaxX=MBlobsVTemp[i].MaxX;
            MBlobListTemp[countblob].MaxY=MBlobsVTemp[i].MaxY;
            MBlobListTemp[countblob].MinX=MBlobsVTemp[i].MinX;
            MBlobListTemp[countblob].MinY=MBlobsVTemp[i].MinY;
            MBlobListTemp[countblob].BlobId=MBlobsVTemp[i].BlobId;
            countblob++;
        }
    }

    for(i=0;i<countblob;i++)
    {
        MBlobsVTemp[i].MaxX=MBlobListTemp[i].MaxX;
        MBlobsVTemp[i].MaxY=MBlobListTemp[i].MaxY;
        MBlobsVTemp[i].MinX=MBlobListTemp[i].MinX;
        MBlobsVTemp[i].MinY=MBlobListTemp[i].MinY;
        MBlobsVTemp[i].BlobId=MBlobListTemp[i].BlobId;

        //printf("Blob Merged %d (%d,%d)\n",
        //  MBlobsVTemp[i].BlobId,
        //  (MBlobsVTemp[i].MaxX-MBlobsVTemp[i].MinX),
        //  (MBlobsVTemp[i].MaxY-MBlobsVTemp[i].MinY));
    }

    delete []MBlobListTemp;

    return countblob;
}


int PlateDetector::mer_overlapped(MBlobs MBlobsVTemp[],int size)
{
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        {
            if(MBlobsVTemp[j].BlobId!=-1)
            {
                if(i!=j)
                {
                    if(overlapped(MBlobsVTemp[i],MBlobsVTemp[j]))
                    {
                        MBlobsVTemp[i].MaxX=MAX_VAL(MBlobsVTemp[i].MaxX,MBlobsVTemp[j].MaxX);
                        MBlobsVTemp[i].MaxY=MAX_VAL(MBlobsVTemp[i].MaxY,MBlobsVTemp[j].MaxY);
                        MBlobsVTemp[i].MinX=MIN_VAL(MBlobsVTemp[i].MinX,MBlobsVTemp[j].MinX);
                        MBlobsVTemp[i].MinY=MIN_VAL(MBlobsVTemp[i].MinY,MBlobsVTemp[j].MinY);
                        MBlobsVTemp[i].BlobId=MBlobsVTemp[j].BlobId;
                        MBlobsVTemp[j].BlobId=-1;
                        break;
                    }
                }
            }
        }
    }

    return cleardeletedblobs(MBlobsVTemp,size);
}


void PlateDetector::TrPaste_SubImage(const Mat &srcimg, Mat &dstimg, int xmin, int ymin)
{
    uchar *src=NULL;
    uchar *dst=NULL;

    int i,j,q, src_step, dst_step;

    src_step=(srcimg).step;
    dst_step=(dstimg).step;

    dst = (uchar*)(dstimg.data)+(ymin*dst_step);
    src = (uchar*)(srcimg.data);

    int xmax = MIN(xmin+srcimg.rows, dstimg.rows);
    int ymax = MIN(ymin+srcimg.cols, dstimg.cols);

    if(dstimg.channels()==3 && srcimg.channels()==3)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=3)
            {
                dst[j]=src[q];
                dst[j+1]=src[q+1];

                dst[j+2]=src[q+2];
            }
    }
    if(srcimg.channels()==3 && dstimg.channels()==1)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=xmin, q=0; j<xmax; j+=1, q+=3)

            {
                dst[j]=(uchar)(0.212671*src[q]+0.715160*src[q+1]+0.072169*src[q+2]);
            }
    }
    if(srcimg.channels()==1 && dstimg.channels()==1)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=xmin, q=0; j<xmax; j+=1, q+=1)
            {
                dst[j]=src[q];
            }
    }
    if(srcimg.channels()==1 && dstimg.channels()==3)
    {
        for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
            for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=1)
            {
                dst[j]=src[q];
                dst[j+1]=src[q];
                dst[j+2]=src[q];
            }
    }
}


void PlateDetector::_blob_coloring()
{
    recursive_entry:
    if(m_src_x < m_xmaxmargin)
    {
        m_src_image++;
        m_src_x++;

        if(m_src_image[0]>200)
        {
            m_src_image[0]=150;
            m_count_step++;
            m_blob_size++;
            (m_src_x < m_minx) ? m_minx = m_src_x:0;
            (m_src_x > m_maxx) ? m_maxx = m_src_x:0;
            (m_src_y < m_miny) ? m_miny = m_src_y:0;
            (m_src_y > m_maxy) ? m_maxy = m_src_y:0;

            m_blob_coloring_id[m_count_step]=1;

            goto recursive_entry;
            return_entry1:

            m_count_step--;
        }
        m_src_x--;
        m_src_image--;
    }

    if(m_src_x > m_xminmargin)
    {
        m_src_image--;
        m_src_x--;

        if(m_src_image[0]>200)
        {
            m_src_image[0]=150;
            m_count_step++;
            m_blob_size++;
            (m_src_x < m_minx) ? m_minx = m_src_x:0;
            (m_src_x > m_maxx) ? m_maxx = m_src_x:0;
            (m_src_y < m_miny) ? m_miny = m_src_y:0;
            (m_src_y > m_maxy) ? m_maxy = m_src_y:0;

            m_blob_coloring_id[m_count_step]=2;

            goto recursive_entry;
            return_entry2:

            m_count_step--;
        }

        m_src_x++;
        m_src_image++;
    }

    if(m_src_y < m_ymaxmargin)
    {
        m_src_image+=m_cSrcStep;
        m_src_y++;
        if(m_src_image[0]>200)
        {
            m_src_image[0]=150;

            m_count_step++;

            m_blob_size++;

            (m_src_x < m_minx) ? m_minx = m_src_x:0;
            (m_src_x > m_maxx) ? m_maxx = m_src_x:0;
            (m_src_y < m_miny) ? m_miny = m_src_y:0;
            (m_src_y > m_maxy) ? m_maxy = m_src_y:0;

            m_blob_coloring_id[m_count_step]=3;

            goto recursive_entry;
            return_entry3:

            m_count_step--;
        }

        m_src_y--;
        m_src_image-=m_cSrcStep;

    }

    if(m_src_y > m_yminmargin)
    {
        m_src_image-=m_cSrcStep;
        m_src_y--;

        if(m_src_image[0]>200)
        {
            m_src_image[0]=150;

            m_count_step++;

            m_blob_size++;

            (m_src_x < m_minx) ? m_minx = m_src_x:0;
            (m_src_x > m_maxx) ? m_maxx = m_src_x:0;
            (m_src_y < m_miny) ? m_miny = m_src_y:0;
            (m_src_y > m_maxy) ? m_maxy = m_src_y:0;

            m_blob_coloring_id[m_count_step]=4;

            goto recursive_entry;
            return_entry4:

            m_count_step--;
        }
        m_src_y++;
        m_src_image+=m_cSrcStep;
    }

    if(m_count_step==0)
        return;

    if(m_blob_coloring_id[m_count_step]==1)
    {
        goto return_entry1;
    }
    if(m_blob_coloring_id[m_count_step]==2)
    {
        goto return_entry2;
    }
    if(m_blob_coloring_id[m_count_step]==3)
    {
        goto return_entry3;
    }
    if(m_blob_coloring_id[m_count_step]==4)
    {
        goto return_entry4;
    }
}


bool PlateDetector::overlapped(MBlobs b1, MBlobs b2)
{
    if(b1.MinX>b2.MaxX)
    {
        return false;
    }

    if(b2.MinX>b1.MaxX)
    {
        return false;
    }

    if(b1.MinY>b2.MaxY)
    {
        return false;
    }

    if(b2.MinY>b1.MaxY)
    {
        return false;
    }

    return true;
}


int PlateDetector::getHistogram(unsigned char *img,int dimx,int dimy)
{
    Rgb2Gbr(m_histogram_image,img,dimx,dimy);
    return 0;
}


int PlateDetector::Rgb2Gbr(unsigned char *src,unsigned char *dst, int dimx,int dimy)
{
    int counter=0;

    for(int i=0;i<dimx;i++)
    {
        for(int j=0;j<dimy;j++)
        {
            dst[counter]=src[counter+2];
            dst[counter+1]=src[counter+1];
            dst[counter+2]=src[counter];
            counter+=3;
        }
    }

    return 0;
}


void PlateDetector::getBestPlate(char *value)
{
    strncpy(value, m_best_plate, 256);
}


double PlateDetector::getTime()
{
    return m_detectionTime;
}
