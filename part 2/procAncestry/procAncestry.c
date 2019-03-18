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

#define __NR_cs3013_syscall2 378 // This is the syscall number in the grading kernel, in the proj0 kernel its 334

int main(int argc, char *argv[]) {
	int child = 0;
	int sib = 0;
	int parent = 0;
	if (argc != 2) { // pid check
		perror("Please Specify a PID\n");
		return -1;
	}
	pid_t pid = atoi(argv[1]);
	struct ancestry *family = malloc(sizeof(struct ancestry));
	int sysCall = syscall(__NR_cs3013_syscall2, &pid, family); // replace malloced family with values from syscall
	if (sysCall != 0) {
		perror("SysCall Failed\n");
		return -1;
	} else {
		printf("Targeted Task PID: %d\n\n", (int) pid);
		printf("Children\n");
		while(1) {
			if (family->children[child] < 1) { // check for valid pid
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
	return 0;
}

						

