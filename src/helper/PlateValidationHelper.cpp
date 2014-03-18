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
    printf("made something\n");
    m_threadActive=false;
}

PlateValidationHelper::~PlateValidationHelper()
{

}

void PlateValidationHelper::run()
{
    printf("run forest run\n");
    plate_detect(m_filename.toAscii().data(), 0);
}

int PlateValidationHelper::startPlateValidation(const QString &filename)
{
    printf("received filename is %s\n", filename.toAscii().data());
    m_filename = filename;

    if(!m_threadActive)
    {
        m_threadActive=true;
        start();
    }
    else
    {
        printf("thread allready active doing nothing\n");
    }

    return 0;
}

int PlateValidationHelper::stop_plate_validation()
{
    printf("stopdetect\n");
    m_threadActive=false;
//    stop_plate_detect();
}
