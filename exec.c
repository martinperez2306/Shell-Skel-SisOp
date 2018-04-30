#include "exec.h"

// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void get_environ_key(char* arg, char* key) {

	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void get_environ_value(char* arg, char* value, int idx) {

	int i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void set_environ_vars(char** eargv, int eargc) {

	// Your code here
	int p;
		for(int i = 0; i < eargc; i++){
			char* varEnv = eargv[i];
			int idx = block_contains(varEnv,'=');
			if(idx != -1){
				setEnvVar = 1;
				char* key = malloc(idx);
				char* value = malloc(strlen(varEnv) - (idx + 1));
				get_environ_key(varEnv,key);
				get_environ_value(varEnv,value,idx);
				int set = setenv(key,value,0);
				if(set < 0){
					perror("SetEnv");
				}
				free(key);
				free(value);
			}
		}
} 

// opens the file in which the stdin/stdout or
// stderr flow will be redirected, and returns
// the file descriptor
// 
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int open_redir_fd(char* file) {

	// Your code here
	return -1;
}

//Execute the command searching programs in PATH
//author: Martin Perez
void execute(struct cmd* cmd){
	//Cast struct cmd* to struct execcmd* to use this interface
    struct execcmd* execcmd = (struct execcmd*) cmd;

    //Set Enviroment Variables
    set_environ_vars(execcmd->eargv, execcmd->eargc);
	//Si le paso el execcmd->scmd me va a guardar el comando con parametros y todo
	//Debo pasarle execcmd->argv[0]
	//Example ls -l -a -> execcmd->scmd = "ls -l -a"; execcmd->argv = {"ls","-l","-a",(char*)NULL}
	execvp(execcmd->argv[0],execcmd->argv);
	//En caso de que no haya programa que ejecutar salgo del proceso
	_exit(-1);
}

//Execute the command in background
//author: Martin Perez
void background(struct cmd* cmd){
	//Cast struct cmd* to struct backmcmd*
	struct backcmd* backcmd = (struct backcmd*) cmd;
	//Call execute with struct cmd* contains in backcmd*
	execute(backcmd->c);
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd* cmd) {

    	
	switch (cmd->type) {

		case EXEC:
			// spawns a command
			//
			// Your code here
			execute(cmd);
			//printf("Commands are not yet implemented\n");
			//_exit(-1);
			break;

		case BACK: {
			// runs a command in background
			//
			// Your code here
			background(cmd);
			//printf("Background process are not yet implemented\n");
			//_exit(-1);
			break;
		}

		case REDIR: {
			// changes the input/output/stderr flow
			//
			// Your code here
			printf("Redirections are not yet implemented\n");
			_exit(-1);
			break;
		}
		
		case PIPE: {
			// pipes two commands
			//
			// Your code here
			printf("Pipes are not yet implemented\n");
				
			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);

			break;
		}
	}
}

