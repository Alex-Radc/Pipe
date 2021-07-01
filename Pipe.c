#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO1 "/tmp/fifo.1"
#define MAXLINE 80

FILE *fp;

void doit(void)
{
	pid_t pid;
	int status = 0;
	FILE *readfd;
	FILE *writefp;
	char buff[MAXLINE] = { '\0' };
	char str[MAXLINE] = { '\0' };
	ssize_t len, n;

	mkfifo(FIFO1, 0666);
	pid = fork();
	if (-1 == pid)
	{
		perror("fork"); /* произошла ошибка */
		exit(1); /*выход из родительского процесса*/
	}
	else if (0 == pid)
	{
		printf(" CHILD: Это процесс-потомок!\n");
		printf("Enter string\n");
		scanf("%s", str);

		len = strlen(str) + 1;
		if ((writefp = fopen(FIFO1, "w")) == NULL)
		{
			printf("Cannot open file\n");
			exit(1);
		}
		//sleep(5);
		/* запись в канал IPC */

		fwrite(str, sizeof(char), len, writefp);
		close(writefp);
		printf("Child finished\n");
		exit(status); /* выход из процесс-потомока */
	}
	else
	{
		printf("PARENT: Это процесс-родитель!\n");
		if ((readfd = fopen(FIFO1, "r")) == NULL)
		{
			printf("Cannot open file\n");
			exit(1);
		}
		/* считывание из канала */
		while ((n = fread(buff, sizeof(char), MAXLINE, readfd)) > 0);
		printf("buff = %s\n", buff);
		close(readfd);
		if (wait(&status) == -1)
		{
			perror("wait() error");
		}
		else if (WIFEXITED(status))
		{
			printf("PARENT: Код возврата потомка: %d\n", WEXITSTATUS(status));
		}
		else
		{
			perror("PARENT: потомок не завершился успешно");
		}
	}
}

int main(int argc, char **argv)
{
	doit();
	return 0;
}
