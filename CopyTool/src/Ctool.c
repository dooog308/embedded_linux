#include "../header/Ctool.h"


void send_RegFile(int id, char* path, void(*errfun)(void))
{
	FILE *in=NULL;
	int ret=0;
	Cpacket p={0};
	in=fopen(path, "r");	
	if(in==NULL)
	{
		errfun();
		perror("open file error");
		exit(-1);
	}
	printf("send %s...\n", path);
	p.cmd = 'f';//'f'代表之後傳的是RegFile
	strcat(p.name, strrchr(path, '/'));//只擷取檔案的名子，路徑由接收端決定
	//重複傳送直到ret=0，ret代表實際傳送的資料大小(Byte)
	do{
		ret = fread(p.data, 1, sizeof(p.data), in);
		p.data_size = ret;
		printf("%d\n", ret);
		if(-1==write(id, &p, sizeof(p))&&ret!=0)
		{
			errfun();
			perror("send error");
			exit(-1);
		}
	}while(ret!=0);//因為檔案結束時最後一個封包的data_size=0,所以不需要特別傳't'+1(已完成指令)。
	fclose(in);
}
void send_Dir(int id, char *path, void(*errfun)(void))
{
	DIR *d=NULL;
	struct dirent *dr=NULL;
	Cpacket p={0};
	d = opendir(path);
	if(d==NULL)
	{
		errfun();
		perror("open direction error");
		exit(-1);
	}	
	p.cmd = 'd';//'d'代表傳送的是目錄
	printf("send %s...\n", path);
	strcat(p.name, strrchr(path, '/'));//只擷取目錄的名子，路徑由接收端決定
	if(-1==write(id, &p, sizeof(p)))//先傳送母目錄的名稱
	{
		errfun();
		perror("send error");
		exit(-1);
	}
	dr = readdir(d);
	while(dr!=NULL)//遞迴傳送資料夾的內容
	{
		strcat(path, "/");
		strcat(path, dr->d_name);
		switch(dr->d_type)
		{
			case DT_DIR:
			{
				if(dr->d_name[0]!='.') send_Dir(id, path, errfun);
				break;
			}
			case DT_REG:
			{
				send_RegFile(id, path, errfun);
				break;
			}
		}
		rmtail(path, findchrr(path, '/'));
		dr = readdir(d);
	}
	p.cmd = (char)('f'+1);//'f'+1代表已完成上一個指令
	if(-1==write(id, &p, sizeof(p)))
	{
		errfun();
		perror("send error");
		exit(-1);
	}
	printf("done\n");
	closedir(d);
}
void receive_RegFile(int id, char *path, Cpacket *p, void(*errfun)(void))
{
	FILE *f=NULL;
	int ret=0;
	f = fopen(path, "a");//存放接收到的檔案，如果path不存在就創造一個
	if(f==NULL)
	{
		errfun();
		perror("create file error");
		exit(-1);
	}
	printf("copy %s...\n", path);
	do{//寫入
		if(-1==fwrite(p->data, 1, p->data_size, f))
		{
			errfun();
			perror("write error");
			exit(-1);
		}
		ret = read(id, p, sizeof(*p));
	}while(p->data_size!=0);
	rmtail(path, findchrr(path, '/'));
	fclose(f);
	printf("done.\n");
}
void receive_Dir(char *path, void(*errfun)(void))//需要做的只有創造一個資料夾
{
	if(0!=mkdir(path, 0777))
	{
		//errfun();
		//perror("create direction error");
		//exit(-1);
	}
}
