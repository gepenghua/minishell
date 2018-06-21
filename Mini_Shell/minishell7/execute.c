#include "def.h"
#include "externs.h"
#include "execute.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>

void forkexec(int i)
{
	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fork"); //??
	if (pid > 0)
	{
		if (backgnd == 1)
			printf("%d\n", pid);
		/* ������ */
		lastpid = pid;

	}
	else if (pid == 0)
	{
		//backgnd == 1ʱ������һ���������infd�ض�����dev/null
		/* ����һ��������ͼ�ӱ�׼�����ȡ���ݵ�ʱ����������EOF*/
		if (cmd[i].infd == 0 && backgnd == 1)
			cmd[i].infd = open("/dev/null", O_RDONLY);	//??

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

int execute_disk_command(void)
{
	if (cmd_count == 0)
		return 0;

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
	else
		signal(SIGCHLD, SIG_DFL);

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