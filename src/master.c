// *test machine: apollo/vole
// * name: Gabe Campos , Tyler Johnson-Vasquez, Zachary Guldberg

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"


int main(int argc, char** argv){

	if(argc != 3){

		fprintf(stderr,"Usage ./a.out [Path to Directory] [Pattern to search] \n");
		exit(EXIT_FAILURE);
	}
	char* path = argv[1];
	char* pattern = argv[2];
	
	//Declare any other neccessary variables
	DIR* root_dir;
	struct dirent* entry;
	pid_t pid;
	struct stat buf;
	int fds[2];
	int pipeResult = pipe(fds);
	
	//Open root directory
	root_dir = opendir(path);
	if(root_dir == NULL){
		printf("Failed to open %s\n", path);
		exit(0);
	}

	// Iterate through root dir and spawn children as neccessary
	while((entry = readdir(root_dir)) != NULL){

		lstat(entry->d_name, &buf);

		if(entry->d_type == DT_LNK){
			continue;
		}

		char* childPath = malloc(sizeof(char) * MAX_PATH_LENGTH);
		*childPath = '\0';

		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
			continue;
		}
		// only spawn children if directory
		if(entry->d_type == DT_DIR){
			pid = fork();
			if(pid < 0){
				printf("Failed to create child process\n");
				exit(0);
			}
			else if(pid == 0){
				// don't need read end of pipe
				close(fds[0]);
				dup2(fds[1], STDOUT_FILENO);

				strcat(childPath, path);
				strcat(childPath, "/");
				strcat(childPath, entry->d_name);

				execl("child", "child", childPath, pattern, NULL);
				printf("Error executing execl()\n");
				exit(0);
			}
			else{
				//parent
			}
		}
		// basic file in root directory
		else{
			strcat(childPath, path);
			strcat(childPath, "/");
			strcat(childPath, entry->d_name);

			searchPatternInFile(childPath, pattern);
		}
		free(childPath);
		//counter++;
	}

	closedir(root_dir);
	
	
	//Wait for all children to complete
	while((pid = wait(NULL)) > 0){	}

	close(fds[1]);
	dup2(fds[0], STDIN_FILENO);	

	// Read pipes of all children and print to stdout
	// Assumption : Pipe never gets full

		char* buffer = malloc(sizeof(char) * MAX_READ_LENGTH_BYTES);
		int bufferSize = sizeof(buffer);

		int bytes_read = read(STDIN_FILENO, buffer, MAX_READ_LENGTH_BYTES);

		if (bytes_read < 0) {
			fprintf(stderr, "ERROR: Failed to read from pipe\n");
			close(fds[0]);
			free(buffer);
		}

		// Print result
		printf("%s", buffer);
		free(buffer);

	close(fds[0]);
	
	return 0;
}
