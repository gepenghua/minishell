#include "parse.h"
#include "externs.h"
#include "init.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>

int execute_command(void);
void get_command(int i);
int check(const char *str);
void getname(char *name);
void print_command(); 
void forkexec(int i);

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
		print_command();
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
	/* cat < test.txt | grep -n public > test2.txt & */
	if (check("\n"))
	{
		return 0;
	}
	/* 1. ������һ�������� */
	get_command(0);
	/* 2. �ж��Ƿ��������ض���� < */
	if (check("<"))
		getname(infile);
	/* 3. �ж��Ƿ��йܵ� */
	int i ;
	for (i = 1; i < PIPELINE; i++)
	{
		if (check("|"))
			get_command(i);
		else
			break;
	}
	/* 4.�ж��Ƿ�������ض���� */
	if (check(">"))
	{
		if (check(">"))		//����ǡ�>>��������׷�ӵķ�ʽ
			append = 1;
		getname(outfile);
	}
	/* 5.�ж��Ƿ��̨��ҵ */
	if (check("&"))
		backgnd = 1;
	/* 6.�ж�������� */
	if (check("\n"))
	{
		cmd_count = i;
		return cmd_count;
	}
	else
	{
		fprintf(stderr, "Command line syntax error\n");
		return -1;
	}
}

/*
 *ִ������
 *�ɹ�����0��ʧ�ܷ���-1
 */
int execute_command(void)
{
	/* �����ӽ��� */
	/*
	pid_t pid = fork();
	if (pid == -1) //�����ӽ���ʧ��
		ERR_EXIT("fork");

	if (pid == 0)
		execvp(cmd.args[0], cmd.args); //unistd.h ��int execvp(const char *file, char *const argv[]);
	wait(NULL);
	*/
	
	if (infile[0] != '\0')
	{
		cmd[0].infd = open(infile, O_RDONLY);
	}
	if (outfile[0] != '\0')
	{
		if (append)
			cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);	//��׷�ӵķ�ʽ������ض�����ļ�???
		else
			cmd[cmd_count - 1].outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);	//������ķ�ʽ��???
	}

	/* ��Ϊ��̨��ҵ�������̲������wait() ���ȴ��ӽ����˳��� Ϊ�˱�������������̣�
	   ���Ժ����ź�SIGCHLD,�ӽ����˳�ʱ��������̷���һ��SIGCHLD���źţ������Ե���*/
	if (backgnd == 1)
		signal(SIGCHLD, SIG_IGN);

	int i;
	int fd;
	int fds[2];
	for (i = 0; i < cmd_count; i++)
	{
		/* ����������һ����� ����Ҫ�����ܵ� */
		if (i < cmd_count - 1)
		{
			pipe(fds);
			cmd[i].outfd = fds[1];
			cmd[i + 1].infd = fds[0];
		}

		forkexec(i);

		if ((fd = cmd[i].infd) != 0)	//infd=0, �������ض���
			close(fd);
		if ((fd = cmd[i].outfd) != 1)	//outfd=1, ������ض���
			close(fd);
	}
	if (backgnd == 0)
	{
		/* ǰ̨��ҵ�� ��Ҫ�ȴ��ܵ������һ�������˳� */
		while (wait(NULL) != lastpid)	//??
			;
	}

	return 0;
}

void print_command()
{
	int i;
	int j;
	printf("cmd_count = %d\n", cmd_count);
	if (infile[0] != '\0')
		printf("infile = [%s]\n", infile);
	if (outfile[0] != '\0')
		printf("outfile = [%s]\n", outfile);

	for ( i = 0; i < cmd_count; i++)
	{
		j = 0;
		while (cmd[i].args[j] != NULL)
		{
			printf("[%s] ", cmd[i].args[j]);
			j++;
		}
		printf("\n");
	}
}

/*������������ cmd[i] 
 *��ȡ���������avline������
 *���ҽ�COMMAND�ṹ�е�args[]�е�ÿ��ָ��ָ����Щ�ַ���
 */
void get_command(int i)
{
	/* cat < test.txt | grep -n public > test2.txt & */
	int j = 0;
	int inword;		//ifΪ1�� ��ʾ��<����ǰ������û�пո�ֱ������
	while(*lineptr != '\0')
	{
		//ȥ�����ͷ�Ŀո�
		while (*lineptr == ' ' || *lineptr == '\t')
			lineptr++;		// ??*
		/* ����ǰ��i������ĵ�ǰ��j������ָ��avptr */
		cmd[i].args[j] = avptr;// avline
		/* ��ȡ���� */
		while (*lineptr != '\0'
			&& *lineptr != ' '
			&& *lineptr != '\t'
			&& *lineptr != '<'
			&& *lineptr != '>'
			&& *lineptr != '|'
			&& *lineptr != '&'
			&& *lineptr != '\n')
		{
			/* ��ָ��lineptrָ��ģ���������ԭʼ�����У�������cmdline�Ĳ�����
			��ȡ�� avptr ָ��ָ������� avline */
			*avptr++ = *lineptr++;
			inword = 1;
		}
		*avptr++ = '\0';
		switch (*lineptr)
		{
			case ' ':
			case '\t':
				inword = 0;
				j++;
				break;
			case '>':
			case '<':
			case '|':
			case '&':
			case '\n':
				if (inword == 0)
					cmd[i].args[j] = NULL;
				return;
			default: //for'\0'
				return;
		}
	}
}

/*
 * ��lineptr�е��ַ�����str����ƥ��
 * �ɹ����� 1��lineptr �ƹ���ƥ����ַ�
 * ʧ�ܷ��� 0��lineptr ���ֲ���
 */
int check(const char *str)
{
	char *p;
	while (*lineptr == ' ' || *lineptr == '\t')
		lineptr++;
	p = lineptr;
	while (*str != '\0' && *str == *p)
	{
		str++;
		p++;
	}

	if (*str == '\0')
	{
		lineptr = p;	/* lineptr �ƹ���ƥ����ַ� */
		return 1;
	}
	return 0;
}
void getname(char *name)
{
	while (*lineptr == ' ' || *lineptr == 't')
		lineptr++;
	while (*lineptr != '\0'
		&& *lineptr != ' '
		&& *lineptr != '\t'
		&& *lineptr != '<'
		&& *lineptr != '>'
		&& *lineptr != '|'
		&& *lineptr != '&'
		&& *lineptr != '\n')
	{
		*name++ = *lineptr++;
	}
	*name = '\0';
}


void forkexec(int i)
{
	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fork"); //??
	if (pid > 0)
	{
		/* ������ */
		lastpid = pid;

	}
	else if (pid == 0)
	{
		//backgnd == 1ʱ������һ���������infd�ض�����dev/null
		/* ����һ��������ͼ�ӱ�׼�����ȡ���ݵ�ʱ����������EOF*/
		if (cmd[i].infd == 0 && backgnd == 1)
			cmd[i].infd == open("/dev/null", O_RDONLY);

		//����һ����������Ϊ��һ���������鳤
		if (i == 0)		
			setpgid(0, 0);
		/* �ӽ��� */
		if (cmd[i].infd != 0)		//outfd=0, �������ض���
		{
			close(0);		//??
			dup(cmd[i].infd);
		}
		if (cmd[i].outfd != 1)		//outfd=1, ������ض���
		{
			close(1);		//??
			dup(cmd[i].outfd);
		}
		int j;
		for (j = 3; j < FOPEN_MAX; j++)
			close(j);	//??
		if (backgnd == 0)
		{
			// �����ǰ̨��ҵ�� �ź�SIGINT�� ��Ҫ�ָ���Ĭ��ֵ 
			signal(SIGINT, SIG_DFL);
			//ǰ̨��ҵ�˳�Ҳ��Ҫ��ΪĬ��ֵ����Ҫctr+C / ctr+��/' ���˳���ǰǰ̨����
			signal(SIGQUIT, SIG_DFL);
		}
		execvp(cmd[i].args[0], cmd[i].args);
		exit(EXIT_FAILURE);
	}

}