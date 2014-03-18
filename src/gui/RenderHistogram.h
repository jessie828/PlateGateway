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

#include <QPaintEvent>
#include <QCloseEvent>

#include "RenderHistogramHelper.h"

class RenderHistogram : public QWidget
{
    Q_OBJECT

    public:
        RenderHistogram(RenderHistogramHelper *helper, QWidget *parent);
        ~RenderHistogram();
    public slots:
        void animate(int tmp_width,int tmp_height);

    protected:
        void paintEvent(QPaintEvent *event);
        void closeEvent(QCloseEvent *event);

    private:
        RenderHistogramHelper *helper;
        int elapsed;
        int width;
        int height;

    public:
        void Go();
};

