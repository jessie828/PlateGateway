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

//    rv_hlp = new RenderVideoHelper();
//    rh_hlp = new RenderHistogramHelper();
    fv_hlp = new PlateValidationHelper();

//    rv_widget = new Widget_RenderVideo(rv_hlp,m_plateGateway->render_video_widget);
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


//void PlateGatewayQt::on_SetupButton_clicked()
//{
//   SetupDialog setup_dialog;
//
//   setup_dialog.ip_address=ip_address;
//   setup_dialog.ip_port=ip_port;
//   setup_dialog.muin_port=muin_port;
//   setup_dialog.classifier_file=classifier_file;
//   setup_dialog.sensibility=sensibility;
//   setup_dialog.th_recon=th_recon;
//   setup_dialog.th_alert=th_alert;
//   setup_dialog.th_alarm=th_alarm;
//   setup_dialog.dbase_len=dbase_len;
//   setup_dialog.red=red;
//   setup_dialog.green=green;
//   setup_dialog.blue=blue;
//   setup_dialog.cr_min=cr_min;
//   setup_dialog.cr_max=cr_max;
//   setup_dialog.pc_number=pc_number;
//
//   int ret_val = setup_dialog.exec();
//
//   if(ret_val==QDialog::Accepted)
//   {
//
//       ip_address=setup_dialog.ip_address;
//       ip_port=setup_dialog.ip_port;
//       muin_port=setup_dialog.muin_port;
//       classifier_file=setup_dialog.classifier_file;
//       sensibility=setup_dialog.sensibility;
//       th_recon=setup_dialog.th_recon;
//       th_alert=setup_dialog.th_alert;
//       th_alarm=setup_dialog.th_alarm;
//       dbase_len=setup_dialog.dbase_len;
//
//       red=setup_dialog.red;
//       green=setup_dialog.green;
//       blue=setup_dialog.blue;
//
//       cr_min=setup_dialog.cr_min;
//       cr_max=setup_dialog.cr_max;
//       pc_number=setup_dialog.pc_number;
//
//       QFile file("./config.xml");
//       file.open(QIODevice::WriteOnly);
//
//       QXmlStreamWriter xmlWriter(&file);
//       xmlWriter.setAutoFormatting(true);
//       xmlWriter.writeStartDocument();
//
//       xmlWriter.writeStartElement("FaceRecognition");
//
//       xmlWriter.writeStartElement("Connection");
//       xmlWriter.writeTextElement("Ip", setup_dialog.ip_address);
//       xmlWriter.writeTextElement("Port", QString::number(setup_dialog.ip_port,10));
//       xmlWriter.writeTextElement("MuinPort", QString::number(setup_dialog.muin_port,10));
//       xmlWriter.writeEndElement();
//
//       xmlWriter.writeStartElement("Classifier");
//
//       xmlWriter.writeTextElement("Sensibility",QString::number(setup_dialog.sensibility,10));
//       xmlWriter.writeTextElement("Red",QString::number(setup_dialog.red,10));
//       xmlWriter.writeTextElement("Green",QString::number(setup_dialog.green,10));
//       xmlWriter.writeTextElement("Blue",QString::number(setup_dialog.blue,10));
//       xmlWriter.writeTextElement("CR_min",QString::number(setup_dialog.cr_min,10));
//       xmlWriter.writeTextElement("CR_max",QString::number(setup_dialog.cr_max,10));
//       xmlWriter.writeTextElement("PC_number",QString::number(setup_dialog.pc_number,10));
//
//       xmlWriter.writeStartElement("PlateRec");
//       xmlWriter.writeTextElement("DbaseLen",QString::number(setup_dialog.dbase_len,10));
//       xmlWriter.writeEndElement();
//
//       xmlWriter.writeEndElement();
//       xmlWriter.writeEndDocument();
//
//       file.close();
//
//       set_ip_address(ip_address,ip_port,muin_port);
//
//       set_params(classifier_file,sensibility,th_recon,th_alert,th_alarm);
//
//       set_plate_mask(red,green,blue);
//
//       set_plate_chr(cr_min,cr_max,pc_number);
//
//   }
//}


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
    printf("load pressed\n");
    QFileDialog dlg;

    dlg.setFilter("*.avi");
    if(dlg.exec())
    {
        m_filename = dlg.selectedFiles().at(0);
        printf("Open filename %s\n", m_filename.toAscii().data());
        fflush(stdout);
    }
}


void PlateGatewayDialog::on_playButton_clicked()
{
    printf("play pressed\n");
    if(!m_filename.isEmpty())
    {
        printf("filename sent is %s\n", m_filename.toAscii().data());
        fv_hlp->startPlateValidation(m_filename);
    }
    else
    {
        printf("Please select a file before playing\n");
    }
}


void PlateGatewayDialog::on_stopButton_clicked()
{
    printf("stop pressed\n");
    fv_hlp->stop_plate_validation();
}


//void PlateGatewayQt::on_AccessButton_clicked()
//{
//    Plates plates;
//    plates.exec();
//}
