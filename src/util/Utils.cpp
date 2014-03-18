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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int read_ini_string(char inifile[],char key_find[],char param_find[],char *val)
{
    FILE *inistr;
    char tmp_char;
    char key_value[256];
    char param_line[256];
    char param_name[256];
    char param_value[256];
    int count_char=0;

    key_value[0]='\0';
    param_line[0]='\0';
    param_name[0]='\0';
    param_value[0]='\0';

    val[0]='\0';

    inistr = fopen(inifile,"r");

    if(inistr==NULL)
    {
        printf("unable to open %s file ini\n",inifile);
        return -1;
    }
    //find requested key

    while(1)
    {
        do
        {
            tmp_char=fgetc(inistr);

            if (feof(inistr))
            {
                printf("Unable to find a key!");
                fclose(inistr);
                return -1;
            }
        } while(tmp_char!='[');

        count_char=0;

        do
        {
            key_value[count_char]=tmp_char=fgetc(inistr);

            if (feof(inistr))
            {
                printf("Unable to read key!");
                fclose(inistr);
                return -1;
            }

            count_char++;

        } while(tmp_char!=']');

        key_value[count_char-1]='\0';

        //printf("key:%s\n",key_value);

        if(strcmp(key_value,key_find)==0)
        {
            //printf("key finded!\n");
            break;
        }
        else
            continue;
    }

    while(!feof(inistr))
    {
        //read param line
        count_char=0;
        do
        {
            tmp_char=param_line[count_char]=fgetc(inistr);
            count_char++;

        } while((tmp_char!='\n'));

        param_line[count_char-1]='\0';

        //printf("line=%s\n",param_line);

        if(param_line[0]=='[')
        {
            printf("unable to find %s\n",param_find);
            strcpy(val,"-1\n");
            break;
        }

        count_char=0;

        do
        {
            tmp_char=param_name[count_char]=param_line[count_char];

            count_char++;

        } while((tmp_char!='=') && (tmp_char!=' ') && (tmp_char!='\n') && (tmp_char!='\0'));

        param_name[count_char-1]='\0';

        //printf("param_name=%s\n",param_name);

        //skip blank
        do
        {
            tmp_char=param_line[count_char];
            count_char++;
            //printf("%c",tmp_char);
        } while((tmp_char=='=') || (tmp_char==' '));

        if((tmp_char!='=') || (tmp_char!=' '))
            count_char=count_char-1;

        //find param name
        if(strcmp(param_name,param_find)==0)
        {
            //printf("param finded!\n");
            //printf("param=%s\n",param_name);

            //find param value
            int count_char_2=0;
            do
            {
                tmp_char=param_value[count_char_2]=param_line[count_char];
                count_char++;
                count_char_2++;

            } while((tmp_char!=' ') && (tmp_char!='\n') && (tmp_char!='\0'));

            param_value[count_char_2-1]='\0';
            strcpy(val,param_value);

            break;
        }
    }

    fclose(inistr);

    return 0;
}

////////////////////////////////////////////////////////////////////
//write_ini_string

int write_ini_string(char inifile[],char key_find[],char param_find[],char *val)
{
    FILE *inistr;
    FILE *tmpinistr;

    char tmp_char;
    char key_value[1024];
    char param_line[1024];
    char param_name[1024];
    char param_value[1024];
    char cmd_str[1024];
    char tmp_str[1024];
    int count_char=0;

    key_value[0]='\0';
    param_line[0]='\0';
    param_name[0]='\0';
    param_value[0]='\0';

    inistr = fopen(inifile,"r");

    tmpinistr = fopen("/tmp/initmp.tmp","w");

    if(inistr==NULL)
    {
        printf("unable to open %s file ini\n",inifile);
        return -1;
    }

    if(tmpinistr==NULL)
    {
        printf("unable to open tmp file ini\n",inifile);
        return -1;
    }

    //find requested key

    while(1)
    {
        do
        {
            tmp_char=fgetc(inistr);
            fputc(tmp_char,tmpinistr);

            if (feof(inistr))
            {
                printf("Unable to find a key!");
                fclose(inistr);
                fclose(tmpinistr);
                return -1;
            }
        } while(tmp_char!='[');

        count_char=0;

        do
        {
            key_value[count_char]=tmp_char=fgetc(inistr);
            fputc(tmp_char,tmpinistr);
            if (feof(inistr))
            {
                printf("Unable to read key!");
                fclose(inistr);
                fclose(tmpinistr);
                return -1;
            }

            count_char++;

        } while(tmp_char!=']');

        key_value[count_char-1]='\0';

        //printf("key:%s\n",key_value);

        if(strcmp(key_value,key_find)==0)
        {
            //printf("key finded!\n");
            break;
        }
        else
            continue;
    }

    while(!feof(inistr))
    {
        //read param line
        count_char=0;
        do
        {
            tmp_char=param_line[count_char]=fgetc(inistr);
            count_char++;

        } while((tmp_char!='\n'));

        param_line[count_char-1]='\0';

        //printf("line=%s\n",param_line);

        if(param_line[0]=='[')
        {
            printf("unable to find param: %s!\n",param_find);
            sprintf(tmp_str,"%s\n",param_line);
            fprintf(tmpinistr,tmp_str);
            break;
        }

        count_char=0;

        do
        {
            tmp_char=param_name[count_char]=param_line[count_char];

            count_char++;

        } while((tmp_char!='=') && (tmp_char!=' ') && (tmp_char!='\n') && (tmp_char!='\0'));

        param_name[count_char-1]='\0';

        //printf("param_name=%s\n",param_name);

        //skip blank
        do
        {
            tmp_char=param_line[count_char];
            count_char++;
            //printf("%c",tmp_char);
        } while((tmp_char=='=') || (tmp_char==' '));

        if((tmp_char!='=') || (tmp_char!=' '))
            count_char=count_char-1;

        //find param name
        if(strcmp(param_name,param_find)==0)
        {
            //printf("param finded!\n");
            //printf("param=%s\n",param_name);

            //find param value
            int count_char_2=0;

            do
            {
                tmp_char=param_value[count_char_2]=param_line[count_char];
                count_char++;
                count_char_2++;

            } while((tmp_char!=' ') && (tmp_char!='\n') && (tmp_char!='\0'));

            param_value[count_char_2-1]='\0';

            sprintf(tmp_str,"%s = %s\n",param_find,val);

            fprintf(tmpinistr,tmp_str);

            break;
        }
        else
        {
            sprintf(tmp_str,"%s\n",param_line);
            fprintf(tmpinistr,tmp_str);
        }
    }

    while(!feof(inistr))
    {
        tmp_char=fgetc(inistr);
        if(!feof(inistr))
            fputc(tmp_char,tmpinistr);
    }

    fclose(inistr);
    fclose(tmpinistr);


    //inifile
    sprintf(cmd_str,"cp /tmp/initmp.tmp %s",inifile);
    system(cmd_str);
    remove("/tmp/initmp.tmp");

    return 0;
}
