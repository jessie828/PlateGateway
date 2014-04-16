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
    char best_plate[256];

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
                        strcpy(best_plate, plate_str);
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

//    m_storage = QList<QString>;
//    CvSeq * rect_seq = 0;//findSquares4(img_out, m_storage);
//    CvSeqReader reader;
//    cvStartReadSeq( rect_seq, &reader, 0 );
//
    int size;
    for(int i = 0; i < size; i += 4 )
    {
        // read 4 vertices
//        memcpy( m_pt, reader.ptr, rect_seq->elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
//        memcpy( m_pt + 1, reader.ptr, rect_seq->elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
//        memcpy( m_pt + 2, reader.ptr, rect_seq->elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
//        memcpy( m_pt + 3, reader.ptr, rect_seq->elem_size );
//        CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );

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
//        TrExtract_SubImage(img_out, image_blob_orig,_minx,_miny,_maxx,_maxy);
        image_blob = Mat(Size(abs(_maxx-_minx),abs(_maxy-_miny)),CV_8U,3);
//        TrExtract_SubImage(img_out, image_blob,_minx,_miny,_maxx,_maxy);
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
        int symbolNumber = 10;//BlobColoring((unsigned char *)blob_image,MBlobsFindedCharacter,image_plate_2x->width,
//            image_plate_2x->height,1000,(image_plate_2x->width*image_plate_2x->height),4,2,2,0,0);

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
            IplImage* image_symbol = cvCreateImage(cvSize(abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX),
                abs(MBlobsFindedCharacter[k].MaxY-MBlobsFindedCharacter[k].MinY))
                ,IPL_DEPTH_8U,1);

//            TrExtract_SubImage(image_plate_2x,image_symbol ,MBlobsFindedCharacter[k].MinX,
//                MBlobsFindedCharacter[k].MinY,
//                MBlobsFindedCharacter[k].MaxX,MBlobsFindedCharacter[k].MaxY);
//
//            TrPaste_SubImage(image_symbol,image_symbols,
//                space_symbols,10);

            cvReleaseImage( &image_symbol );

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

