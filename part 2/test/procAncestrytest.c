#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
};

#define __NR_cs3013_syscall2 378

int main() {
	pid_t pid1;
	pid_t pid2;
	pid_t pid3;
	int child = 0;
	int sib = 0;
	int parent = 0;
	struct ancestry *family = malloc(sizeof(struct ancestry));
	
	if ((pid1 = fork()) < 0) {
		perror("Could Not Fork1\n");
	} else if (pid1 == 0) {
		sleep(10);
		exit(0);
	} else {
		printf("PID1 %d\n", (int) pid1);
		if ((pid2 = fork()) < 0) {
			perror("Could Not Fork2\n");
		} else if (pid2 == 0) {
			sleep (10);
			exit(0);
		 } else {
			printf("PID2 %d\n", (int) pid2);
			if ((pid3 = fork()) < 0) {
				perror("Could Not Fork3\n");
			} else if (pid3 == 0) {
				sleep (10);
				exit(0);
			} else {
				printf("PID3 %d\n", (int) pid3);
				int sysCall = syscall(__NR_cs3013_syscall2, &pid2, family);
				if (sysCall != 0) {
					perror("SysCall Failed\n");
					return -1;
				} else {
					printf("Targeted Task PID: %d\n\n", (int) pid2);
					printf("Children\n");
					while(1) {
						if (family->children[child] < 1) {
							break;
						} else {
							printf("Targeted Child %d PID: %d\n", (child + 1), (int) family->children[child]);
							child++;
						}
					}
					printf("\nSiblings\n");
					while(1) {
						if (family->siblings[sib] < 1) {
							break;
						} else {
							printf("Targeted Sibling %d PID: %d\n", (sib + 1), (int) family->siblings[sib]);
							sib++;
						}
					}
					printf("\nAncestors\n");
					while(1) {
						if (family->ancestors[parent] < 1) {
							break;
						} else {
							printf("Targeted Parent %d PID: %d\n", (parent + 1), (int) family->ancestors[parent]);
							parent++;
						}
					}
				}
			}
		}
	}
	return 0;
}

						

