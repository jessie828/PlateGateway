////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//plategateway.h
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

#ifndef PLATEGATEWAYQT_H
#define PLATEGATEWAYQT_H

#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include <QListWidget>
#include <QDir>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <stdio.h>
#include <QFileDialog>
#include <QDebug>
#include <QProcess>

class PlateValidationHelper :  QThread
{
    public:
        PlateValidationHelper();
        ~PlateValidationHelper();

    public:

        int start_plate_validation(QString file_name,int md);
        int stop_plate_validation();

    private:

        int thread_active;
        QWidget *loc_wdg;
        QListWidget *plt_list;
        QListWidget *img_list;
        QString filename;
        int mode;

    protected:
        void run();
};

class RenderHistogramHelper :  QThread
{
    public:
        RenderHistogramHelper();
        ~RenderHistogramHelper();

    public:

        void paint(QPainter *painter, QPaintEvent *event, int elapsed,int width,int height);
        int start_rendering();

    private:

        unsigned char *vid_image_data_rgb;
        unsigned char *vid_image_data_yuv;
        QImage vid_image;
        int image_width;
        int image_height;
        int thread_active;

    protected:
        void run();
};

class RenderVideoHelper :  QThread
{
    public:
        RenderVideoHelper();
        ~RenderVideoHelper();

    public:

        void paint(QPainter *painter, QPaintEvent *event, int elapsed,int width,int height);
        int start_rendering();

    private:

        unsigned char *vid_image_data_rgb;
        unsigned char *vid_image_data_yuv;
        QImage vid_image;
        int image_width;
        int image_height;
        int thread_active;

    protected:
        void run();
};


class Widget_RenderVideo : public QWidget
{
    Q_OBJECT

    public:
        Widget_RenderVideo(RenderVideoHelper *helper, QWidget *parent);
        ~Widget_RenderVideo();
    public slots:
        void animate(int tmp_width,int tmp_height);

    protected:
        void paintEvent(QPaintEvent *event);
        void closeEvent(QCloseEvent *event);

    private:
        RenderVideoHelper *helper;
        int elapsed;
        int width;
        int height;

    public:
        void Go();
};


class Widget_RenderHistogram : public QWidget
{
    Q_OBJECT

    public:
        Widget_RenderHistogram(RenderHistogramHelper *helper, QWidget *parent);
        ~Widget_RenderHistogram();
    public slots:
        void animate(int tmp_width,int tmp_height);

    protected:
        void paintEvent(QPaintEvent *event);
        void closeEvent(QCloseEvent *event);

    private:
        RenderHistogramHelper *helper;
        int elapsed;
        int width;
        int height;

    public:
        void Go();
};

namespace Ui {
    class PlateGatewayQt;
}

class PlateGatewayQt : public QMainWindow {
    Q_OBJECT
public:
    PlateGatewayQt(QWidget *parent = 0);
    ~PlateGatewayQt();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PlateGatewayQt *ui;

    RenderVideoHelper *rv_hlp;
    RenderHistogramHelper *rh_hlp;
    PlateValidationHelper *fv_hlp;

    Widget_RenderVideo *rv_widget;
    Widget_RenderHistogram *rh_widget;

    QTimer *render_timer;
    QTimer *update_timer;
    QTimer *update_plate_timer;
    QTimer *update_database_timer;

    int counter_item;

    QDir dir;

    QXmlStreamReader Rxml;

    int get_element_xml(QString &str);

    QString ip_address;
    int ip_port;
    int muin_port;
    QString classifier_file;
    int sensibility;
    int th_recon;
    int th_alert;
    int th_alarm;
    int dbase_len;
    int red;
    int green;
    int blue;
    int cr_min;
    int cr_max;
    int pc_number;
    int row_counter;
    char plate[1024];
    QString filename;
    int mode;

public:

private slots:
    void on_SetupButton_clicked();
    void on_StopButton_clicked();
    void on_StartButton_clicked();
    void update_screen();
    //void update_list();
    //void update_plate_list();
    //void update_image_list();
    //void update_database();
    void click_image(QListWidgetItem *);
    void on_loadButton_clicked();
    void on_playButton_clicked();
    void on_stopButton_clicked();
    void on_AccessButton_clicked();
    void on_WCStopButton_clicked();
    void on_WCStartButton_clicked();
};

#endif // PLATEGATEWAYQT_H
