////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//plates.h
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

#ifndef PLATES_H
#define PLATES_H

#include <QDialog>
#include <QListWidgetItem>
namespace Ui {
    class Plates;
}

class Plates : public QDialog
{
    Q_OBJECT

public:
    explicit Plates(QWidget *parent = 0);
    ~Plates();

private slots:
    void on_AddPlate_clicked();

    void on_RemovePlate_clicked();

    void ItemClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

private:
    Ui::Plates *ui;
};

#endif // PLATES_H
