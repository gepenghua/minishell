#include "builtin.h"
#include "parse.h"
#include "externs.h"
#include <stdlib.h>
#include <stdio.h>


typedef void(*CMD_HANDLER)(void);	//函数指针类型

typedef struct builtin_cmd
{
	char *name;
	CMD_HANDLER handler;
}BUILTIN_CMD;


void do_exit(void);
void do_cd(void);
void do_type(void);

BUILTIN_CMD builtins[]=
{
	{"exit", do_exit},
	{"cd", do_cd},
	{"type", do_type},
	{NULL, NULL}
};

/*
 * 内部命令解析
 * 返回1 表示是内部命令， 0表示外部命令
 */
int builtin(void)
{
	/*
	if (check("exit"))
		do_exit();
	else if (check("cd"))
		do_cd();
	else
		return 0;
	return 1;	//如果是上述两个内部命令， 执行结束后返回1；
	*/

	int i = 0;
	int found = 0;
	while (builtins[i].name != NULL)
	{
		if (check(builtins[i].name))
		{
			builtins[i].handler();
			found = 1;
			break;
		}
		i++;
	}
	return found;
}

void do_exit(void)
{
	printf("exit\n");
	exit(EXIT_SUCCESS);
}

void do_cd(void)
{
	//参数在cmdline中
	printf("do_cd ...\n");
}

void do_type(void)
{
	printf("do_type\n");
}