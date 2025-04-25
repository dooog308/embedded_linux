#ifndef DEF_Ctool

#define DEF_Ctool

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<dirent.h>
#include "Cstr.h"

#define ServerPort 8888
#define PATH       "./receive"
#define ServerIP   "192.168.1.88"
#define MaxUser    5

/* Cpacket 是 socket 傳送時的基本封包格式
 * cmd: 指令代號，用來表示此封包的操作類型（如傳送檔案、資料夾等）
 * name: 檔案或資料夾的名稱
 * data_size: data 欄位中實際資料的大小（最多 512 bytes）
 * data: 實際傳送的資料內容
 */
typedef struct
{
	char cmd;
	char name[128];
	int data_size;
	char data[512];
}Cpacket;
/*
 * 傳送一般檔案給遠端端點（sid 為 socket 編號）
 * path: 欲傳送的檔案路徑
 * errfun: 發生錯誤時要執行的函式
 */
void send_RegFile(int sid, char *path, void(*errfun)(void));
/*
 * 傳送整個資料夾給遠端端點（包含資料夾內的檔案與子目錄）
 * path: 欲傳送的資料夾路徑
 * errfun: 發生錯誤時要執行的函式
 */
void send_Dir(int sid, char *path, void(*errfun)(void));
/*
 * 接收一個檔案，並儲存到指定的路徑中
 * sid: socket 編號
 * path: 儲存接收檔案的路徑
 * p: 初始接收到的封包（含檔名等基本資訊）
 * errfun: 發生錯誤時要執行的函式
 */
void receive_RegFile(int sid, char *path, Cpacket *p, void(*errfun)(void));
/*
 * 建立並接收整個資料夾的內容（與 send_Dir 搭配使用）
 * path: 資料夾儲存的路徑
 * errfun: 發生錯誤時要執行的函式
 */
void receive_Dir(char *path, void(*errfun)(void));

#endif
