PlateGateway
============

PlateGateway is a license plate recognition ANPR open source software. It can read video files.

It uses:

- [OpenCV](http://www.opencv.org): plate location and recognition (2.4.8)
- FFmpeg : live ip camera connection and video decoding (0.6.6)
- [Tesseract](https://code.google.com/p/tesseract-ocr): OCR (3.02.02)
- [Leptonica](http://www.leptonica.com): Image Processing (1.70)
- [QT](http://qtcenter.org): graphic interface (4.8.5)

It came with QTCreator project and it can easily adapted to every supported platform. This software run on Linux systems Now Tesseract ocr is available, but no training file is released (it could be done, obviously, but it is dependent by plate type).

It can read multiple plates at same time. With new release, positive hit number on test video file is many times better than old one.

The software is released under the GNU GPL V3 Open source license.

This application is based on the project found at: http://www.dexmac.com/index.php?option=com_content&view=article&id=114:plategatewayqt&catid=82&Itemid=474
