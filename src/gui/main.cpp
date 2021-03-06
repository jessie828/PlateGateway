////////////////////////////////////////////////////////////////////////////////////
//
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU
// General Public License version 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE included in the
// packaging of this file.  Please review the following information to
// ensure the GNU General Public License version 3.0 requirements will be
// met: http://www.gnu.org/copyleft/gpl.html.
///////////////////////////////////////////////////////////////////////////////////

#include <QApplication>

#include "PlateGatewayDialog.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    PlateGatewayDialog window;
    window.show();
    return application.exec();
}
