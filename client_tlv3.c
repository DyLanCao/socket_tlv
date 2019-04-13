#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "ds18b20.h"
#include "crc-itu-t.h"
#define TLV_HEAD 0xFD
#define MAX_DATA 32
enum type
{
	TAG1=0x01,
	TAG2,
	TAG3,
};
int tlv_data(char *buf,float value);
int main(int argc,char **argv)
{
	char sendbuf[MAX_DATA]={0};
	int sockfd = -1;
	struct sockaddr_in server;
        int res = -1;
 	float temp;
	unsigned short crc16;
	int i;
	int ret;	
	int len = 0;
	ds18b20_get_temperature(&temp);
	int rest = (int)temp;
	printf("the temp of room is :%d\n",rest);
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if( -1 == sockfd)
	{
	perror("sock created");
	exit(-1);
        }

	memset(&server,0,sizeof(struct sockaddr_in));
        server.sin_family = AF_INET;
	server.sin_port = htons(6669);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	res = connect(sockfd,(struct sockaddr*)&server,sizeof(server));
	if( -1 == res){
		perror("sock connect");
		exit(-1);
	}	
	printf("connect server success\n");
	while(1)
	{
		int resl = tlv_data(sendbuf,temp);

	/*	for(i=0;i<resl;i++)
		{
			if(i<resl-1)
			{
			printf("%02x",sendbuf[i]);
			}
			else 
			{
				printf("%02x\n",sendbuf[i]);
			}
			
		}
	*/	
		crc16=crc_itu_t(MAGIC_CRC,sendbuf,strlen(sendbuf));
		ushort_to_bytes(&sendbuf[strlen(sendbuf)],crc16);
		printf("%x\n",crc16);
		for(i=0;i<strlen(sendbuf);i++)
		{
			printf("%02x",sendbuf[i]);
		}
		printf("\n");
		crc16=crc_itu_t(MAGIC_CRC,sendbuf,sendbuf[2]);
		printf("%x\n",crc16);
		ret = write(sockfd,sendbuf,strlen(sendbuf));
		if(ret < 0)
		{
			printf("%s\n",strerror(errno));
		}
		sleep(2);
	}
	close(sockfd);
	return 0;
}
int tlv_data(char *buf,float value)
{
	int a,b,c,d,e,f;
	float x,y;
	int i;
	printf("%f\n",value);
	/*a = (int)value;
	x = value - a;
	b = x*1000000;
	c = b/10000;
	d = (b%10000)/100;
	e = (b%10000)%100;*/
/*	char *p;
	p=buf;*/
	for(i=0;i<MAX_DATA;i++)
	{
			buf[i++]=TLV_HEAD;
			buf[i++]=TAG1;
			buf[i++]=0x08;
			a=(int)value;
			x=value - a;
			b=x*1000000;
			c=b/10000;
			d=(b%10000)/100;
			e=(b%10000)%100;
		 	buf[i++]=a;
			buf[i++]=c;
			buf[i++]=d;
			buf[i++]=e;	
			
			break;
		

	}
	
	return i;
}
