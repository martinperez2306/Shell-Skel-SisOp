#include "builtin.h"

// returns true if the 'exit' call
// should be performed
int exit_shell(char* cmd) {

	//Your code here

	int ret = 0;
	char* aux = malloc(strlen(cmd));
	strcpy(aux,cmd);
	split_line(aux,SPACE);
	if(strcmp(aux,"exit") == 0){
		ret = 1;
	}

	free(aux);
	return ret;
}

// returns true if "chdir" was performed
// this means that if 'cmd' contains:
// 	$ cd directory (change to 'directory')
// 	$ cd (change to HOME)
// it has to be executed and then return true
int cd(char* cmd) {

	//Your code here

	char* aux = malloc(strlen(cmd));
	int ret = 0;
	
	strcpy(aux, cmd);
	char* directory;
	directory = split_line(aux,SPACE);
	if(strcmp(aux,"cd") == 0){
		ret = 1;
		int cdErr = 0;
		if(strlen(directory) != 0 && strlen(aux) > 2){
			cdErr = chdir(directory);
		}else{
			cdErr = chdir(getenv("HOME"));
			strcpy(promt,getenv("HOME"));
		}
		if(cdErr < 0){
			perror("Error");
		}
		char* pwd = get_current_dir_name();
		strcpy(promt,pwd);
		free(pwd);
	}

	free(aux);
	return ret;
}

// returns true if 'pwd' was invoked
// in the command line
int pwd(char* cmd) {

	// Your code here

	int ret = 0;
	char* aux = malloc(strlen(cmd));
	
	strcpy(aux,cmd);
	split_line(aux,SPACE);
	if(strcmp(aux,"pwd") == 0){
		ret = 1;
		//Al invocar este medoto, el mismo hace un malloc para setear el buffer, despues de usarlo
		//deberia hacer un free para limpiar la memoria.
		char* buffer = get_current_dir_name();
		printf("%s\n", buffer);
		free(buffer);
	}

	free(aux);
	return ret;
}

