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

#include "PlateValidationHelper.h"

#include <stdio.h>

PlateValidationHelper::PlateValidationHelper()
{
    m_threadActive=false;
}

PlateValidationHelper::~PlateValidationHelper()
{

}

void PlateValidationHelper::run()
{
    printf("find plate\n");
    PlateDetector::getInstance()->plateDetect(m_filename);
}

int PlateValidationHelper::startPlateValidation(const QString &filename)
{
    m_filename = filename;

    if(!m_threadActive)
    {
        m_threadActive=true;
        start();
    }

    return 0;
}

int PlateValidationHelper::stop_plate_validation()
{
    m_threadActive=false;
    PlateDetector::getInstance()->stopPlateDetect();
    return 0;
}
