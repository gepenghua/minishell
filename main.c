#include "parse.h"
#include "init.h"
#include "def.h"

char avline[MAXLINE + 1];
char cmdline[MAXLINE + 1]; //cmdline��ŵ��Ƕ�ȡ��������
COMMAND cmd;			   //cmd��ŵ��ǽ�cmdline�����������


int main(void)
{
	setup();
	shell_loop();
	return 0;
}