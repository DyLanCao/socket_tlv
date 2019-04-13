#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_DATA 4
#define TLV_HEAD 0xFD
enum type
{
	TAG1=0x01,
	TAG2,
	TAG3,
};
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
				for(i=0;i<MAX_DATA;i++)
				{
					if(recvBuf[i]==TLV_HEAD)
					{
						i++;
						if(recvBuf[i]==TAG1)
						{
							i++;
							if(recvBuf[i]==0x06)
							{
								i++;	
								int x = recvBuf[i];
								e = x;
								i++;
								a = (float)recvBuf[i]/100;
								i++;
								b = (float)recvBuf[i]/10000;
								i++;
								c = (float)recvBuf[i]/1000000;
								printf("the temperature of room is :%f\n",a+b+c+e);
								break;
							}
							else
							{
								printf("len of data error\n");
							}
							
						}
						else
							printf("tag data error\n");
						
					}
					else
						printf("head data error\n");
					i++;
				}
			}
		}
		else
		{
			close(acceptfd);
		}
		
	}
	return 0;
}
