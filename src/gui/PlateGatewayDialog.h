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

#ifndef PLATEGATEWAYQT_H
#define PLATEGATEWAYQT_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPainter>
#include <QImage>
#include <QTimer>

#include <stdio.h>

#include "ui_PlateGatewayDialogUi.h"
#include "PlateValidationHelper.h"
#include "RenderHistogramHelper.h"
#include "RenderVideo.h"
#include "RenderHistogram.h"

class PlateGatewayDialog : public QMainWindow, public Ui_PlateGatewayQt
{
    Q_OBJECT;

public:
    PlateGatewayDialog(QWidget *parent = 0);
    ~PlateGatewayDialog();

private:
    void showThumbnail(const QString &filename);

private:
    RenderVideoHelper *m_renderVideoHelper;
    RenderHistogramHelper *m_renderHistogramHelper;
    PlateValidationHelper *m_plateValidateHelper;

    RenderVideo *renderVideoWidget;
    RenderHistogram *renderHistogramwidget;

    QTimer *render_timer;
    QTimer *update_timer;
    QTimer *update_plate_timer;
    QTimer *update_database_timer;

//    QString classifier_file;
//    int sensibility;
//    int th_recon;
//    int th_alert;
//    int th_alarm;
//    int dbase_len;
//    int red;
//    int green;
//    int blue;
//    int cr_min;
//    int cr_max;
//    int pc_number;
//    char plate[1024];
    QString m_filename;

private slots:
    void update_screen();
    //void update_list();
    //void update_plate_list();
    //void update_image_list();
    //void update_database();
//    void click_image(QListWidgetItem *);
    void on_loadButton_clicked();
    void on_playButton_clicked();
    void on_stopButton_clicked();
//    void on_AccessButton_clicked();
};

#endif // PLATEGATEWAYQT_H
