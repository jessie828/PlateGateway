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

#include <QThread>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>

#include "PlateGateway.h"

class RenderVideoHelper : public QThread
{
    public:
        RenderVideoHelper();
        ~RenderVideoHelper();
        void paint(QPainter *painter, QPaintEvent *event, int elapsed,int width,int height);
        int start_rendering();

    private:
        unsigned char *vid_image_data_rgb;
        unsigned char *vid_image_data_yuv;
        QImage vid_image;
        int image_width;
        int image_height;
        int thread_active;

    protected:
        void run();
};
