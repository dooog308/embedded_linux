#include<signal.h>
#include<pthread.h>
#include "../header/Ctool.h"

struct sockaddr_in server_addr={0};
int sid=0, fid=0, addrlen=sizeof(server_addr);
char buf[256]={0};

void close_all(void)
{
	close(fid);
	close(sid);
}
int main(int argc, char *argv[])
{
	int ret=0;
	sid = socket(AF_INET, SOCK_STREAM, 0);
	if(sid==-1)
	{
		perror("socket create error");
		exit(-1);
	}
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ServerIP, &server_addr.sin_addr);
	server_addr.sin_port = htons(ServerPort);
	if(-1==connect(sid, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		close_all();
		perror("connect error");
		exit(-1);
	}
	printf("connect success!\n");
	while(1)
	{
		memset(buf, 0x00, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		if(!strncmp("-f", buf, 2)) send_RegFile(sid, substr(buf, 3, strlen(buf)-1), close_all);
		else if(!strncmp("-d", buf, 2)) send_Dir(sid, substr(buf, 3, strlen(buf)-1), close_all);	
		else if(!strncmp("-e", buf, 2)) 
		{
			Cpacket p={0};
			p.cmd = 'e';
			if(-1==write(sid, &p, sizeof(p)))
			{
				close_all();
				perror("send error");
				exit(-1);
			}
			break;
		}
	}
	close_all();
	exit(0);
}
	
