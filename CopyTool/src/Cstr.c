#include "../header/Cstr.h"

char* substr(char* sstr,int start,int end)
{
	int i=0;
	rmtail(sstr, end);
	return sstr+start;
}
void rmtail(char *str,int ind)
{
	memset(str+ind, strlen(str)-ind-1, 0 );
	str[ind]='\0';
}
int findchrr(char *str, int c)
{
	int len=strlen(str), i=0;
	for(i=len;i>=0;i--)
	{
		if(str[i]==c) return i;
	}
	return -1;
}
