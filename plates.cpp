////////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//plates.cpp
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

#include "plates.h"
#include "ui_plates.h"
#include <QDebug>

Plates::Plates(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plates)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(ItemClicked(QListWidgetItem *)));
    FILE *stream=fopen("plate.txt","r");
    if(stream)
    {
        while(!feof(stream))
        {
            char str[256];
            fscanf(stream,"%s\n",str);
            ui->listWidget->addItem((QString)str);
        }
        fclose(stream);
    }
}

Plates::~Plates()
{
    delete ui;
}

void Plates::on_AddPlate_clicked()
{
    QString tmp_str=ui->PlateEdit->text();
    ui->listWidget->addItem(tmp_str);
}

void Plates::on_RemovePlate_clicked()
{
    QString tmp_str=ui->PlateEdit->text();
    QList<QListWidgetItem *> wdget_list = ui->listWidget->findItems(tmp_str,Qt::MatchExactly);
    if(wdget_list.count()!=0)
    {
        int row = ui->listWidget->row(wdget_list[0]);
        ui->listWidget->takeItem(row);
    }
}

void Plates::ItemClicked(QListWidgetItem *item)
{
    QString tmp_str=item->text();
    ui->PlateEdit->setText(tmp_str);
}

void Plates::on_buttonBox_accepted()
{
    FILE *stream=fopen("plate.txt","w");
    if(stream)
    {
        while(ui->listWidget->count())
        {
            QListWidgetItem *wdget = ui->listWidget->takeItem(0);
            fprintf(stream,"%s\n",wdget->text().toAscii().data());
        }

        fclose(stream);
    }
}
