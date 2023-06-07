// *test machine: apollo/vole
// * name: Gabe Campos , Tyler Johnson-Vasquez, Zachary Guldberg

#include<stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include "utils.h"


/*
	Searches for pattern in file and writes the entire line containing the pattern to STDOUT
	Format for print: [path to file] : Line in the file
	@Params:
		path - path to the file
		pattern : pattern to be searched
*/
void searchPatternInFile(char* path, char* pattern){

	FILE * fp;

	fp = fopen(path, "r");
	if (fp == NULL){
		fprintf(stderr,"Error opening file: %s \n",path);
		return;
	}
	
	//Read file line by line and check if pattern exists in the line
	char* line = NULL;
	size_t len = 0;
	ssize_t line_size;
	char* found;

	while((line_size = getline(&line, &len, fp)) != -1){
		if(line_size < 0){
			printf("Failed to read line from file\n");
			fclose(fp);
			exit(0);
		}
		
		found = strstr(line, pattern);
		// line contains the word we're looking for
		if(found){
			char* printPath = malloc(sizeof(char) * MAX_PATH_LENGTH);
			*printPath = '\0';
			strcat(printPath, path);
			strcat(printPath, ": ");
			strcat(printPath, line);

			write(STDOUT_FILENO, printPath, strlen(printPath));
			
			free(printPath);
		}		
	}
		
	//Close fp
	fclose(fp);

}
