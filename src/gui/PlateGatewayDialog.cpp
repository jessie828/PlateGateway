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

#include "PlateGatewayDialog.h"

PlateGatewayDialog::PlateGatewayDialog(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    //disable things i wont implement just yet. maybe get out entirely
    StartButton->setEnabled(false);
    StopButton->setEnabled(false);
    WCStartButton->setEnabled(false);
    WCStopButton->setEnabled(false);
    SetupButton->setEnabled(false);
    label_8->setVisible(false);
    label_3->setVisible(false);
    StartButton->setVisible(false);
    StopButton->setVisible(false);
    WCStartButton->setVisible(false);
    WCStopButton->setVisible(false);
    SetupButton->setVisible(false);
    //end disable

    m_renderVideoHelper = new RenderVideoHelper();
    m_plateValidateHelper = new PlateValidationHelper();
//    rh_hlp = new RenderHistogramHelper();


    renderVideoWidget = new RenderVideo(render_video_widget);
    thumbnailLabel = new QLabel(render_video_widget);
    thumbnailLabel->setText("Waiting for loading of video ...");
    thumbnailLabel->setGeometry(render_video_widget->geometry().center().x()/2, render_video_widget->geometry().center().y()/2, 500, 50);

//    rh_widget = new Widget_RenderHistogram(rh_hlp,m_plateGateway->render_histogram_widget);

//    render_timer=new QTimer(this);
//    update_timer=new QTimer(this);
//    update_database_timer=new QTimer(this);
//    update_plate_timer=new QTimer(this);

//    connect(render_timer,SIGNAL(timeout()),this,SLOT(update_screen()));
//
//    render_timer->start(40);
//    update_timer->start(1000);
//    update_database_timer->start(30000);
//    update_plate_timer->start(100);

//    rv_widget->Go();
    renderVideoWidget->Go();
//    rh_widget->Go();

    mainStatusBar->showMessage("Waiting...",0);
    TimeStr->setText("0.0 ms");

    char *chrs = new char[256];
    memset(chrs,0L,256);
    set_best_plate(chrs);
    delete []chrs;
}


PlateGatewayDialog::~PlateGatewayDialog()
{
}


//void PlateGatewayQt::click_image(QListWidgetItem *item)
//{
//    QString tmp_str=item->text();
//
//    char tmp_string[256];
//
//    strcpy(tmp_string,tmp_str.toAscii().data());
//
//    int year;
//    int month;
//    int day;
//    int hr;
//    int min;
//    int sec;
//    int type;
//    char tmp_plate[1024];
//
//    printf("%s\n",tmp_string);
//
//    sscanf(tmp_string,"%d/%d/%d %d:%d:%d %s",&year,&month,&day,&hr,&min,&sec,tmp_plate);
//
//    QColor clr = item->backgroundColor();
//
//    if(clr.red()==255 && clr.green()==0 && clr.blue()==0)
//    {
//        type=3;
//    }
//
//    if(clr.red()==255 && clr.green()==255 && clr.blue()==0)
//    {
//        type=2;
//    }
//
//    if(clr.red()==0 && clr.green()==255 && clr.blue()==0)
//    {
//        type=1;
//    }
//
//    if(clr.red()==255 && clr.green()==255 && clr.blue()==255)
//    {
//        type=0;
//    }
//
//    char filename[256];
//
//    sprintf(filename,"/usr/bin/eog ./image_db/%d_%02d_%02d_%02d_%02d_%02d_%d_%s.jpeg &",year,month,day,hr,min,sec,type,tmp_plate);
//
//    printf("%s\n",filename);
//    fflush(stdout);
//
//    system(filename);
//}
//
//void PlateGatewayQt::update_screen()
//{
//    rv_widget->animate(m_plateGateway->render_video_widget->width(),m_plateGateway->render_video_widget->height());
//    rh_widget->animate(m_plateGateway->render_histogram_widget->width(),m_plateGateway->render_histogram_widget->height());
//
//    char *bp=new char[256];
//    memset(bp,0L,256);
//    get_best_plate(bp);
//
//    m_plateGateway->StrPlate->setText((QString)bp);
//
//    delete []bp;
//
//    double dts_val=get_time();
//    char *ts=new char[256];
//    sprintf(ts,"%.2f ms",dts_val);
//    m_plateGateway->TimeStr->setText(ts);
//    delete []ts;
//}


void PlateGatewayDialog::on_loadButton_clicked()
{
    QFileDialog dlg;
    dlg.setFilter("*.avi *.mov *.MOV");

    if(dlg.exec())
    {
        m_filename = dlg.selectedFiles().at(0);
        mainStatusBar->showMessage(QString("Video %1 loaded ...").arg(m_filename), 0);
    }

    showThumbnail(m_filename);
}


void PlateGatewayDialog::on_playButton_clicked()
{
    if(!m_filename.isEmpty())
    {
        m_plateValidateHelper->startPlateValidation(m_filename);
    }
    else
    {
        mainStatusBar->showMessage("Please select a file before playing", 0);
    }
}


void PlateGatewayDialog::on_stopButton_clicked()
{
//    m_plateValidateHelper->stop_plate_validation();
    mainStatusBar->showMessage("Stop button pressed. Will now stop retrieving license plates ...");
}


void PlateGatewayDialog::showThumbnail(const QString &filename)
{
    QPixmap thumbnail = m_plateValidateHelper->getThumbnail(filename);
    thumbnailLabel->setBackgroundRole(QPalette::Base);
    thumbnailLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    thumbnailLabel->setScaledContents(true);
    thumbnailLabel->setGeometry(QRect(0,0, render_video_widget->width(), render_video_widget->height()));
    thumbnailLabel->setPixmap(thumbnail);
    thumbnailLabel->resize(render_video_widget->size());
    thumbnailLabel->setScaledContents(true);
    thumbnailLabel->setVisible(true);
}


//void PlateGatewayQt::on_AccessButton_clicked()
//{
//    Plates plates;
//    plates.exec();
//}
