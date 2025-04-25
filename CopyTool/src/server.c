#include<signal.h>
#include<pthread.h>
#include "../header/Ctool.h"
//使用multithread來實現同時與多個user連接的功能

/*empty: 使用位元來標記可用的 thread slot，1 = 使用中，0 = 空位
 *eind:  下一個可用的空位的index
 *lock:  自製簡易鎖，防止 race condition，因為thread比較少，所以沒使用spin_lock或mutex_lock
*/
int empty=0, eind=0, lock=0;
/*server_addr: server socket 地址資訊
 *ssid:        server socket id（監聽 socket）
 *sfid:        保留未使用，第一版沒有傳送資料夾的功能，資料統一存到sfid
 *addrlen:     sockaddr_in 結構大小
*/
struct sockaddr_in server_addr={0};
int ssid=0, sfid=0, addrlen=sizeof(server_addr);

/*sid:       user socket id
 *tid:       user thread id
 *user_addr: user socket addr
 *user_ip:   user ip
*/
typedef struct{
	int sid;
	pthread_t tid;
	struct sockaddr_in user_addr;
	char user_ip[35];
}thread_t;
thread_t td[MaxUser]={0};

void* WaitRead(void*);

//尋找一個空的 thread index，設定到 eind
void find_eind(void)
{
	while(lock);
	lock = 1;
	int i=0;
	for(i=0;i<MaxUser;i++)
	{
		if(!(empty&(1<<i)))
		{
			eind = i;
			break;
		}
	}
	lock = 0;
}
//關閉所有 socket（用於錯誤或結束）
void close_all(void)
{
	int i=0;
	close(sfid);
	close(ssid);
	for(i=0;i<MaxUser;i++)
	{
		if(td[i].sid!=0) close(td[i].sid);
	}	
}
//等待user的連線並開啟一條thread
void con(void)
{
	int ind=0, ret=0;
	while(lock);
	lock=1;
	ind = eind;
	empty |= (1<<eind); //標記為已使用
	eind=-1; //代表目前沒位置
	lock=0;
	td[ind].sid = accept(ssid, (struct sockaddr*)&td[ind].user_addr, &addrlen);
	if(td[ind].sid==-1)
	{
		close_all();
		perror("accept error");
		exit(-1);
	}	
	ret = pthread_create(&td[ind].tid, NULL, WaitRead, (void*)ind);
	if(0!=ret)
	{
		close_all();
		printf("thread create error: %d\n", ret);
		exit(-1);
	}
	ret = pthread_detach(td[ind].tid);
	if(0!=ret)
	{
		close_all();
		printf("thread detach error: %d\n", ret);
		exit(-1);
	}
}
//每個user對應的thread執行函式
void* WaitRead(void* n)
{
	int ind=(int)n,ret=-1;
	Cpacket p={0};
	char path[256]={0};
	strcat(path, PATH);//初始的路徑是./receive
	inet_ntop(AF_INET, &td[ind].user_addr.sin_addr.s_addr,td[ind].user_ip,20);
	printf("a user connect.\n");
	printf("user ip: %s\n", td[ind].user_ip);
	printf("user port:%d\n", td[ind].user_addr.sin_port);
	path[strlen(path)] = (char)(ind+'0');//每個thread對應的目錄
	path[strlen(path)] = '\0';
	if(0!=mkdir(path, 0777))
	{
		//close_all();
		//perror("create direction error");
		//exit(-1);
	}
	while(1)
	{
		ret = read(td[ind].sid, &p, sizeof(p));
		if(-1==ret)
		{
			close_all();
			perror("receive error");
			exit(-1);
		}
		// 若傳來的路徑內包含 '/'，就抓最後的檔名
		if(findchrr(p.name, '/')!=-1)strcat(path, strrchr(p.name, '/'));
		if(p.cmd=='e') break;
		else if(p.cmd=='d') receive_Dir(path, close_all);
		else if(p.cmd==(char)('f'+1)) //user離線
				rmtail(path, findchrr(path, '/'));
		else if(p.cmd=='f') receive_RegFile(td[ind].sid, path, &p, close_all);
	}
	while(lock);
	lock = 1;
	empty &= (0<<ind);
	lock = 0;
	printf("a user unconnect.\n");
	printf("user ip: %s\n", td[ind].user_ip);
	printf("user port:%d\n", td[ind].user_addr.sin_port);
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	ssid = socket(AF_INET, SOCK_STREAM, 0);
	if(ssid==-1)
	{
		perror("socket create error");
		exit(-1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ServerPort);
	if(-1==bind(ssid, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		close_all();
		perror("bind error");
		exit(-1);
	}
	if(-1==listen(ssid, MaxUser))
	{
		close_all();
		perror("listen error");
		exit(-1);
	}
	while(1)
	{
		if(eind==-1)
		{
			printf("server is full\n");
			while(eind==-1);
		}
		con();
		find_eind();
	}
	close_all();
	exit(0);
}
	
