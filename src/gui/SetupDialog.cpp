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

#include "SetupDialog.h"
#include "ui_SetupDialogUi.h"

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SetupDialog::showEvent( QShowEvent * event )
{
    ui->IpAddress->setText(ip_address);
    ui->IpPort->setText(QString::number(ip_port,10));
    //ui->MuinPort->setText(QString::number(muin_port,10));
    ui->Sensibility->setText(QString::number(sensibility,10));
    //ui->DbaseLen->setText(QString::number(dbase_len,10));
    ui->Red->setText(QString::number(red,10));
    ui->Green->setText(QString::number(green,10));
    ui->Blue->setText(QString::number(blue,10));
    ui->CR_Min->setText(QString::number(cr_min,10));
    ui->CR_Max->setText(QString::number(cr_max,10));
    ui->PC_Number->setText(QString::number(pc_number,10));

    return;
}


void SetupDialog::closeEvent( QCloseEvent * event )
{
    //QMessageBox msgbox;
    //msgbox.setText("prova");
    //msgbox.exec();

    QString str;
    bool ok;

    ip_address=ui->IpAddress->text();

    str=ui->IpPort->text();
    ip_port=str.toInt(&ok,10);

    //str=ui->MuinPort->text();
    muin_port=str.toInt(&ok,10);

    str=ui->Sensibility->text();
    sensibility=str.toInt(&ok,10);

    //str=ui->DbaseLen->text();
    dbase_len=str.toInt(&ok,10);

    str=ui->Red->text();
    red=str.toInt(&ok,10);

    str=ui->Green->text();
    green=str.toInt(&ok,10);

    str=ui->Blue->text();
    blue=str.toInt(&ok,10);

    str=ui->CR_Min->text();
    cr_min=str.toInt(&ok,10);

    str=ui->CR_Max->text();
    cr_max=str.toInt(&ok,10);

    str=ui->PC_Number->text();
    pc_number=str.toInt(&ok,10);

    return;
}

void SetupDialog::on_buttonBox_accepted()
{
    this->close();
}

void SetupDialog::on_buttonBox_rejected()
{
    this->close();
}
