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

#include <QListWidget>
#include <QString>
#include <QThread>

#include "PlateGateway.h"

class PlateValidationHelper : public QThread
{
    public:
        PlateValidationHelper();
        ~PlateValidationHelper();
        int startPlateValidation(const QString &filename);
        int stop_plate_validation();

    private:
        bool m_threadActive;
        QWidget *m_loc_wdg;
        QListWidget *m_plt_list;
        QListWidget *m_img_list;
        QString m_filename;

    protected:
        void run();
};
