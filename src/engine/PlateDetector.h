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

#ifndef PLATEDETECTOR_H_
#define PLATEDETECTOR_H_

#include <QPixmap>

class PlateDetector
{

public:
    PlateDetector();
    ~PlateDetector();

    QPixmap getThumbnail(const QString &filename);
};

#endif /* PLATEDETECTOR_H_ */
