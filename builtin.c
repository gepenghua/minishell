#include "builtin.h"
#include "parse.h"
#include "externs.h"
#include <stdlib.h>
#include <stdio.h>


typedef void(*CMD_HANDLER)(void);	//����ָ������

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
 * �ڲ��������
 * ����1 ��ʾ���ڲ���� 0��ʾ�ⲿ����
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
	return 1;	//��������������ڲ���� ִ�н����󷵻�1��
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
	//������cmdline��
	printf("do_cd ...\n");
}

void do_type(void)
{
	printf("do_type\n");
}