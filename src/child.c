// *test machine: apollo/vole
// * name: Gabe Campos , Tyler Johnson-Vasquez, Zachary Guldberg

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include<string.h>
#include<stdlib.h>
#include "utils.h"


/*
	Traverse the directory recursively and search for pattern in files.
	@params:
		name - path to the directory
		pattern - pattern to be recusrively searched in the directory
		
	Note: Feel free to modify the function header if neccessary
	
*/
void dirTraverse(const char *name, char * pattern)
{
	DIR *dir;
	struct dirent *entry;

	//Recursively traverse the directory and call SearchForPattern when neccessary	
	dir = opendir(name);
	if(dir == NULL){
		printf("Could not open %s\n", name);
		exit(0);
	}

	struct stat buf;

	while((entry = readdir(dir)) != NULL){
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
			continue;
		}

		// check to see if file is a soft link
		lstat(entry->d_name, &buf);
		
		if(entry->d_type == DT_LNK){
			continue;
		}

		char* newPath = malloc(sizeof(char) * MAX_PATH_LENGTH);
		*newPath = '\0';

		strcat(newPath, name);
		strcat(newPath, "/");
		strcat(newPath, entry->d_name);

		// recursive case - still in a directory
		if(entry->d_type == DT_DIR){
			dirTraverse(newPath, pattern);
		}
		//base case - at a file - or maybe files
		else{
			searchPatternInFile(newPath, pattern);
		}
		free(newPath);
	}
	closedir(dir);
}

int main(int argc, char** argv){

	if(argc !=3){
	
		fprintf(stderr,"Child process : %d recieved %d arguments, expected 3 \n",getpid(), argc);
		fprintf(stderr,"Usage child.o [Directory Path] [Pattern] \n");
		exit(EXIT_FAILURE);
	}
	
	char* path = argv[1];
	char* pattern = argv[2];
		

	close(STDIN_FILENO);

	dirTraverse(path,pattern);
	
	close(STDOUT_FILENO);
	
	exit(EXIT_SUCCESS);
}
