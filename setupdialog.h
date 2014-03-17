////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//setupdialog.h
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

#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
    class SetupDialog;
}

class SetupDialog : public QDialog {
    Q_OBJECT
public:
    SetupDialog(QWidget *parent = 0);
    ~SetupDialog();

protected:
    void changeEvent(QEvent *e);
    void showEvent ( QShowEvent * event );
    void closeEvent( QCloseEvent * event );

private:
    Ui::SetupDialog *ui;

public:
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
    int cr_number;
    int cr_min;
    int cr_max;
    int pc_number;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // SETUPDIALOG_H
