////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//plateegateway.cpp
//Dexmac 2012
//
//www.dexmac.com
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU
// General Public License version 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL included in the
// packaging of this file.  Please review the following information to
// ensure the GNU General Public License version 3.0 requirements will be
// met: http://www.gnu.org/copyleft/gpl.html.
///////////////////////////////////////////////////////////////////////////////////

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include "plategateway.h"
#include <muin.h>


#define ITALY
//#define INDIAN

using namespace std;

IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0,
*histimg = 0;
CvHistogram *hist = 0;

int backproject_mode = 0;
int select_object = 0;
int track_object = 0;
int show_hist = 1;
CvPoint origin;
CvRect selection;
CvRect track_window;
CvBox2D track_box; 
CvConnectedComp track_comp;
int hdims = 96; 
float hranges_arr[] = {0,180};
float* hranges = hranges_arr;
int vmin = 40, vmax = 160, smin = 80;
int move_cam=1;

int x_old=0;
int y_old=0;
int move_counter=0;

#ifdef WIN32
//#include <windef.h>
#endif

#ifdef WIN32
#define __restrict__ 
#endif

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
}

#ifndef SOCKET
#define SOCKET int
#endif

#ifndef WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "muin.h"

#define closesocket	close

#define SOCKET	int
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1

#ifndef INADDR_ANY
#define INADDR_ANY	0x00000000
#endif

#ifndef INADDR_NONE
#define INADDR_NONE	0xFFFFFFFF
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef SOCKADDR_IN
#define SOCKADDR_IN struct sockaddr_in
#endif

#ifndef LPSOCKADDR
#define LPSOCKADDR struct sockaddr_in *
#endif

#endif

unsigned char frame_image[1920*1200*3];
unsigned char histogram_image[640*480*3];

//#define MUIN

QList<QString> image_list;
QList<QString> plate_list;
int plate_finded=0;

int plate_detect_flag=0;

char ip_addr[256];
int ip_port;
int muin_chan;

char class_file[256];
int sensibility_face;
int recognition_frame_numb=5;
int alert_frame_numb=10;
int alarm_frame_numb=20;

CvHaarClassifierCascade *cascade_object;
CvMemStorage			*storage;

AVFormatContext *ic_dec_mjpeg;
AVCodec *decoder_mjpeg;
SOCKET skt_mjpeg;

int thread_active_mjpeg=0;
int vop_size=0;
int vop_pos=0;
unsigned char tmp_frame_buffer[50000];
unsigned char vop_buffer[200000];
unsigned char yuv_image[704*576*2];
int yellow_elmet=0;

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

int _blob_size;
int _xminmargin=0;
int _xmaxmargin=352;
int _yminmargin=0;
int _ymaxmargin=288;

int _minx, _maxx, _miny, _maxy;

int _c_src_step=352;

int count_step=0;
int counterplate=0;

#ifndef BYTE
#define BYTE unsigned char
#endif

BYTE *src_image;
int src_x;
int src_y;

unsigned char blob_coloring_id[20000000];

typedef struct _plates_recognized_elem
{
        char plate_string[16];
        int hit_number;
} plates_recognized_elem;

plates_recognized_elem plates_recognized_queue[256];
int plates_recognized_elem_number=0;

char plate_string_value[256];

int plate_recognized=0;
int plate_good_for_ocr=0;
int plate_bad_for_ocr=0;
int plate_ocr_hit=0;
int plate_ocr_miss=0;
int frame_number=0;

char plate_cfr[256];

char best_plate[256];

unsigned char tmp_data[100000];

#define PI 3.1415926536

#ifndef WIN32 
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#define LPSOCKADDR struct sockaddr_in *
#define closesocket close
#endif

double detection_time=0.0;

int blue_thresh=128;
int green_thresh=96;
int red_thresh=4;
int plate_thresh=85;
int symbol_min=7;
int symbol_max=9;
int symbol_characters=8;
#define _S(a)		(a)>255 ? 255 : (a)<0 ? 0 : (a)

#define _Red(y,u,v) ((0x2568*(y) + 0x3343*(u))/0x2000)
#define _Green(y,u,v) (0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u)) /0x2000
#define _Blue(y,u,v) (0x2568*(y) + 0x40cf*(v))					     /0x2000

#define MAX_VAL(a,b)        (((a) > (b)) ? (a) : (b))
#define MIN_VAL(a,b)        (((a) < (b)) ? (a) : (b))
int thresh = 50;
IplImage* img = 0;
IplImage* img0 = 0;
CvPoint pt[4];

tesseract::TessBaseAPI *tAPI;

int recvtimeout_http(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);

    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv(s, buf, len, 0);
}

int sendtimeout_http(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, NULL, &fds, NULL, &tv);

    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal send()
    return send(s, buf, len, 0);
}

SOCKET OpenHttpConn(char *ipaddr,int port)
{
	int nRet;
	char tmpbuf[1024];

#ifdef WIN32
	WSADATA wsaData;
	nRet = WSAStartup(MAKEWORD(2,0), &wsaData);
#endif

	struct hostent *he;

	if(inet_addr(ipaddr)==INADDR_NONE)
	{
		if ((he=gethostbyname(ipaddr))==NULL)
		{
			printf("gethostbyname() errorn!\n");
			return -1;
		}

		printf("Hostname : %s\n",he->h_name);  // prints the hostname 

		strcpy(ipaddr,inet_ntoa(*((struct in_addr *)he->h_addr)));

		printf("IP Address: %s\n",ipaddr); // prints IP address
		
		fflush(stdout);
	}

	SOCKADDR_IN sockBuff;

	SOCKET sockId = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(sockId==-1)
	{
		return -1;
	}
	
	sockBuff.sin_family=AF_INET;
	sockBuff.sin_port=htons((u_short)port);

	if((sockBuff.sin_addr.s_addr=inet_addr(ipaddr))==INADDR_NONE)
	{
		return -1;
	}	
	
	if((nRet=connect(sockId,(const sockaddr *)&sockBuff,sizeof(SOCKADDR_IN)))<0)
	{
		printf("Http: Unable to connect\n");
		return -1;
	}

	return sockId;
}

void CloseHttpConn(SOCKET sockId)
{
	closesocket(sockId);
}

int frame_finder_mjpeg(unsigned char *tmp_data_in,unsigned char *tmp_data_out,int len,int mb_mode)
{
	char mboudary[256];
	int	mboundary_len;
	
	if(mb_mode==0)
	{
		strcpy(mboudary,"--myboundary");
	}
	else if(mb_mode==1)
	{
		strcpy(mboudary,"--WINBONDBOUDARY");
	}
	
	mboundary_len=strlen(mboudary);
	
	memcpy(tmp_frame_buffer+vop_pos,tmp_data_in,len);

	for(int k=0;k<len+vop_pos-12;k++)
	{
		if(strncmp(mboudary,(char *)tmp_frame_buffer+k,mboundary_len)==0)
		{
			if(vop_size==0)
			{
				tmp_data_out[vop_size]=tmp_frame_buffer[k];
				vop_size++;
				continue;
			}

			memcpy(tmp_frame_buffer,tmp_frame_buffer+k+12,len+vop_pos-k-12);
			
			vop_pos=len+vop_pos-k-12;
			
			int tmp_vop_size=vop_size;
			
			vop_size=0;
			
			return tmp_vop_size;
		}
		else
		{
			tmp_data_out[vop_size]=tmp_frame_buffer[k];
		}

		vop_size++;
	}

	memcpy(tmp_frame_buffer,tmp_frame_buffer+len+vop_pos-12,12);
	vop_pos=12;

	return -1;
}

int start_finder_mjpeg(unsigned char *tmp_data_in,unsigned char *tmp_data_out,int len)
{
	for(int k=0;k<len;k++)
	{
		if(tmp_data_in[k]==0xFF && tmp_data_in[k+1]==0xD8)
		{
			memcpy(tmp_data_out,tmp_data_in+k,len-k);
			return len-k;
		}
	}

	return -1;
}

int open_mjpeg_encoder()
{
	printf("open ffmpeg image encoder!\n");

	av_register_all();

	decoder_mjpeg = avcodec_find_decoder(CODEC_ID_MJPEG);

	ic_dec_mjpeg = (AVFormatContext *)av_malloc(sizeof(AVFormatContext));
	ic_dec_mjpeg->streams[0] = (AVStream *)av_malloc(sizeof(AVStream));

	ic_dec_mjpeg->streams[0]->codec = avcodec_alloc_context3(0);

	if (!decoder_mjpeg || avcodec_open2(ic_dec_mjpeg->streams[0]->codec, decoder_mjpeg, 0) < 0)
	{
		printf("Unable to open mjpeg encoder!\n");
		return -1;
	}

	return 0;
}

void close_mjpeg_encoder()
{
	AVCodecContext *dec_jpeg;
	dec_jpeg = ic_dec_mjpeg->streams[0]->codec;
	avcodec_close(dec_jpeg);
	av_free(ic_dec_mjpeg->streams[0]);
	av_free(ic_dec_mjpeg);
}  

SOCKET open_mjpeg_stream(char *ip_address,int ip_port,int video_format,int type)
{
	vop_size=0;
	vop_pos=0;

	string http_cmd;

	open_mjpeg_encoder();

        SOCKET skt= OpenHttpConn(ip_address,ip_port);

	if (skt==-1)
	{
		return NULL;
	}

        if(type==0)//Axis
	{
		if(video_format==2)
			http_cmd = (string)"GET /axis-cgi/mjpg/video.cgi?resolution=352x288 HTTP/1.1\r\n";
		else if(video_format==3)
			http_cmd = (string)"GET /axis-cgi/mjpg/video.cgi?resolution=704x576 HTTP/1.1\r\n";
		else if(video_format==4)
			http_cmd = (string)"GET /axis-cgi/mjpg/video.cgi?resolution=320x240 HTTP/1.1\r\n";
		else if(video_format==5)
			http_cmd = (string)"GET /axis-cgi/mjpg/video.cgi?resolution=640x480 HTTP/1.1\r\n";
		else
			http_cmd = (string)"GET /axis-cgi/mjpg/video.cgi HTTP/1.1\r\n";

		http_cmd += (string)"\r\n";
	}
        else if (type==1)//Mobotix
	{
                http_cmd = (string)"GET /cgi-bin/faststream.jpg HTTP/1.1\r\n";
		http_cmd += (string)"\r\n";
	}
		
	sendtimeout_http(skt,(char *)http_cmd.c_str(),1024,10);

	return skt;
}

int recvtimeout_host(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);

    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv(s, buf, len, 0);
}

int sendtimeout_host(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, NULL, &fds, NULL, &tv);

    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal send()
    return send(s, buf, len, 0);
}

SOCKET ConnHttp(char *ipaddr,int port)
{
	int nRet;
	char tmpbuf[1024];

#ifdef WIN32
	WSADATA wsaData;
	nRet = WSAStartup(MAKEWORD(2,0), &wsaData);
#endif

	struct hostent *he;

	if(inet_addr(ipaddr)==INADDR_NONE)
	{
		if ((he=gethostbyname(ipaddr))==NULL)
		{
			printf("gethostbyname() errorn!\n");
			return -1;
		}

		printf("Hostname : %s\n",he->h_name);  // prints the hostname 

		strcpy(ipaddr,inet_ntoa(*((struct in_addr *)he->h_addr)));

		printf("IP Address: %s\n",ipaddr); // prints IP address
		
		fflush(stdout);
	}

	SOCKADDR_IN sockBuff;

	SOCKET sockId = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(sockId==-1)
	{
		return -1;
	}
	
	sockBuff.sin_family=AF_INET;
	sockBuff.sin_port=htons((u_short)port);

	if((sockBuff.sin_addr.s_addr=inet_addr(ipaddr))==INADDR_NONE)
	{
		return -1;
	}	
	
	if((nRet=connect(sockId,(const sockaddr *)&sockBuff,sizeof(SOCKADDR_IN)))<0)
	{
		printf("Ptz: Unable to connect\n");
		return -1;
	}

	return sockId;
}

void CloseHttp(SOCKET sockId)
{
	closesocket(sockId);
}

int receive_mjpeg_stream(SOCKET skt,unsigned char *tmp_data_out,int &width,int &height,int mode)
{
	int got_picture;

	char buffer[2048];
	
	while(1)
	{
		int ret_value = recvtimeout_http(skt, buffer, 1024, 10);

                printf("receiving %d\n",ret_value);
                fflush(stdout);
		
		if(ret_value<=0)
			return -2;

		int data_len  = frame_finder_mjpeg((unsigned char *)buffer,tmp_data,ret_value,mode);

		if(data_len>0)
		{
			int frame_len = start_finder_mjpeg(tmp_data,tmp_data,data_len);

			AVFrame *frame= avcodec_alloc_frame();
			AVFrame *frame_in= avcodec_alloc_frame();
                        AVPacket *avpkt;
                        av_new_packet(avpkt, frame_len);

			int len1=avcodec_decode_video2(ic_dec_mjpeg->streams[0]->codec,
				frame, &got_picture,
				avpkt);
				//(unsigned char *)tmp_data, frame_len);

			unsigned char *image_buffer = new unsigned char[ic_dec_mjpeg->streams[0]->codec->width*
				ic_dec_mjpeg->streams[0]->codec->height*2];

			avpicture_fill((AVPicture *)frame_in,tmp_data_out, PIX_FMT_YUV420P,
				ic_dec_mjpeg->streams[0]->codec->width,
				ic_dec_mjpeg->streams[0]->codec->height);

			int image_size=ic_dec_mjpeg->streams[0]->codec->width*ic_dec_mjpeg->streams[0]->codec->height*1.5;

			width=ic_dec_mjpeg->streams[0]->codec->width;
			height=ic_dec_mjpeg->streams[0]->codec->height;

                        if(ic_dec_mjpeg->streams[0]->codec->pix_fmt==PIX_FMT_YUVJ420P)
                        {
                            avpicture_fill((AVPicture *)frame_in,tmp_data_out, PIX_FMT_YUV420P,
                                    ic_dec_mjpeg->streams[0]->codec->width,
                                    ic_dec_mjpeg->streams[0]->codec->height);


                           av_picture_copy((AVPicture *)frame_in,(AVPicture *)frame,PIX_FMT_YUV420P,ic_dec_mjpeg->streams[0]->codec->width, ic_dec_mjpeg->streams[0]->codec->height);

                            image_size=width*height*1.5;
                        }
                        else
                        {
                            avpicture_fill((AVPicture *)frame_in,tmp_data_out, PIX_FMT_YUV422P,
                                    ic_dec_mjpeg->streams[0]->codec->width,
                                    ic_dec_mjpeg->streams[0]->codec->height);

                            av_picture_copy((AVPicture *)frame_in,(AVPicture *)frame,PIX_FMT_YUV422P,ic_dec_mjpeg->streams[0]->codec->width, ic_dec_mjpeg->streams[0]->codec->height);

                            image_size=width*height*2.0;
                        }

			delete []image_buffer;
			av_free(frame);
			av_free(frame_in);

			return image_size;
		}
	}

	return -1;
}

void close_mjpeg_encoder(SOCKET skt)
{
	CloseHttpConn(skt);
	close_mjpeg_encoder();
	thread_active_mjpeg=0;
	printf("close_mjpeg_encoder\n");
	fflush(stdout);
	//fclose(mjpeg_stream);
}

void yuv2rgb_24(BYTE *puc_y, int stride_y, 
                BYTE *puc_u, BYTE *puc_v, int stride_uv, 
                BYTE *puc_out, int width_y, int height_y) 
{

	int x, y;

	puc_out = puc_out + width_y * (height_y-1) * 3;

	for (y=0; y<height_y; y++) 
	{
		for (x=0; x<width_y; x++)
		{
			signed int _r,_g,_b; 
			signed int r, g, b;
			signed int y, u, v;

			y = puc_y[x] - 16;
			u = puc_u[x>>1] - 128;
			v = puc_v[x>>1] - 128;

			_r = _Red(y,u,v);
			_g = _Green(y,u,v);
			_b = _Blue(y,u,v);

			r = _S(_r);
			g = _S(_g);
			b = _S(_b);

			puc_out[0] = r;
			puc_out[1] = g;
			puc_out[2] = b;

			puc_out+=3;
		}

		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out -= (width_y*6);
	}
}

int VertFlipBuf(BYTE  * inbuf,BYTE  * outbuf,
					  unsigned int widthBytes, 
					  unsigned int height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	unsigned int bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	unsigned int row_cnt;     
	unsigned long off1=0;
	unsigned long off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) 
	{
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(outbuf+off1,tb2,bufsize);
		memcpy(outbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}

CvScalar hsv2rgb( float hue )
{
	int rgb[3], p, sector;

	static const int sector_data[][3]=
	{{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
	hue *= 0.033333333333333333333333333333333f;
	sector = cvFloor(hue);
	
	p = cvRound(255*(hue - sector));
	p ^= sector & 1 ? 255 : 0;

	rgb[sector_data[sector][0]] = 255;
	rgb[sector_data[sector][1]] = 0;
	rgb[sector_data[sector][2]] = p;

	return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

static const double pi = 3.14159265358979323846;

inline static double square(int a)
{
	return a * a;
}

inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	return;

	*img = cvCreateImage( size, depth, channels );
	if ( *img == NULL )
	{
		fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
		exit(-1);
	}
}

void TrExtract_SubImage(IplImage* srcimg, IplImage* dstimg, int xmin, int ymin, int xmax, int ymax)
{
	uchar *src=NULL;
	uchar *dst=NULL;

	int i,j,q, src_step, dst_step;

	src_step=(srcimg)->widthStep;
	dst_step=(dstimg)->widthStep;

	src = (uchar*)(srcimg->imageData)+(ymin*src_step);
	dst = (uchar*)(dstimg->imageData);

	if(srcimg->nChannels==3 && dstimg->nChannels==3)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=3)
			{	
				dst[q]=src[j];
				dst[q+1]=src[j+1];
				dst[q+2]=src[j+2];
			}
	}

	if(srcimg->nChannels==3 && dstimg->nChannels==1)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=1)
			{
				dst[q]=(uchar)(0.212671*src[j]+0.715160*src[j+1]+0.072169*src[j+2]);
			}
	}

	if(srcimg->nChannels==1 && dstimg->nChannels==1)
	{
		for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
			for(j=xmin, q=0; j<xmax; j+=1, q+=1)
			{
				dst[q]=src[j];
			}
	}

	if(srcimg->nChannels==1 && dstimg->nChannels==3)
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

int InitClassifier(char *filename)
{
        cascade_object = (CvHaarClassifierCascade*)cvLoad(filename, 0, 0, 0);

        storage = cvCreateMemStorage(0);

        return 0;
}

int Rgb2Gbr(unsigned char *src,unsigned char *dst, int dimx,int dimy)
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

void TrPaste_SubImage(IplImage* srcimg, IplImage* dstimg, int xmin, int ymin)
{
        uchar *src=NULL;
        uchar *dst=NULL;

        int i,j,q, src_step, dst_step;

        src_step=(srcimg)->widthStep;
        dst_step=(dstimg)->widthStep;

        dst = (uchar*)(dstimg->imageData)+(ymin*dst_step);
        src = (uchar*)(srcimg->imageData);

        int xmax = MIN(xmin+srcimg->width, dstimg->width);
        int ymax = MIN(ymin+srcimg->height, dstimg->height);

        if(dstimg->nChannels==3 && srcimg->nChannels==3)
        {
                for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
                        for(j=3*xmin, q=0; j<3*xmax; j+=3, q+=3)
                        {
                                dst[j]=src[q];
                                dst[j+1]=src[q+1];

                                dst[j+2]=src[q+2];
                        }
        }
        if(srcimg->nChannels==3 && dstimg->nChannels==1)
        {
                for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
                        for(j=xmin, q=0; j<xmax; j+=1, q+=3)

                        {
                                dst[j]=(uchar)(0.212671*src[q]+0.715160*src[q+1]+0.072169*src[q+2]);
                        }
        }
        if(srcimg->nChannels==1 && dstimg->nChannels==1)
        {
                for(i=ymin; i<ymax; i++, src+=src_step, dst+=dst_step)
                        for(j=xmin, q=0; j<xmax; j+=1, q+=1)
                        {
                                dst[j]=src[q];
                        }
        }
        if(srcimg->nChannels==1 && dstimg->nChannels==3)
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

void _blob_coloring()
{

        recursive_entry:

        if(src_x < _xmaxmargin)
        {
                src_image++;
                src_x++;

                if(src_image[0]>200)
                {
                        src_image[0]=150;

                        count_step++;

                        _blob_size++;

                        (src_x < _minx) ? _minx = src_x:0;
                        (src_x > _maxx) ? _maxx = src_x:0;
                        (src_y < _miny) ? _miny = src_y:0;
                        (src_y > _maxy) ? _maxy = src_y:0;

                        blob_coloring_id[count_step]=1;

                        goto recursive_entry;
return_entry1:

                        count_step--;
                }
                src_x--;
                src_image--;
        }

        if(src_x > _xminmargin)
        {
                src_image--;
                src_x--;

                if(src_image[0]>200)
                {
                        src_image[0]=150;

                        count_step++;

                        _blob_size++;

                        (src_x < _minx) ? _minx = src_x:0;
                        (src_x > _maxx) ? _maxx = src_x:0;
                        (src_y < _miny) ? _miny = src_y:0;
                        (src_y > _maxy) ? _maxy = src_y:0;

                        blob_coloring_id[count_step]=2;

                        goto recursive_entry;
return_entry2:

                        count_step--;
                }

                src_x++;
                src_image++;
        }

        if(src_y < _ymaxmargin)
        {
                src_image+=_c_src_step;
                src_y++;
                if(src_image[0]>200)
                {
                        src_image[0]=150;

                        count_step++;

                        _blob_size++;

                        (src_x < _minx) ? _minx = src_x:0;
                        (src_x > _maxx) ? _maxx = src_x:0;
                        (src_y < _miny) ? _miny = src_y:0;
                        (src_y > _maxy) ? _maxy = src_y:0;

                        blob_coloring_id[count_step]=3;

                        goto recursive_entry;
return_entry3:

                        count_step--;
                }

                src_y--;
                src_image-=_c_src_step;

        }

        if(src_y > _yminmargin)
        {
                src_image-=_c_src_step;
                src_y--;

                if(src_image[0]>200)
                {
                        src_image[0]=150;

                        count_step++;

                        _blob_size++;

                        (src_x < _minx) ? _minx = src_x:0;
                        (src_x > _maxx) ? _maxx = src_x:0;
                        (src_y < _miny) ? _miny = src_y:0;
                        (src_y > _maxy) ? _maxy = src_y:0;

                        blob_coloring_id[count_step]=4;

                        goto recursive_entry;
return_entry4:

                        count_step--;
                }
                src_y++;
                src_image+=_c_src_step;
        }

        if(count_step==0)
                return;

        if(blob_coloring_id[count_step]==1)
        {
                goto return_entry1;
        }
        if(blob_coloring_id[count_step]==2)
        {
                goto return_entry2;
        }
        if(blob_coloring_id[count_step]==3)
        {
                goto return_entry3;
        }
        if(blob_coloring_id[count_step]==4)
        {
                goto return_entry4;
        }
}

bool overlapped(MBlobs b1, MBlobs b2)
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

int cleardeletedblobs(MBlobs MBlobsVTemp[],int size)
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
                //	MBlobsVTemp[i].BlobId,
                //	(MBlobsVTemp[i].MaxX-MBlobsVTemp[i].MinX),
                //	(MBlobsVTemp[i].MaxY-MBlobsVTemp[i].MinY));
        }

        delete []MBlobListTemp;

        return countblob;
}

int mer_overlapped(MBlobs MBlobsVTemp[],int size)
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

int BlobColoring(BYTE src[],MBlobs MBlobsVTemp[],int dimx,int dimy,int minarea,int maxarea,int size,int xsize,int ysize,
                                 int ratio,int merging)
{
        int BlobFinded=0;
        int counter=0;
        int status=0;

        _xmaxmargin=dimx;
        _ymaxmargin=dimy;
        _c_src_step=dimx;

        for(int i=_yminmargin; i<_ymaxmargin; i++, src+=_c_src_step)
        {
                for(int j=_xminmargin; j<_xmaxmargin; j++)
                {
                        if(src[j]>200)
                        {
                                _minx = _xmaxmargin;
                                _maxx = _xminmargin;
                                _miny = _ymaxmargin;
                                _maxy = _yminmargin;

                                _blob_size = 0;
                                
                                count_step=0;
                                src_image=src+j;
                                src_x=j;
                                src_y=i;
                                _blob_coloring();

                                if ((_blob_size > size))
                                {
                                        if (((_maxx - _minx) > xsize) && ((_maxy - _miny) > ysize))
                                        {
                                                int area_blob=(_maxx-_minx)*(_maxy-_miny);

                                                double result_ratio=(double)(_maxx-_minx)/(double)(_maxy-_miny);

                                                if(area_blob>minarea && area_blob<maxarea)
                                                {
                                                        if(ratio)
                                                        {
                                                                if((result_ratio>1.0) && (result_ratio<20.0))
                                                                {
                                                                        //printf("Blob %d (%d,%d,%3.3f)\n",
                                                                        //	counter,(_maxx-_minx),(_maxy-_miny),result_ratio);

                                                                        MBlobsVTemp[BlobFinded].MaxX=_maxx;
                                                                        MBlobsVTemp[BlobFinded].MaxY=_maxy;
                                                                        MBlobsVTemp[BlobFinded].MinX=_minx;
                                                                        MBlobsVTemp[BlobFinded].MinY=_miny;

                                                                        MBlobsVTemp[BlobFinded].BlobId = counter;

                                                                        counter++;
                                                                        BlobFinded++;
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //printf("Blob %d (%d,%d)\n",
                                                                //        counter,(_maxx-_minx),(_maxy-_miny));

                                                                MBlobsVTemp[BlobFinded].MaxX=_maxx;
                                                                MBlobsVTemp[BlobFinded].MaxY=_maxy;
                                                                MBlobsVTemp[BlobFinded].MinX=_minx;
                                                                MBlobsVTemp[BlobFinded].MinY=_miny;

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

int BlobColoring(unsigned char src[],unsigned char dst[])
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

double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

CvSeq* findSquares4( IplImage* img, CvMemStorage* storage )
{
    CvSeq* contours;
    int i, c, l, N = 11;
    CvSize sz = cvSize( img->width & -2, img->height & -2 );
    IplImage* timg = cvCloneImage( img ); // make a copy of input image
    IplImage* gray = cvCreateImage( sz, 8, 1 );
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

    // down-scale and upscale the image to filter out the noise
    cvPyrDown( timg, pyr, 7 );
    cvPyrUp( pyr, timg, 7 );
    tgray = cvCreateImage( sz, 8, 1 );

    // find squares in every color plane of the image
    for( c = 0; c < 3; c++ )
    {
        // extract the c-th color plane
        cvSetImageCOI( timg, c+1 );
        cvCopy( timg, tgray, 0 );

        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cvCanny( tgray, gray, 0, thresh, 3 );//not 5, faster with 3
                // dilate canny output to remove potential
                // holes between edge segments
                cvDilate( gray, gray, 0, 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }

            // find contours and store them all as a list
            cvFindContours( gray, storage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );

            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly( contours, sizeof(CvContour), storage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result->total == 4 &&
                    fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000 &&
                    cvCheckContourConvexity(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( result, i),
                            (CvPoint*)cvGetSeqElem( result, i-2),
                            (CvPoint*)cvGetSeqElem( result, i-1)));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( s < 0.3 )
                        for( i = 0; i < 4; i++ )
                            cvSeqPush( squares,
                                (CvPoint*)cvGetSeqElem( result, i));
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    }

    // release all the temporary images
    cvReleaseImage( &gray );
    cvReleaseImage( &pyr );
    cvReleaseImage( &tgray );
    cvReleaseImage( &timg );

    return squares;
}

void drawSquares( IplImage* img, CvSeq* squares )
{
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage( img );
    int i;

    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );

    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint* rect = pt;
        int count = 4;

        // read 4 vertices
        memcpy( pt, reader.ptr, squares->elem_size );
        CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
        memcpy( pt + 1, reader.ptr, squares->elem_size );
        CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
        memcpy( pt + 2, reader.ptr, squares->elem_size );
        CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
        memcpy( pt + 3, reader.ptr, squares->elem_size );
        CV_NEXT_SEQ_ELEM( squares->elem_size, reader );

        // draw the square as a closed polyline
        cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, 2 );
    }

    // show the resultant image
    cvShowImage("image",cpy);
    cvReleaseImage( &cpy );
}

int DistanceTransform(IplImage *img_in,IplImage *plate,QStringList &str_list)
{
        long t_start=cvGetTickCount();

        int k=0;

        IplImage* dist = 0;
        IplImage* dist8u1 = 0;
        IplImage* dist8u2 = 0;
        IplImage* dist8u = 0;
        IplImage* dist32s = 0;
        IplImage* gray = 0;
        IplImage* edge = 0;
        IplImage* img_out = 0;
        IplImage* image_blob=NULL;
        IplImage* image_blob_orig=NULL;
        IplImage* m_tmp8uC1 = NULL;

        m_tmp8uC1 = cvCreateImage(cvSize(img_in->width,img_in->height),IPL_DEPTH_8U,1);
        img_out = cvCreateImage( cvSize(img_in->width,img_in->height), IPL_DEPTH_8U, 3 );
        dist = cvCreateImage( cvSize(img_in->width,img_in->height), IPL_DEPTH_32F, 1 );
        dist8u = cvCreateImage( cvSize(img_in->width,img_in->height), IPL_DEPTH_8U, 1 );
        dist32s = cvCreateImage( cvSize(img_in->width,img_in->height), IPL_DEPTH_32S, 1 );

        img_out=cvCloneImage(img_in);

        IplImage *img_wb = cvCreateImage(cvSize(img_out->width,img_out->height),IPL_DEPTH_8U,3);

        unsigned char *img_wb_buffer=new unsigned char[img_out->width*img_out->height*3];
        unsigned char *img_out_buffer=new unsigned char[img_out->width*img_out->height*3];

        cvCvtColor(img_out, m_tmp8uC1, CV_RGB2GRAY);

        gray = cvCloneImage(m_tmp8uC1);
        edge = cvCloneImage( gray );
        dist8u1 = cvCloneImage( gray );
        dist8u2 = cvCloneImage( gray );

        BYTE *blob_image = new BYTE[(img_out->width*img_out->height*3)*16];

        storage = cvCreateMemStorage(0);

        CvSeq * rect_seq = findSquares4(img_out, storage);

        CvSeqReader reader;

        cvStartReadSeq( rect_seq, &reader, 0 );

        for(int i = 0; i < rect_seq->total; i += 4 )
        {
                CvPoint* rect = pt;

                // read 4 vertices
                memcpy( pt, reader.ptr, rect_seq->elem_size );
                CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
                memcpy( pt + 1, reader.ptr, rect_seq->elem_size );
                CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
                memcpy( pt + 2, reader.ptr, rect_seq->elem_size );
                CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );
                memcpy( pt + 3, reader.ptr, rect_seq->elem_size );
                CV_NEXT_SEQ_ELEM( rect_seq->elem_size, reader );

                int xmin=10000;
                int ymin=10000;
                int xmax=0;
                int ymax=0;

                for(int j=0;j<4;j++)
                {
                    if(pt[j].x<xmin)
                        xmin=pt[j].x;
                    if(pt[j].y<ymin)
                        ymin=pt[j].y;

                    if(pt[j].x>xmax)
                        xmax=pt[j].x;
                    if(pt[j].y>ymin)
                        ymax=pt[j].y;
                }

                int area=(xmax-xmin)*(ymax-ymin);

                double result_ratio=(double)(ymax-ymin)/(double)(xmax-xmin);

                if(!((result_ratio>0.2) && (result_ratio<0.3)) || area>100000 || area<1000)
                    continue;

                cvRectangle(img_in,cvPoint(xmin,ymin),cvPoint(xmax,ymax),CV_RGB(255,0,0), 2 );

                cvRectangle(img_out,cvPoint(xmin,ymin),cvPoint(xmax,ymax),CV_RGB(255,255,255), 2 );//clean margin noise

                int _minx=xmin;
                int _miny=ymin;
                int _maxx=xmax;
                int _maxy=ymax;

                image_blob = cvCreateImage(cvSize(abs(_maxx-_minx),abs(_maxy-_miny)),IPL_DEPTH_8U,1);

                image_blob_orig = cvCreateImage(cvSize(abs(_maxx-_minx),abs(_maxy-_miny)),IPL_DEPTH_8U,1);

                TrExtract_SubImage(img_out, image_blob_orig,_minx,_miny,_maxx,_maxy);

                image_blob = cvCreateImage(cvSize(abs(_maxx-_minx),abs(_maxy-_miny)),IPL_DEPTH_8U,3);

                TrExtract_SubImage(img_out, image_blob,_minx,_miny,_maxx,_maxy);

                char bmp_filename[256];

                IplImage* image_plate_inv = cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,1);
                IplImage* image_plate_2x = cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,1);
                IplImage* image_symbols = cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,1);
                IplImage* image_blob_2x = cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,3);
                IplImage* color_plate= cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,3);
                IplImage* image_symbols_inv = cvCreateImage(cvSize(2*abs(_maxx-_minx),2*abs(_maxy-_miny)),IPL_DEPTH_8U,1);

                memset(image_symbols->imageData,0,2*abs(_maxx-_minx)*2*abs(_maxy-_miny));

                cvResize(image_blob_orig,  image_plate_2x, CV_INTER_LINEAR );

                cvEqualizeHist(image_plate_2x,image_plate_2x);

                cvThreshold(image_plate_2x, image_plate_inv, plate_thresh, 255, CV_THRESH_BINARY);//VERY IMPORTANT

                cvThreshold(image_plate_inv, image_plate_2x, 0, 255, CV_THRESH_BINARY_INV);

                cvErode(image_plate_2x,image_plate_2x);
                cvErode(image_plate_2x,image_plate_2x);

                cvDilate(image_plate_2x,image_plate_2x);
                cvDilate(image_plate_2x,image_plate_2x);

                memcpy(blob_image, image_plate_2x->imageData, (image_plate_2x->width*image_plate_2x->height));

                //plate segmentation with blob coloring
                int symbol_number = BlobColoring((unsigned char *)blob_image,MBlobsFindedCharacter,image_plate_2x->width,
                        image_plate_2x->height,1000,(image_plate_2x->width*image_plate_2x->height),4,2,2,0,0);

                sprintf(bmp_filename,"./images/image.pbm");

                //find symbols
                for(k=0;k<symbol_number;k++)
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
                for(k=0;k<symbol_number;k++)
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

                symbol_number=counter;

                //ordering finded symbols
                counter=0;

                while(1)
                {
                        int min_value=1000;
                        int min_value_pos=0;

                        int ordered=1;

                        for(k=0;k<symbol_number;k++)
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
                for(k=0;k<symbol_number;k++)
                {
                        IplImage* image_symbol = cvCreateImage(cvSize(abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX),
                                abs(MBlobsFindedCharacter[k].MaxY-MBlobsFindedCharacter[k].MinY))
                                ,IPL_DEPTH_8U,1);

                        TrExtract_SubImage(image_plate_2x,image_symbol ,MBlobsFindedCharacter[k].MinX,
                                MBlobsFindedCharacter[k].MinY,
                                MBlobsFindedCharacter[k].MaxX,MBlobsFindedCharacter[k].MaxY);

                        TrPaste_SubImage(image_symbol,image_symbols,
                                space_symbols,10);

                        cvReleaseImage( &image_symbol );

                        space_symbols+=abs(MBlobsFindedCharacter[k].MaxX-MBlobsFindedCharacter[k].MinX)+10;
                }

                //ck for right symbol number
                if(symbol_number>symbol_min && symbol_number<symbol_max)
                {
                    cvThreshold(image_symbols,image_symbols_inv , 1, 255, CV_THRESH_BINARY_INV);

                    //cvErode(image_symbols_inv,image_symbols_inv);
                    //cvDilate(image_symbols_inv,image_symbols_inv);

                    cvCvtColor(image_symbols_inv, color_plate, CV_GRAY2RGB);

                    cvResize(color_plate,plate,CV_INTER_LINEAR);

                    //OCR
                    char cmdline[1024];
                    char platevalue[1024];
//#ifdef WIN32

                    int counter_chars=0;
                    int ocrerror=0;

                    char plate_str_inv[256];

                    plate_str_inv[0]='\0';

                    char plate_str[256];

                    plate_str[0]='\0';

                    tAPI->SetImage((const unsigned char *)image_symbols_inv->imageData,
                                   image_symbols_inv->width,
                                   image_symbols_inv->height,
                                   1,image_symbols_inv->width);

                    char* outText=tAPI->GetUTF8Text();

                    //printf("Plate: %s",outText);

                    int j=0;
                    //int nt=0;

                    for(int i=0 ;i<strlen(outText);i++)
                    {
                        if(outText[i]!='\n')
                        {
                            if(outText[i+1]=='\n')
                            {
                                plate_str_inv[j]=outText[i];
                                j++;
                            }
                            else
                            {
                                plate_str_inv[j]='#';
                                j++;
                                char val='\0';
                                do
                                {
                                   val=outText[i];
                                   i++;
                                } while(val!='\n');
                            }
                        }
                    }

                    plate_str_inv[j+1]='\0';

                    j--;

                    for(int i=0;i<strlen(plate_str_inv);i++)
                    {
                        plate_str[i]=plate_str_inv[j];
                        j--;
                    }

#ifdef INDIAN
                    plate_str[10]='\0';
#endif

#ifdef ITALY
                    plate_str[7]='\0';
#endif

                    int cc=strlen(plate_str);

                    char c;
                    int counter_cc=0;
#ifdef ITALY
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
#endif

#ifdef INDIAN
                    while(counter_cc<cc)
                    {
                        c=plate_str[counter_cc];

                        if(counter_cc<2)
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
                        else
                        if(counter_cc>=2 && counter_cc<4)
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
                        else
                        if (counter_cc>=4 && counter_cc<6)
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
                        else
                        if (counter_cc>=6)
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
#endif

                    platevalue[counter_chars]='\0';

/*#else
#ifdef LEARN_OCR
                    sprintf(cmdline,"gocr -m 130 %s",bmp_filename);//learn
#endif
#ifndef LEARN_OCR
                    sprintf(cmdline,"gocr -m 2 -a 96 %s",bmp_filename);
#endif
                    int counter_chars=0;

                    printf("plate:");

                    FILE *platechars = popen(cmdline,"r");

                    if(platechars!=NULL)
                    {
#ifdef ITALY
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
#endif

                        pclose(platechars);
                    }

                    printf("\n");
#endif*/
                    //printf("Plate correct %s,%d\n",platevalue,counter_chars);

                    if(counter_chars==symbol_characters)
                    {
                        printf("Plate correct %s is the %d finded rect with area:%d,ratio:%f\n",platevalue,i/4,area,result_ratio);

                        str_list.append((QString)platevalue);

                        cvRectangle(img_in,
                             cvPoint(_minx,_miny),
                             cvPoint(_maxx,_maxy),
                             CV_RGB(255,0,0), 3 );

                        for(k=0;k<symbol_number;k++)
                        {
                            cvRectangle(img_in,
                                 cvPoint(MBlobsFindedCharacter[k].MinX/2+_minx,MBlobsFindedCharacter[k].MinY/2+_miny),
                                 cvPoint(MBlobsFindedCharacter[k].MaxX/2+_minx,MBlobsFindedCharacter[k].MaxY/2+_miny),
                                 CV_RGB(255,0,0), 2 );
                        }

                        cvReleaseImage( &image_plate_inv);
                        cvReleaseImage( &image_plate_2x );
                        cvReleaseImage( &image_symbols );
                        cvReleaseImage( &image_blob_2x );
                        cvReleaseImage( &color_plate);
                        cvReleaseImage( &image_blob );
                        cvReleaseImage( &image_blob_orig );
                        cvReleaseImage( &image_symbols_inv);

                        plate_good_for_ocr++;

                        cvClearMemStorage( storage );
                        cvReleaseMemStorage(&storage);

                        cvReleaseImage( &img_out );
                        cvReleaseImage( &gray );
                        cvReleaseImage( &edge );
                        cvReleaseImage( &dist );
                        cvReleaseImage( &dist8u );
                        cvReleaseImage( &dist8u1 );
                        cvReleaseImage( &dist8u2 );
                        cvReleaseImage( &dist32s );
                        cvReleaseImage(	&m_tmp8uC1 );
                        cvReleaseImage( &img_wb );

                        delete []img_wb_buffer;
                        delete []img_out_buffer;
                        delete []blob_image;

                        fflush(stdout);
                        
                        return 1;
                    }
                }
                else
                {
                    //printf("Unable to see plate chars\n");
                    //fflush(stdout);
                }

                cvReleaseImage( &image_plate_inv);
                cvReleaseImage( &image_plate_2x );
                cvReleaseImage( &image_symbols );
                cvReleaseImage( &image_blob_2x );
                cvReleaseImage( &color_plate);
                cvReleaseImage( &image_blob );
                cvReleaseImage( &image_blob_orig );
                cvReleaseImage( &image_symbols_inv);
        }

        cvClearMemStorage( storage );
        cvReleaseMemStorage(&storage);

        cvReleaseImage( &img_out );
        cvReleaseImage( &gray );
        cvReleaseImage( &edge );
        cvReleaseImage( &dist );
        cvReleaseImage( &dist8u );
        cvReleaseImage( &dist8u1 );
        cvReleaseImage( &dist8u2 );
        cvReleaseImage( &dist32s );
        cvReleaseImage(	&m_tmp8uC1 );

        delete []blob_image;

        cvReleaseImage(&img_wb);
        delete []img_wb_buffer;
        delete []img_out_buffer;

        fflush(stdout);

        long t_stop=cvGetTickCount();

        detection_time=(t_stop-t_start)/(double)(cvGetTickFrequency()*1000.0);

        return 0;
}

int plate_detect(char *filename,int mode)
{
        if(plate_detect_flag==1)
            return -1;

        plate_detect_flag=1;

        tAPI = new tesseract::TessBaseAPI();

        printf("Tesseract-ocr version: %s\n",
               tesseract::TessBaseAPI::Version());

        printf("Leptonica version: %s\n",
               getLeptonicaVersion());

        int rc = tAPI->Init(NULL, NULL, tesseract::OEM_DEFAULT, NULL, 0, NULL, NULL, false);

        if (rc) {
          fprintf(stderr, "Could not initialize tesseract.\n");
          exit(1);
        }

        tAPI->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK_VERT_TEXT);

        tAPI->SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

        CvCapture* capture = 0;
        IplImage* frame = 0;
        IplImage* frame_reduced=0;
        IplImage* frame_gray = 0;
        IplImage* frame_img = 0;
        IplImage* plate=0;
        char plate_str[256];

        if(mode==1)
        {
            frame=cvCreateImage(cvSize(704,576),IPL_DEPTH_8U,3);
        }

        frame_reduced=cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
        frame_img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
        plate=cvCreateImage(cvSize(631,201),IPL_DEPTH_8U,3);

        if(mode==1)
        {
            printf("Connecting to %s,%d....... ",ip_addr,ip_port);
            fflush(stdout);

            skt_mjpeg=open_mjpeg_stream(ip_addr,ip_port,3,0);

            if(skt_mjpeg<=0)
            {
                printf("unable to connect\n");
                fflush(stdout);
                return -1;
            }

            printf("Connected!\n");
        }
        else if(mode==0)
        {
            capture = cvCaptureFromAVI(filename);

            if (capture==NULL)
            {
                printf("unable to open file avi\n");
                return -1;
                fflush(stdout);
            }

            printf("file opened!\n");
            fflush(stdout);
        }
        else if(mode==2)
        {
            capture = cvCaptureFromCAM(-1);

            if(capture==NULL)
            {
                qDebug("unable to open camera");
                fflush(stdout);
                return -1;
            }
        }

	int counter_rec=0;
        int counter_day_rec=0;
        int YellowElmet=0;

#ifdef MUIN
        muin tmp_muin;
	
	int ret_val = tmp_muin.Open(MODEMDEVICE,BAUDRATE);
	
	if(ret_val==0)
	{
		printf("Muin Opened!\n");
	}
	else
	{
		printf("Unable to open Muin! Check connections and baudrate\n");
		return -1;
	}
	
        tmp_muin.SetChannel(muin_chan);
#endif

	//wait for activation

        while(plate_detect_flag)
	{
            //reset

#ifdef MUIN
            tmp_muin.SetPortB(0x0);
#endif

            counter_rec=0;

            QDateTime start_time = QDateTime::currentDateTime();
            int diff_time=0;

            int frame_size;

            for(;;)
            {
                if(mode==1)
                {
                    int width;
                    int height;

                    frame_size = receive_mjpeg_stream(skt_mjpeg,(unsigned char *)yuv_image,width,height,0);

                    if(frame_size==width*height*2.0)
                    {
                        yuv2rgb_24(yuv_image,width,
                                    &(yuv_image[width*height]),
                                    &(yuv_image[(width*height)+((width*height)/2)]),
                                    (width),(unsigned char *)frame->imageData,width,height);
                    }
                    else
                    {
                        yuv2rgb_24(yuv_image,width,
                                    &(yuv_image[width*height]),
                                    &(yuv_image[(width*height)+((width*height)/4)]),
                                    (width/2),(unsigned char *)frame->imageData,width,height);
                    }

                    VertFlipBuf((unsigned char *)frame->imageData,(unsigned char *)frame->imageData,
                                              3*width,height);
                }
                else if(mode==0 || mode==2)
                {
                    frame = cvQueryFrame( capture );
                }

                if(frame)
                {
                    QStringList plate_str_list;

                    int ret_val=DistanceTransform(frame,plate,plate_str_list);

                    if(ret_val==1)
                    {
                        while(plate_str_list.count()!=0)
                        {
                            QString str = plate_str_list.takeFirst();

                            strcpy(plate_str,str.toAscii().data());

                            strcpy(best_plate,plate_str);
                        }
                    }

                    cvResize(frame,frame_img);

                    memcpy(frame_image,frame_img->imageData,640*480*3);

                    memcpy(histogram_image,plate->imageData,631*201*3);

                    QDateTime stop_time = QDateTime::currentDateTime();

                    diff_time = start_time.secsTo(stop_time);
                }
                else
                {
                    plate_detect_flag=false;
                    break;
                }

                if(!plate_detect_flag)
                    break;
            }
	}
	
        printf("Close Thread\n");
        fflush(stdout);

        cvReleaseImage(&frame_reduced);

        cvReleaseImage(&plate);

        if(mode==0 || mode==2)
        {
            cvReleaseCapture( &capture );
        }
        else if(mode==1)
        {
            close_mjpeg_encoder(skt_mjpeg);
            cvReleaseImage(&frame);
        }

#ifdef MUIN
        tmp_muin.Close();
#endif

        tAPI->Clear();
        tAPI->End();

	return 0;
}

int get_image(unsigned char *img,int dimx,int dimy)
{
    Rgb2Gbr(frame_image,img,dimx,dimy);
    return 0;
}

int get_histogram(unsigned char *img,int dimx,int dimy)
{
    Rgb2Gbr(histogram_image,img,dimx,dimy);
    return 0;
}

int get_image_list(QList<QString> &img_list)
{
    img_list=image_list;
    image_list.clear();
    return 0;
}

int get_plate_list(QList<QString> &plt_list)
{
    plt_list=plate_list;
    plate_list.clear();
    return plate_finded;
}

int stop_plate_detect()
{
    plate_detect_flag=0;
    return 0;
}

int set_ip_address(QString ip_address,int port,int muin_chn)
{
    strcpy(ip_addr,ip_address.toAscii().data());
    ip_port=port;
    muin_chan=muin_chn;

    return 0;
}

int set_params(QString classifier,int sensibility,int rec_th,int alert_th,int alarm_th)
{
    strcpy(class_file,classifier.toAscii().data());

    plate_thresh=sensibility;

    recognition_frame_numb=rec_th;

    alert_frame_numb=-alert_th;
    alarm_frame_numb=-alarm_th;

    return 0;
}

int set_best_plate(char *value)
{
    strncpy(best_plate,value,256);
    return 0;
}

int get_best_plate(char *value)
{
    strncpy(value,best_plate,256);
    return 0;
}

double get_time()
{
    return detection_time;
}

int set_plate_mask(int red,int green, int blue)
{
    blue_thresh=blue;
    green_thresh=green;
    red_thresh=red;
    return 0;
}

int set_plate_chr(int min,int max,int numb)
{
    symbol_min=min;
    symbol_max=max;
    symbol_characters=numb;
    return 0;
}

