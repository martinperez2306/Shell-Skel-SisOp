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
		for(int i = 0; i < eargc; i++){
			char* varEnv = eargv[i];
			int idx = block_contains(varEnv,'=');
			if(idx != -1){
				setEnvVar = 1;
				char* key = malloc(idx);
				char* value = malloc(strlen(varEnv) - (idx + 1));
				get_environ_key(varEnv,key);
				get_environ_value(varEnv,value,idx);
				int set = setenv(key,value,1);
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
	int execError = execvp(execcmd->argv[0],execcmd->argv);
	//En caso de que no haya programa que ejecutar salgo del proceso
	if(execError < 0){
		printf("Error al ejecutar el comando\n");
	}
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

void redirection(struct cmd* cmd){
	//Cast struct cmd* to struct execcmd* to use this interface
    struct execcmd* execcmd = (struct execcmd*) cmd;
    //Initializing file descriptors
    //Old FD
    int ofd = 0;
    int ifd = 0;
    int efd = 0;
    //New FD
    int newIfd = 0;
	int newOfd = 0;
	int newEfd = 0;
    if(strlen(execcmd->in_file) > 0){
		if((ifd = open(execcmd->in_file,O_RDONLY)) < 0){
			perror("Error in input file");
		}
    }
    if(strlen(execcmd->out_file) > 0){
    	//Condicion para redireccionar truncando archivos '>' o concatenando '>>'
    	if(execcmd->out_file[0] == '>'){
    		strcpy(execcmd->out_file, execcmd->out_file + 1);
    		if((ofd = open(execcmd->out_file,O_CREAT|O_RDWR|O_APPEND,S_IRUSR|S_IWUSR)) < 0){
    			perror("Error in output file");
    		}
    	}else{
    		if((ofd = open(execcmd->out_file,O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR)) < 0){
    			perror("Error in output file");
    		}
    	}	
    }
    if(strlen(execcmd->err_file) > 0){
    	//Condicion para redireccionar el error estandar a la salida estandar '&>'
    	if(execcmd->err_file[0] == '&'){
    		newEfd = dup2(STDOUT_FILENO,STDERR_FILENO);
    	}
    	else{
    		if((efd = open(execcmd->err_file,O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR)) < 0){
    			perror("Error in error file");
    		}
    	}
    }
	if(ifd < 0 || ofd < 0 || efd < 0){
		close(ifd);
		close(ofd);
		close(efd);
		_exit(-1);		
	}else{
		if(ofd > 0){
			if(newEfd > 0){
				//Si redireccione el error estandar a la salida estandar anteriormente
				//Debo redireccionar ese resultado de la salida estandar al archivo
				newEfd = dup2(ofd,STDERR_FILENO);
			}
			newOfd = dup2(ofd,STDOUT_FILENO);			
			close(ofd);
		}
		if(ifd > 0){
			newIfd = dup2(ifd,STDIN_FILENO);
			close(ifd);
		}
		if(efd > 0){
			newEfd = dup2(efd,STDERR_FILENO);
			close(efd);
		}		
		if(newIfd < 0 || newOfd < 0 || newEfd < 0){
			perror("Error");
			_exit(-1);
		}else{
			execute(cmd);
		}
	}  
}

void run_pipe(struct cmd* cmd){

	//Cast struct cmd* to struct pipecmd* to use this interface
	struct pipecmd* pipecmd = (struct pipecmd*) cmd;
	int pipefd[2];
	int pipeStatus = 0;
	pid_t pid = 0;
	int status = 0;

	//Hago un pipe para generar los dos file descriptors
	pipeStatus = pipe(pipefd);

	if(pipeStatus == -1){
		perror("Error");
	}

	//Hago un fork para crear un proceso hijo el cual copia todo el contexto del padre
	if((pid = fork()) == 0){
		//Cierro el file descriptor que no usa el hijo (read end)
		if(close(pipefd[0]) == -1){
			perror("Error INPUT");
		}
		//El hijo va a ejecutar un programa redirigiendo lo que escribe en
		//salida estandar a la parte de escritura del pipe
		dup2(pipefd[1],STDOUT_FILENO);
		//Cierro el file descriptor que uso el hijo (write end)
		if(close(pipefd[1]) == -1){
			perror("Error OUTPUT");
		}
		//Ejecuto el left del pipe
		execute(pipecmd->leftcmd);
	}else{
		//espero a que termine el hijo
		waitpid(pid,&status,0);
		//Cierro el file descriptor que no usa el padre (write end)
		if(close(pipefd[1]) == -1){
			perror("Error OUTPUT");
		}
		//El padre va a ejecutar un programa redirigiendo lo que lee en
		//entrada estandar a la parte de lectura del pipe
		dup2(pipefd[0],STDIN_FILENO);
		//Cierro el file descriptor que uso el padre (read end)
		if(close(pipefd[0]) == -1){
			perror("Error INPUT");
		}
		//Ejecuto el right del pipe
		execute(pipecmd->rightcmd);
	}
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
			redirection(cmd);
			//printf("Redirections are not yet implemented\n");
			//_exit(-1);
			break;
		}
		
		case PIPE: {
			// pipes two commands
			//
			// Your code here
			//printf("Pipes are not yet implemented\n");
			run_pipe(cmd);	
			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);

			break;
		}
	}
}

