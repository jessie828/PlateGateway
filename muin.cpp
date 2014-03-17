///////////////////////////////////////////////////////////////////////////////////
//
//     ////    /////// //     //  //     //   //    //////
//    //  //  //       //  //    //// ////  // //  //
//   //  //  //////     //      // //  // //   // //
//  //  //  //       // //     //     // /////// //
// ////    ////// //    //    //     // //   // //////
//
//muin.cpp
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

#include <muin.h>

#ifdef WIN32

CSyncSerialComm::CSyncSerialComm(const char *pszPortName)
        : m_hSerialComm(INVALID_HANDLE_VALUE)
{
        m_pszPortName = new char[strlen(pszPortName)];
        strcpy(m_pszPortName, pszPortName);
}

CSyncSerialComm::~CSyncSerialComm()
{
        if(m_pszPortName)
                delete[] m_pszPortName;

        Close();
}

HRESULT CSyncSerialComm::Open()
{
        HRESULT hResult;

        TCHAR wstring[256];

        MultiByteToWideChar(CP_ACP, 0, m_pszPortName, -1,  wstring, 256);

        m_hSerialComm = CreateFile(wstring, /* Port Name */
                                   GENERIC_READ | GENERIC_WRITE, /* Desired Access */
                                   0, /* Shared Mode */
                                   NULL, /* Security */
                                   OPEN_EXISTING, /* Creation Disposition */
                                   0,
                                   NULL); /* Non Overlapped */

        if(m_hSerialComm == INVALID_HANDLE_VALUE)
        {
                unsigned long error = ::GetLastError();
                hResult = E_FAIL;
        }

        else
                hResult = S_OK;

        return hResult;
}

HRESULT CSyncSerialComm::Close()
{
        if(m_hSerialComm != INVALID_HANDLE_VALUE)
        {
                CloseHandle(m_hSerialComm);
                m_hSerialComm = INVALID_HANDLE_VALUE;
        }

        return S_OK;
}

HRESULT CSyncSerialComm::ConfigPort(DWORD dwBaudRate, DWORD dwTimeOutInSec)
{
        if(!SetupComm(m_hSerialComm, 1024, 1024))
                return E_FAIL;

        DCB dcbConfig;

        if(GetCommState(m_hSerialComm, &dcbConfig)) /* Configuring Serial Port Settings */
        {
                dcbConfig.BaudRate = dwBaudRate;
                dcbConfig.ByteSize = 8;
                dcbConfig.Parity = NOPARITY;
                dcbConfig.StopBits = ONESTOPBIT;
                dcbConfig.fBinary = TRUE;
                dcbConfig.fParity = TRUE;
        }

        else
                return E_FAIL;

        if(!SetCommState(m_hSerialComm, &dcbConfig))
                return E_FAIL;

        COMMTIMEOUTS commTimeout;

        if(GetCommTimeouts(m_hSerialComm, &commTimeout)) /* Configuring Read & Write Time Outs */
        {
                commTimeout.ReadIntervalTimeout = 100*dwTimeOutInSec;
                commTimeout.ReadTotalTimeoutConstant = 100*dwTimeOutInSec;
                commTimeout.ReadTotalTimeoutMultiplier = 0;
                commTimeout.WriteTotalTimeoutConstant = 100*dwTimeOutInSec;
                commTimeout.WriteTotalTimeoutMultiplier = 0;
        }

        else
                return E_FAIL;

        if(SetCommTimeouts(m_hSerialComm, &commTimeout))
                return S_OK;

        else
                return E_FAIL;
}

HRESULT CSyncSerialComm::Read(char *ppszBuf, DWORD &dwSize)
{
        HRESULT hResult = S_OK;
        std::stringbuf sb;
        DWORD dwEventMask;

        if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) /* Setting Event Type */
                return E_FAIL;

        if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL)) /* Waiting For Event to Occur */
        {
                char szBuf;
                DWORD dwIncommingReadSize;

                do
                {
                        if(ReadFile(m_hSerialComm, &szBuf, 1, &dwIncommingReadSize, NULL) != 0)
                        {
                                if(dwIncommingReadSize > 0)
                                {
                                        dwSize += dwIncommingReadSize;
                                        sb.sputn(&szBuf, dwIncommingReadSize);
                                }
                        }

                        else
                        {
                                unsigned long error = ::GetLastError();
                                hResult = E_FAIL;
                                break;
                        }

                } while(dwIncommingReadSize > 0);

                strcpy(ppszBuf, (sb.str()).c_str());

                return hResult;
        }

        else
                return E_FAIL;
}

HRESULT CSyncSerialComm::Write(const char *pszBuf, DWORD dwSize)
{
        HRESULT hResult = S_OK;

        unsigned long dwNumberOfBytesSent = 0;

        while(dwNumberOfBytesSent < dwSize)
        {
                unsigned long dwNumberOfBytesWritten;

                if(WriteFile(m_hSerialComm, &pszBuf[dwNumberOfBytesSent], 1, &dwNumberOfBytesWritten, NULL) != 0)
                {
                        if(dwNumberOfBytesWritten > 0)
                                ++dwNumberOfBytesSent;
                        else
                        {
                                unsigned long error = ::GetLastError();
                                hResult = E_FAIL;
                                break;
                        }
                }

                else
                {
                        unsigned long error = ::GetLastError();
                        hResult = E_FAIL;
                        break;
                }
        }

        return hResult;
}

HRESULT CSyncSerialComm::Flush(DWORD dwFlag)
{
        if(PurgeComm(m_hSerialComm, dwFlag))
                return S_OK;
        else
                return E_FAIL;
}

#endif

int muin::Open(char *device,long BAUD)
{
#ifndef WIN32
        struct termios oldkey, newkey;       //place tor old and new port settings for keyboard teletype
        struct sigaction saio;               //definition of signal action

        fd_ptz = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);

        if (fd_ptz < 0)
        {
                perror(device);
                return (-1);
        }

        tcgetattr(fd_ptz,&oldtio);	// save current port settings

        // set new port settings for canonical input processing
        newtio.c_cflag = BAUD | CS8 | CLOCAL | CREAD;

        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;       //ICANON;
        newtio.c_cc[VMIN]=1;
        newtio.c_cc[VTIME]=0;
        tcflush(fd_ptz, TCIFLUSH);
        tcsetattr(fd_ptz,TCSANOW,&newtio);
#endif

#ifdef WIN32
        ser_comm = new CSyncSerialComm(device);
        ser_comm->Open();
        ser_comm->ConfigPort(BAUD,1);
#endif

        return 0;
}

void muin::Close()
{
#ifndef WIN32
         tcsetattr(fd_ptz,TCSANOW,&oldtio);
         close(fd_ptz);
#endif

#ifdef WIN32
        ser_comm->Close();
#endif
}

int muin::Write(unsigned char *msg,int msgsize)
{

#ifdef DEBUG
        printf("msg: ");
        for(int i=0;i<msgsize;i++)
        {
                printf("0x%x ",msg[i]);
        }
        printf("\n");
        fflush(stdout);
#endif

#ifndef WIN32
        write(fd_ptz,msg,msgsize);
        fdatasync(fd_ptz);
#endif

#ifdef WIN32
        int ret_val=ser_comm->Write((const char *)msg,msgsize);
        return ret_val;
#endif
}

int muin::Read(unsigned char *buffer,char eod,int timeout)
{
#ifndef WIN32
    int val=0;
    int res=0;
    int counter=0;
    char chr=0;
    int chr_cntr=0;

    do
    {
        res=read(fd_ptz,&chr,1);

        res=ser_comm.Read(&chr,1);

        if(res!=-1)
        {
            buffer[chr_cntr]=(unsigned char)chr;
            chr_cntr++;
        }

        //printf("chr:%d,%d,%d,%d\n",chr,eod,res,chr_cntr);

        if(chr==eod)
            break;

        usleep(1000);

        counter++;

    } while (counter<timeout);

    val=chr_cntr;

    buffer[chr_cntr]='\0';

    if(counter>=timeout)
            val=-1;
#endif

#ifdef WIN32
    DWORD val=0;
    ser_comm->Read((char *)buffer,val);
#endif

    return val;
}

int muin::GetADC(unsigned char *buffer_out,int *data_out)
{
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='@';
        buffer_in[1]='A';
        buffer_in[2]='0';
        buffer_in[3]='0';
        buffer_in[4]='0';
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int val=0;

        do
        {
                Write(buffer_in,8);
                val=Read(buffer_tmp,'#',1000);
        } while(val==-1);

        int count=0;
        for(int k=2;k<(val-1);k++)
        {
            int j=0;
            int counter=0;

            while(counter<4)
            {
                if(buffer_tmp[k]!=0)
                {
                        buffer_out[j]=buffer_tmp[k];
                        j++;
                }
                k++;
                counter++;
            }

            buffer_out[j]='\0';

            data_out[count]=atoi((char *)buffer_out);

            count++;
            k--;
        }

        return 0;
}

int muin::GetPortB(unsigned char &val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='@';
        buffer_in[1]='G';
        buffer_in[2]='R';
        buffer_in[3]='0';
        buffer_in[4]='0';
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;

        do
        {
            Write(buffer_in,8);
            val=Read(buffer_tmp,'#',1000);
            printf("data:%d,%s",val,buffer_tmp);
            fflush(stdout);
        } while(val==-1);

        for(int k=0;k<counter;k++)
        {
            if(buffer_tmp[k]=0)
                    buffer_out[k]='-';
            else
                    buffer_out[k]=buffer_tmp[k];
        }

        buffer_out[counter]='\0';

#ifdef DEBUG
        printf("%s\n",buffer_out);
        fflush(stdout);
#endif

        val=buffer_tmp[2];

        return 0;
}

int muin::SetPortB(unsigned char val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='@';
        buffer_in[1]='G';
        buffer_in[2]='W';
        buffer_in[3]=(val & 0xff);
        buffer_in[4]='0';
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;

        Write(buffer_in,8);

        return 0;
}

int muin::GetChannel()
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='+';
        buffer_in[1]='+';
        buffer_in[2]='+';

        int counter=0;
        do
        {
                Write(buffer_in,3);
                counter=Read(buffer_out,'\r',1000);
        } while(counter==-1);

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='I';
        buffer_in[3]='D';
        buffer_in[4]='\r';

        counter=0;
        do
        {
                Write(buffer_in,5);
                counter=Read(buffer_out,'\r',1000);
        }while(counter==-1);

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        int val=atoi((char *)buffer_out);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='C';
        buffer_in[3]='N';
        buffer_in[4]='\r';

        counter=0;
        do
        {
                Write(buffer_in,5);
                counter=Read(buffer_out,'\r',1000);
        } while(counter==-1);

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        return val;
}

int muin::SetChannel(int val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='+';
        buffer_in[1]='+';
        buffer_in[2]='+';


        int counter=0;
        do
        {
                Write(buffer_in,3);
                counter=Read(buffer_out,'\r',1000);
        }	while(counter==-1);


        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        char tmp_val[256];

        sprintf(tmp_val,"%4.0d",val);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='I';
        buffer_in[3]='D';
        buffer_in[4]=tmp_val[0];
        buffer_in[5]=tmp_val[1];
        buffer_in[6]=tmp_val[2];
        buffer_in[7]=tmp_val[3];
        buffer_in[8]='\r';

        counter=0;
        do
        {
                Write(buffer_in,9);
                counter=Read(buffer_out,'\r',1000);
        }	while(counter==-1);

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='C';
        buffer_in[3]='N';
        buffer_in[4]='\r';

        counter=0;
        do
        {
                Write(buffer_in,5);
                counter=Read(buffer_out,'\r',1000);
        }	while(counter==-1);

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        return 0;
}

int muin::SetChannelWrite(int val)
{
#ifndef WIN32
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_in[0]='+';
        buffer_in[1]='+';
        buffer_in[2]='+';

        int res=0;
        int counter=0;

        Write(buffer_in,3);

        do
        {
                res=read(fd_ptz,buffer_out+counter,1);

                if(res!=-1)
                {
                        counter+=res;
                }

        } while (buffer_out[counter-1]!='\r');

        buffer_out[counter]='\0';

        printf("Rsp: %s",buffer_out);

        char tmp_val[256];

        sprintf(tmp_val,"%4.0d",val);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='I';
        buffer_in[3]='D';
        buffer_in[4]=tmp_val[0];
        buffer_in[5]=tmp_val[1];
        buffer_in[6]=tmp_val[2];
        buffer_in[7]=tmp_val[3];
        buffer_in[8]='\r';

        Write(buffer_in,9);

        counter = 0;
        res=0;
        buffer_out[0]='\0';
        do
        {
                res=read(fd_ptz,buffer_out+counter,1);

                if(res!=-1)
                {
                        counter+=res;
                }

        } while (buffer_out[counter-1]!='\r');

        buffer_out[counter+1]='\0';

        printf("Rsp: %s",buffer_out);


        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='W';
        buffer_in[3]='R';
        buffer_in[4]='\r';

        Write(buffer_in,5);

        counter = 0;
        res=0;
        buffer_out[0]='\0';
        do
        {
                res=read(fd_ptz,buffer_out+counter,1);

                if(res!=-1)
                {
                        counter+=res;
                }

        } while (buffer_out[counter-1]!='\r');

        buffer_out[counter+1]='\0';

        printf("Rsp: %s",buffer_out);

        buffer_in[0]='A';
        buffer_in[1]='T';
        buffer_in[2]='C';
        buffer_in[3]='N';
        buffer_in[4]='\r';

        Write(buffer_in,5);

        counter = 0;
        res=0;
        buffer_out[0]='\0';
        do
        {
                res=read(fd_ptz,buffer_out+counter,1);

                if(res!=-1)
                {
                        counter+=res;
                }

        } while (buffer_out[counter-1]!='\r');

        buffer_out[counter+1]='\0';

        printf("Rsp: %s",buffer_out);
#endif

        return 0;
}

int muin::SetPwm(unsigned char pwm, unsigned int val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        char str_pwm[256];
        //char val_hibyte[256];
        //char val_lobyte[256];

        sprintf(str_pwm,"%d",pwm);
        //sprintf(val_hibyte,"%d",(val & 0xf0) >>8);
        //sprintf(val_lobyte,"%d",val & 0x0f);

        buffer_in[0]='@';
        buffer_in[1]='P';
        buffer_in[2]=str_pwm[0];
        buffer_in[3]=val >>8;
        buffer_in[4]=val;
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;

        Write(buffer_in,8);

        return 0;
}

int muin::SetServo(unsigned char servo, unsigned int val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        char str_servo[256];
        //char val_hibyte[256];
        //char val_lobyte[256];

        sprintf(str_servo,"%d",servo);
        //sprintf(val_hibyte,"%d",(val & 0xf0) >>8);
        //sprintf(val_lobyte,"%d",val & 0x0f);

        buffer_in[0]='@';
        buffer_in[1]='S';
        buffer_in[2]=str_servo[0];
        buffer_in[3]=val>>8;
        buffer_in[4]=val;
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;

        Write(buffer_in,8);

        return 0;
}

int muin::ReadCmps03(double &val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        buffer_out[0]='\0';
        buffer_in[0]='\0';
        buffer_tmp[0]='\0';

        buffer_in[0]='@';
        buffer_in[1]='C';
        buffer_in[2]=1;
        buffer_in[3]=0xC0;
        buffer_in[4]='0';
        buffer_in[5]='0';
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;
        int ret_val=0;
        
        do
        {
                Write(buffer_in,8);
                ret_val=Read(buffer_tmp,'#',1000);
                counter++;
        } while(counter<8 && ret_val==-1);

        buffer_tmp[ret_val]='\0';

        strncpy((char *)buffer_out,(char *)buffer_tmp+2,4);

        buffer_out[4]='\0';
        buffer_tmp[4]='\0';
        
        memset(buffer_tmp,0,4);

        //printf("%c,%c,%c,%c\n",buffer_out[0],buffer_out[1],buffer_out[2],buffer_out[3]);

        /*if(buffer_out[2]==0 && buffer_out[3]==0)
        {
            strncpy((char *)buffer_tmp,(char *)buffer_out,1);
            val=atoi((char *)buffer_tmp);

            //printf("1,%s\n",buffer_tmp);
        }
        else if(buffer_out[3]==0)
        {
            strncpy((char *)buffer_tmp,(char *)buffer_out,2);
            val=atoi((char *)buffer_tmp);

            //printf("2,%s\n",buffer_tmp);
        }
        else
        {
            sscanf((char *)buffer_out,"%d",&val);
            val=val/10;

            //printf("3\n");
        }*/
	
	int i=0;
	int j=0;
	while(i<4)
	{
		if(buffer_out[i]!=0)
		{
			buffer_tmp[j]=buffer_out[i];
			j++;
		}
		i++;
	}
	
	val=atoi((char *)buffer_tmp)/10.0;
        
#ifdef DEBUG
        printf("%s\n",buffer_out);
        fflush(stdout);
#endif

        //sscanf((char *)buffer_out,"@C%d#",&val);

        return 0;
}

int muin::CalibrateCmps03(unsigned char val)
{
        unsigned char buffer_out[256];
        unsigned char buffer_in[256];
        unsigned char buffer_tmp[256];

        //char str_servo[256];
        //char val_hibyte[256];
        //char val_lobyte[256];

        //sprintf(str_servo,"%d",servo);
        //sprintf(val_hibyte,"%d",(val & 0xf0) >>8);
        //sprintf(val_lobyte,"%d",val & 0x0f);

        buffer_in[0]='@';
        buffer_in[1]='I';
        buffer_in[2]=2;
        buffer_in[3]=0xC0;
        buffer_in[4]=0x0F;
        buffer_in[5]=0xFF;
        buffer_in[6]='0';
        buffer_in[7]='#';

        int res=0;
        int counter=0;

        Write(buffer_in,8);

        return 0;
}
