///////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//muin.h
//Dexmac 2011
//
//www.dexmac.com
//
// GNU General Public License Usage
// Alternatively, this file may be used under the terms of the GNU
// General Public License version 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL included in the
// packaging of this file.  Please review the following information to
// ensure the GNU General Public License version 3.0 requirements will be
// met: http://www.gnu.org/copyleft/gpl.html.
///////////////////////////////////////////////////////////////////////////////////

#ifndef MUIN_H
#define MUIN_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

#ifndef WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <sstream>

class CSyncSerialComm
{
public:
        CSyncSerialComm(){};

        HRESULT Flush(DWORD dwFlag = PURGE_TXCLEAR | PURGE_RXCLEAR);
        HRESULT Write(const char *pszBuf, DWORD dwSize);
        HRESULT Read(char *ppszBuf, DWORD &dwSize);
        HRESULT ConfigPort(DWORD dwBaudRate = CBR_19200, DWORD dwTimeOutInSec = 5);
        HRESULT Close();
        HRESULT Open();

        CSyncSerialComm(const char *pszPortName);
        virtual ~CSyncSerialComm();

private:
        char *m_pszPortName;
        HANDLE m_hSerialComm;
};

#endif

#ifdef WIN32
#define BAUDRATE 9600
#define MODEMDEVICE "COM3"
#endif

#ifndef WIN32
#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1         //POSIX compliant source
#define FALSE 0
#define TRUE 1
#define BYTE unsigned char
#endif

//#define DEBUG

class muin{

public:
    muin(){};
    ~muin(){};

public:

#ifndef WIN32
    struct termios newtio;
    struct termios oldtio;
    int fd_ptz;
#endif

#ifdef WIN32
    CSyncSerialComm *ser_comm;
#endif

public:
    int Open(char *device,long BAUD);
    void Close();
    int Write(unsigned char *msg,int msgsize);
    int GetADC(unsigned char *buffer_out,int *data_val);
    int GetPortB(unsigned char &val);
    int SetPortB(unsigned char val);
    int GetChannel();
    int SetChannel(int val);
    int SetChannelWrite(int val);
    int Read(unsigned char *buffer,char eod,int timeout);
    int SetPwm(unsigned char pwm,unsigned int val);
    int SetServo(unsigned char servo, unsigned int val);
    int ReadCmps03(double &val);
    int CalibrateCmps03(unsigned char val);
};

#endif // MUIN_H
