#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "decorators.c"

#define	MAX_SIZE_CMD	256
#define	MAX_SIZE_ARG	16
#define MAX_SIZE_CWD  128

char cmd[MAX_SIZE_CMD];				// string holder for the command
char *argv[MAX_SIZE_ARG];			// an array for command and arguments
char cwd[MAX_SIZE_CWD];
pid_t pid;										// global variable for the child process ID
int i; 										    // global for loop counter

void get_cmd();								// get command string from the user
void convert_cmd();						// convert the command string to the required format by execvp()
void c_shell();								// to start the shell
const char* printDir();
const char* hostName();

int main(){

	// start the shell
	c_shell();
	return EXIT_SUCCESS; // returns 0;
}

const char* printDir() {
	return getcwd(cwd, sizeof(cwd));
}

const char* hostName() {
	return getenv("hostname") == NULL ? "terminal" : getenv("hostname");
}

void c_shell(){
	while(1){
		// get the command from user
		get_cmd();

		// bypass empty commands
		if(!strcmp("", cmd)) continue;

		// check for "exit" command
    if(!strcmp("exit", cmd)) break;

		// fit the command into *argv[]
		convert_cmd();

		// print current working directory
		if(!strcmp("cd", argv[0])) {
			// chdir(argv[1]);
			if(chdir(argv[1]) != 0) {
				red();
        printf("Directory doesn't exist");
				reset();
			}
			continue;
		}

		// custom clear command
		if(!strcmp("clear", argv[0])) {
			system("@cls||clear");
			continue;
		}

		// fork and execute the command
		pid = fork();
		if(pid < 0){
			red();
			printf("Failed to create a child process\n");
			reset();
		}
		else if(pid == 0){
			printf("Child's PID: %d\n",(int)getpid());
			printf("+--------------------+\n");
			// execute a command
			int status = execvp(argv[0], argv);
			if(status == -1) {
				red();
				printf("\nProgram terminated incorrectly!\n\n");
				reset();
			}
		}
		else{
			printf("+--------------------+\n");
			printf("Parent's PID: %d\n",(int)getpid());
			// wait for the command to finish 
			if(argv[i] == NULL) waitpid(pid, NULL, 0);
		}
	}
}

void get_cmd() {
	// get command from user
	green();
	printf("\n%s", getenv("USER"));
	purple();
	printf("@%s", hostName());
	blue();
	printf(" %s", printDir());
	reset();
	// fgets(cmd, MAX_SIZE_CMD, stdin); // if readline lib doesn't exist

	char* input = readline(">>> ");
 	// add to history so it can be accessed using up arrow key.
	add_history(input); 
	strcpy(cmd, input);
	free(input);

	// remove trailing newline
	if ((strlen(cmd) > 0) && (cmd[strlen(cmd) - 1] == '\n'))
    cmd[strlen (cmd) - 1] = '\0';
}

void convert_cmd() {
	// split string into argv
	char *ptr;
	i = 0;
	ptr = strtok(cmd, " ");
	while(ptr != NULL){
		// printf("%s\n", ptr);
		argv[i] = ptr;
		i++;
		ptr = strtok(NULL, " ");
	}
}
