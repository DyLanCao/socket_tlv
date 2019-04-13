#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "crc-itu-t.h"
#define MAX_DATA 4
#define TLV_HEAD 0xFD
enum type
{
	TAG1=0x01,
	TAG2,
	TAG3,
};
int unpack(char buf[],int bytes);
int main(int argc,char **argv)
{
	int on = 1,i;
	float a,b,c,d,e;
	int sockfd,bindres,listenres;
	char recvBuf[1024];
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd )
	{
		perror("sock created");
		exit(-1);
	}

	struct sockaddr_in server;
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(6669);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	bindres = bind(sockfd,(struct sockaddr*)&server,sizeof(server));
	if(-1 == bindres){
	 	perror("sock bind");
		exit(-1);
	}								

	listenres = listen(sockfd,SOMAXCONN);
      	if(-1 == listenres){
		perror("sock listen");
		exit(-1);
	}
	struct sockaddr_in peerServer;		 
	int acceptfd = -1;	
	socklen_t len = sizeof(peerServer);
	printf("waiting the client to connect...\n");
  	while(1)
	{
		acceptfd = accept(sockfd,(struct sockaddr*)&peerServer,&len);
		if(-1 == acceptfd)
		{
			perror("sock accept");
			exit(-1);
		}

	//	printf("Accept new fd=%d\n", acceptfd);
	
		pid_t pid = fork();
		if(pid < 0)
		{
			printf("fork() create failure.\n");
			return -1;
		}
		else if(pid == 0)
		{
			close(sockfd);
			while(1)
			{			
				memset(recvBuf,0,sizeof(recvBuf));
	       			//printf("client ip_addr is %s\n",inet_ntoa(peerServer.sin_addr));
				int recvBytes = read(acceptfd,recvBuf,sizeof(recvBuf));
				if(recvBytes <= 0)
				{
					printf("client disconnected.\n");
					printf("error\n");
					break;
				}
				printf("receive %d bytes\n",recvBytes);	
				for(i=0;i<recvBytes;i++)
				{
					printf("%02x",recvBuf[i]);
				}
				int x = unpack(recvBuf,recvBytes);
		//		printf("return bytes of func:%d\n",x);
			}
		}
		else
		{
			close(acceptfd);
		}
		
	}
	return 0;
}
int unpack(char buf[],int bytes)
{
	unsigned short crc16;
	float a,b,c,d,e;
	int i,tlv_len;
	for(i=0;i<bytes;i++)
	{
		if(buf[i]==TLV_HEAD)
		{
			if(bytes-i>2)
			{
				i++;
				i++;
				tlv_len=buf[i];
				i++;
				if(6<=tlv_len<=8)
				{
					if(tlv_len<32)
					{
						crc16=crc_itu_t(MAGIC_CRC,buf,buf[2]);
						ushort_to_bytes(&buf[bytes],crc16);
						if(crc16 != 0)
						{
							printf("value error\n");
							break;
						}
						int x = buf[i];
						i++;
						e = x;
						//printf("f=%d\n",e);
						a = (float)buf[i]/100;
						//printf("a=%f\n",a);
						i++;
						b = (float)buf[i]/10000;
						//printf("b=%f\n",b);
						i++;
						c = (float)buf[i]/1000000;
						//printf("c=%f\n",c);
						i++;
						printf("the temp of room:%f\n",a+b+e);
						break;
					}
					else
					{
						
						memmove(buf,&buf[i],bytes-i);
						printf("return byte3:%d\n",bytes-i);
						return bytes-i;
					}
				}
				else 
				{
					memmove(buf,&buf[2+1],bytes-i-3);
					bytes=bytes-i;
					printf("return bytes2:%d\n",bytes-i);
				}
			}
			else
			{
				memmove(buf,&buf[i],bytes-i);
				printf("return bytes1:%d\n",bytes-i);
				return bytes-i;
			}
		}	
	}
}
