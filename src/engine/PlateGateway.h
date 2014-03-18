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

#ifndef __FACEGATEWAY_H

#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include <QDateTime>
#include <QListWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QDebug>

int plate_detect(char *filename,int mode);
int get_image(unsigned char *img,int dimx,int dimy);
int get_histogram(unsigned char *img,int dimx,int dimy);
int get_image_list(QList<QString> &img_list);
int get_plate_list(QList<QString> &plt_list);
int stop_plate_detect();
int set_ip_address(QString ip_address,int port,int muin_chn);
int set_params(QString classifier,int sensibility,int rec_th,int alert_th,int alarm_th);
int set_best_plate(char *value);
int get_best_plate(char *value);

double get_time();

int set_plate_mask(int red,int green,int blue);
int set_plate_chr(int min,int max,int number);

#define __FACEGATEWAY_H
#endif
