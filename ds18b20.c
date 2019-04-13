/**********************************************************************

    > File Name: ds18b20.c

    > Author: xxx

    > Created Time: Tue 18 Sep 2018 08:14:09 UTC

***********************************************************************/
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <dirent.h> 
#include <string.h> 
#include <time.h> 
#include <errno.h>

int ds18b20_get_temperature(float *temp)
{
	char w1_path[50]="/sys/bus/w1/devices/";
	DIR *dirp;
	struct dirent *direntp;
	char chip[20];
	int found = 0;
	int fd = -1;
	char *ptr;
	char buf[128];

	if(!temp)
	{
		return -1;
	}
       	if((dirp=opendir(w1_path))==NULL)
       	{
		printf("open dir failture\n");
		return -2;
	}
       
	while((direntp=readdir(dirp))!=NULL)
		{
				if(strstr(direntp->d_name,"28-"))
				{
						strcpy(chip,direntp->d_name);
						found = 1;
				}
		}
		closedir(dirp);
		if(!found)
		{
			printf("can't find 18b20 in it %s\n",w1_path);
			return -3;
		}

		strncat(w1_path,chip,sizeof(w1_path));
	/*	strncat(
		*£


	*/
		strncat(w1_path,"/w1_slave",sizeof(w1_path));

		if((fd=open(w1_path,O_RDONLY))<0)
		{
				printf("open file %s failture %s\n",w1_path,strerror(errno));
				return -4;
		}
		if(read(fd,buf,sizeof(buf))<0)
		{
				printf("read error:%s\n",strerror(errno));
				return -5;
		}
		ptr=strstr(buf,"t=")+2;
		if(!ptr)
		{
				printf("can't get the temperature\n");
				return -6;
		}
		*temp=atof(ptr)/1000;
		return 0;
}
