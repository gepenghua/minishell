#include "parse.h"
#include "externs.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
 * shell��ѭ��
 */
void shell_loop(void)
{
	while (1)
	{
		printf("[minishell]$ ");
		fflush(stdout);//�������������ӡ�����
		init();
		if (read_command() == -1)
			break;
		parse_command();
		execute_command();
	}
	printf("\nexit\n");
}

/*
 * ��ȡ����
 * �ɹ�����0��ʧ�ܻ��߶�ȡ���ļ���������EOF)����-1
 */
int read_command(void)
{
	if (fgets(cmdline, MAXLINE, stdin) == NULL)
		return -1;
	return 0;
}

/*
 * ��������
 * �ɹ����ؽ����������������ʧ�ܷ���-1
 */
int parse_command(void)
{
	/* ��ʼ���룺ls -l */
	/* �����ﵽ�Ľ����ls\0-l\0 */
	char *cp = cmdline;
	char *avp = avline;
	int i = 0;

	while (*cp != '\0')
	{
		/* ȥ�������еĿո� */
		while (*cp == ' ' || *cp == '\t')	//���������еĿո�
			cp++;
		/* ���������β�˳�ѭ�� */
		if (*cp == '\0' || *cp == '\n') //if?
			break;
		cmd.args[i] = avp;

		/* �������� */
		while (*cp != '\0' 
			&& *cp != ' ' 
			&& *cp != '\t' 
			&& *cp != '\n')
			*avp++ = *cp++;
		/* ��ÿ���������һ��'\0' */
		*avp++ = '\0';
		printf("[%s]\n", cmd.args[i]);
		i++;
	}
	return 0;
}

/*
 *ִ������
 *�ɹ�����0��ʧ�ܷ���-1
 */
int execute_command(void)
{
	/* �����ӽ��� */
	pid_t pid = fork();
	if (pid == -1) //�����ӽ���ʧ��
		ERR_EXIT("fork");

	if (pid == 0)
		execvp(cmd.args[0], cmd.args); /*unistd.h ��int execvp(const char *file, char *const argv[]);*/
	wait(NULL);
	return 0;
}
