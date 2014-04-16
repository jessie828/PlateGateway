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
#include <QPixmap>

#include "PlateGateway.h"
#include "PlateDetector.h"

class PlateValidationHelper : public QThread
{
    public:
        PlateValidationHelper();
        ~PlateValidationHelper();
        QPixmap getThumbnail(const QString &filename);
        int startPlateValidation(const QString &filename);
        int stop_plate_validation();

    private:
        bool m_threadActive;
        QString m_filename;

    protected:
        void run();
};
