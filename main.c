#include "parse.h"
#include "init.h"
#include "def.h"

char avline[MAXLINE + 1];	//�����б�����
char cmdline[MAXLINE + 1]; //cmdline��ŵ��Ƕ�ȡ��������
char *avptr;	//ָ��avline
char *lineptr;	//ָ��cmdline

char infile[MAXNAME + 1];  //�����ض���Ĵ洢������
char outfile[MAXNAME + 1]; //�洢����ض��������

COMMAND cmd[PIPELINE];	  //cmd��ŵ��ǽ�cmdline�����������

int cmd_count;
int backgnd;


int main(void)
{
	setup();
	shell_loop();
	return 0;
}