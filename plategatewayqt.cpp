////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//plategatewayqt.cpp
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

#include "plategatewayqt.h"
#include "ui_plategatewayqt.h"
#include "plategateway.h"
#include "setupdialog.h"
#include "plates.h"
#include "muin.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

PlateGatewayQt::PlateGatewayQt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlateGatewayQt)
{
    ui->setupUi(this);

    rv_hlp = new RenderVideoHelper();
    rh_hlp = new RenderHistogramHelper();
    fv_hlp = new PlateValidationHelper();

    rv_widget = new Widget_RenderVideo(rv_hlp,ui->render_video_widget);

    rh_widget = new Widget_RenderHistogram(rh_hlp,ui->render_histogram_widget);

    render_timer=new QTimer(this);
    update_timer=new QTimer(this);
    update_database_timer=new QTimer(this);
    update_plate_timer=new QTimer(this);

    connect(render_timer,SIGNAL(timeout()),this,SLOT(update_screen()));

    render_timer->start(40);
    update_timer->start(1000);
    update_database_timer->start(30000);
    update_plate_timer->start(100);

    rv_widget->Go();
    rh_widget->Go();

    counter_item=0;

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    ui->statusBar->showMessage("Waiting...",0);

    QFile file("./config.xml");

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
       printf("unable to open config file\n");
       fflush(stdout);
    }


    Rxml.setDevice(&file);
    Rxml.readNext();

    Rxml.readNextStartElement();
    Rxml.readNextStartElement();

    while(!Rxml.atEnd())
    {
        if(Rxml.isStartElement())
        {
            if(Rxml.name()=="")
            {

            }
            else if(Rxml.name() == "Connection")
            {
                while(!Rxml.atEnd())
                {
                    if(Rxml.isEndElement())
                    {
                            Rxml.readNext();
                            break;
                    }
                    else if(Rxml.isCharacters())
                    {
                            Rxml.readNext();
                    }
                    else if(Rxml.isStartElement())
                    {
                            QString str;
                            bool ok;
                            if(Rxml.name() == "Ip")
                            {
                               get_element_xml(str);
                               ip_address = str;
                            }
                            else if(Rxml.name() == "Port")
                            {
                                get_element_xml(str);
                                ip_port=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "MuinPort")
                            {
                                get_element_xml(str);
                                muin_port=str.toUInt(&ok,10);
                            }

                            Rxml.readNext();
                    }
                    else
                    {
                            Rxml.readNext();
                    }
                }
            }
            else if(Rxml.name() == "Classifier")
            {
                while(!Rxml.atEnd())
                {
                    if(Rxml.isEndElement())
                    {
                            Rxml.readNext();
                            break;
                    }
                    else if(Rxml.isCharacters())
                    {
                            Rxml.readNext();
                    }
                    else if(Rxml.isStartElement())
                    {
                            QString str;
                            bool ok;

                            if(Rxml.name() == "Sensibility")
                            {
                                get_element_xml(str);
                                sensibility=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "Red")
                            {
                                get_element_xml(str);
                                red=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "Green")
                            {
                                get_element_xml(str);
                                green=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "Blue")
                            {
                                get_element_xml(str);
                                blue=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "CR_min")
                            {
                                get_element_xml(str);
                                cr_min=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name() == "CR_max")
                            {
                                get_element_xml(str);
                                cr_max=str.toUInt(&ok,10);
                            }
                            else if(Rxml.name()== "PC_number")
                            {
                                get_element_xml(str);
                                pc_number=str.toUInt(&ok,10);
                            }

                            Rxml.readNext();
                    }
                    else
                    {
                            Rxml.readNext();
                    }
                }
            }
            else if(Rxml.name() == "PlateRec")
            {
                while(!Rxml.atEnd())
                {
                    if(Rxml.isEndElement())
                    {
                            Rxml.readNext();
                            break;
                    }
                    else if(Rxml.isCharacters())
                    {
                            Rxml.readNext();
                    }
                    else if(Rxml.isStartElement())
                    {
                            QString str;
                            bool ok;

                            if(Rxml.name() == "DbaseLen")
                            {
                                get_element_xml(str);
                                dbase_len=str.toUInt(&ok,10);
                            }

                            Rxml.readNext();
                    }
                    else
                    {
                            Rxml.readNext();
                    }
                }
            }
            else
            {
              Rxml.raiseError(QObject::tr("Not a bookindex file"));
            }
        }
        else
        {
            Rxml.readNext();
        }
    }

    file.close();

    set_ip_address(ip_address,ip_port,muin_port);

    set_params(classifier_file,sensibility,th_recon,th_alert,th_alarm);

    set_plate_mask(red,green,blue);

    set_plate_chr(cr_min,cr_max,pc_number);

    //update_list();

    row_counter=0;

    ui->TimeStr->setText("0.0 ms");

    char *chrs = new char[256];
    memset(chrs,0L,256);
    set_best_plate(chrs);
    delete []chrs;
}

PlateGatewayQt::~PlateGatewayQt()
{
    delete ui;
}

int PlateGatewayQt::get_element_xml(QString &str)
{
    while(!Rxml.atEnd())
    {
        if(Rxml.isEndElement())
        {
                Rxml.readNext();
                break;
        }
        else if(Rxml.isStartElement())
        {
                str=Rxml.readElementText();
                break;

        }
        else if(Rxml.isCharacters())
        {
                Rxml.readNext();
        }
        else
        {
                Rxml.readNext();
        }
    }
}

void PlateGatewayQt::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PlateGatewayQt::on_StartButton_clicked()
{
    mode=1;
    fv_hlp->start_plate_validation(filename,mode);
}

void PlateGatewayQt::on_StopButton_clicked()
{
    fv_hlp->stop_plate_validation();
}

void PlateGatewayQt::click_image(QListWidgetItem *item)
{
    QString tmp_str=item->text();

    char tmp_string[256];

    strcpy(tmp_string,tmp_str.toAscii().data());

    int year;
    int month;
    int day;
    int hr;
    int min;
    int sec;
    int type;
    char tmp_plate[1024];

    printf("%s\n",tmp_string);

    sscanf(tmp_string,"%d/%d/%d %d:%d:%d %s",&year,&month,&day,&hr,&min,&sec,tmp_plate);

    QColor clr = item->backgroundColor();

    if(clr.red()==255 && clr.green()==0 && clr.blue()==0)
    {
        type=3;
    }

    if(clr.red()==255 && clr.green()==255 && clr.blue()==0)
    {
        type=2;
    }

    if(clr.red()==0 && clr.green()==255 && clr.blue()==0)
    {
        type=1;
    }

    if(clr.red()==255 && clr.green()==255 && clr.blue()==255)
    {
        type=0;
    }

    char filename[256];

#ifndef WIN32
    sprintf(filename,"/usr/bin/eog ./image_db/%d_%02d_%02d_%02d_%02d_%02d_%d_%s.jpeg &",year,month,day,hr,min,sec,type,tmp_plate);

    printf("%s\n",filename);
    fflush(stdout);

    system(filename);
#endif
}

void PlateGatewayQt::update_screen()
{
    rv_widget->animate(ui->render_video_widget->width(),ui->render_video_widget->height());
    rh_widget->animate(ui->render_histogram_widget->width(),ui->render_histogram_widget->height());

    char *bp=new char[256];
    memset(bp,0L,256);
    get_best_plate(bp);

    ui->StrPlate->setText((QString)bp);

    delete []bp;

    double dts_val=get_time();
    char *ts=new char[256];
    sprintf(ts,"%.2f ms",dts_val);
    ui->TimeStr->setText(ts);
    delete []ts;
}

//RenderVideoHelper

RenderVideoHelper::RenderVideoHelper()
{
    vid_image_data_yuv = new unsigned char[1920*1200*2];
    vid_image_data_rgb = new unsigned char[1920*1200*4];

    image_width=640;
    image_height=480;
}

RenderVideoHelper::~RenderVideoHelper()
{
    delete []vid_image_data_yuv;
    delete []vid_image_data_rgb;
}

void RenderVideoHelper::paint(QPainter *painter, QPaintEvent *event, int elapsed,int width,int height)
{
    vid_image=QImage(vid_image_data_rgb,image_width,image_height,QImage::Format_RGB888);
    //vid_image=vid_image.mirrored(false,true);
    painter->drawImage(QRect(0,0,width,height),vid_image,QRect(0,0,image_width,image_height),Qt::AutoColor);
}

void RenderVideoHelper::run()
{
    thread_active=true;

    while(thread_active)
    {
        get_image(vid_image_data_rgb,640,480);
        usleep(40000);
    }
}

int RenderVideoHelper::start_rendering()
{
    start();
    return 0;
}

//RenderHistogramHelper

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
        get_histogram(vid_image_data_rgb,631,201);
        usleep(250000);
    }
}

int RenderHistogramHelper::start_rendering()
{
    start();
    return 0;
}

//RenderHistogramHelper

PlateValidationHelper::PlateValidationHelper()
{
    thread_active=false;
}

PlateValidationHelper::~PlateValidationHelper()
{

}

void PlateValidationHelper::run()
{
    plate_detect(filename.toAscii().data(),mode);
}

int PlateValidationHelper::start_plate_validation(QString file_name,int md)
{
    filename=file_name;
    mode=md;

    if(!thread_active)
    {
        start();
    }

    return 0;
}

int PlateValidationHelper::stop_plate_validation()
{
    thread_active=false;
    stop_plate_detect();
}

//GLWidget_RenderVideo

Widget_RenderVideo::Widget_RenderVideo(RenderVideoHelper *helper, QWidget *parent)
        : QWidget(parent), helper(helper)
{
    elapsed = 0;
    Widget_RenderVideo::setMinimumSize(QSize(1920,1200));
}

void Widget_RenderVideo::animate(int tmp_width,int tmp_height)
{
    elapsed += 10;
    elapsed = elapsed % 1000;
    width=tmp_width;
    height=tmp_height;
    repaint();
}

void Widget_RenderVideo::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    //painter.setRenderHint(painter.Antialiasing,true);
    helper->paint(&painter, event, elapsed,width,height);
    painter.end();
}

void Widget_RenderVideo::closeEvent(QCloseEvent *event)
{

}

void Widget_RenderVideo::Go()
{
    helper->start_rendering();
}

Widget_RenderVideo::~Widget_RenderVideo()
{

}

//GLWidget_RenderHistogram

Widget_RenderHistogram::Widget_RenderHistogram(RenderHistogramHelper *helper, QWidget *parent)
        : QWidget(parent), helper(helper)
{
    elapsed = 0;
    Widget_RenderHistogram::setMinimumSize(QSize(1920,1200));
}

void Widget_RenderHistogram::animate(int tmp_width,int tmp_height)
{
    elapsed += 10;
    elapsed = elapsed % 1000;
    width=tmp_width;
    height=tmp_height;
    repaint();
}

void Widget_RenderHistogram::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    //painter.setRenderHint(painter.Antialiasing,true);
    helper->paint(&painter, event, elapsed,width,height);
    painter.end();
}

void Widget_RenderHistogram::closeEvent(QCloseEvent *event)
{

}

void Widget_RenderHistogram::Go()
{
    helper->start_rendering();
}

Widget_RenderHistogram::~Widget_RenderHistogram()
{
    
}

void PlateGatewayQt::on_SetupButton_clicked()
{
   SetupDialog setup_dialog;

   setup_dialog.ip_address=ip_address;
   setup_dialog.ip_port=ip_port;
   setup_dialog.muin_port=muin_port;
   setup_dialog.classifier_file=classifier_file;
   setup_dialog.sensibility=sensibility;
   setup_dialog.th_recon=th_recon;
   setup_dialog.th_alert=th_alert;
   setup_dialog.th_alarm=th_alarm;
   setup_dialog.dbase_len=dbase_len;
   setup_dialog.red=red;
   setup_dialog.green=green;
   setup_dialog.blue=blue;
   setup_dialog.cr_min=cr_min;
   setup_dialog.cr_max=cr_max;
   setup_dialog.pc_number=pc_number;

   int ret_val = setup_dialog.exec();

   if(ret_val==QDialog::Accepted)
   {

       ip_address=setup_dialog.ip_address;
       ip_port=setup_dialog.ip_port;
       muin_port=setup_dialog.muin_port;
       classifier_file=setup_dialog.classifier_file;
       sensibility=setup_dialog.sensibility;
       th_recon=setup_dialog.th_recon;
       th_alert=setup_dialog.th_alert;
       th_alarm=setup_dialog.th_alarm;
       dbase_len=setup_dialog.dbase_len;

       red=setup_dialog.red;
       green=setup_dialog.green;
       blue=setup_dialog.blue;

       cr_min=setup_dialog.cr_min;
       cr_max=setup_dialog.cr_max;
       pc_number=setup_dialog.pc_number;

       QFile file("./config.xml");
       file.open(QIODevice::WriteOnly);

       QXmlStreamWriter xmlWriter(&file);
       xmlWriter.setAutoFormatting(true);
       xmlWriter.writeStartDocument();

       xmlWriter.writeStartElement("FaceRecognition");

       xmlWriter.writeStartElement("Connection");
       xmlWriter.writeTextElement("Ip", setup_dialog.ip_address);
       xmlWriter.writeTextElement("Port", QString::number(setup_dialog.ip_port,10));
       xmlWriter.writeTextElement("MuinPort", QString::number(setup_dialog.muin_port,10));
       xmlWriter.writeEndElement();

       xmlWriter.writeStartElement("Classifier");

       xmlWriter.writeTextElement("Sensibility",QString::number(setup_dialog.sensibility,10));
       xmlWriter.writeTextElement("Red",QString::number(setup_dialog.red,10));
       xmlWriter.writeTextElement("Green",QString::number(setup_dialog.green,10));
       xmlWriter.writeTextElement("Blue",QString::number(setup_dialog.blue,10));
       xmlWriter.writeTextElement("CR_min",QString::number(setup_dialog.cr_min,10));
       xmlWriter.writeTextElement("CR_max",QString::number(setup_dialog.cr_max,10));
       xmlWriter.writeTextElement("PC_number",QString::number(setup_dialog.pc_number,10));

       xmlWriter.writeStartElement("PlateRec");
       xmlWriter.writeTextElement("DbaseLen",QString::number(setup_dialog.dbase_len,10));
       xmlWriter.writeEndElement();

       xmlWriter.writeEndElement();
       xmlWriter.writeEndDocument();

       file.close();

       set_ip_address(ip_address,ip_port,muin_port);

       set_params(classifier_file,sensibility,th_recon,th_alert,th_alarm);

       set_plate_mask(red,green,blue);

       set_plate_chr(cr_min,cr_max,pc_number);

   }
}

void PlateGatewayQt::on_loadButton_clicked()
{
    QFileDialog dlg;

    if(dlg.exec())
    {
        filename = dlg.selectedFiles().at(0);
        mode=0;
        printf("Open filename %s\n",filename.toAscii().data());
        fflush(stdout);
    }
}

void PlateGatewayQt::on_playButton_clicked()
{
    if(mode==0)
    {
        fv_hlp->start_plate_validation(filename,mode);
    }
}

void PlateGatewayQt::on_stopButton_clicked()
{
    if(mode==0)
    {
        fv_hlp->stop_plate_validation();
    }
}

void PlateGatewayQt::on_AccessButton_clicked()
{
    Plates plates;
    plates.exec();
}

void PlateGatewayQt::on_WCStopButton_clicked()
{
    if(mode==2)
    {
        fv_hlp->stop_plate_validation();
        mode=0;
    }
}

void PlateGatewayQt::on_WCStartButton_clicked()
{
    mode=2;
    fv_hlp->start_plate_validation(filename,mode);
    qDebug()<<"start";
}
