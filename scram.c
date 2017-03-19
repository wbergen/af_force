// Technique relies heavily upon:
// http://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>


char *path;

int main(int argc, char *argv[])
{

    char cwd[1024];

	// printf("Scram Up!\n");
	
	// Get path to scramble from arg:
	if (argv[1] == NULL){
		printf("%s\n", "Usage: scram <dir_to_scram>");
		exit(0);
	} else {
		printf("Dir to Scram: %s...\n", argv[1]);
		path = argv[1];
	    getcwd(cwd, 1024);
	}

	// Dir/fd structs/defines:
	DIR* FD;
    struct dirent* in_file;
    FILE    *current_file;

    // Seed rand once:
	srand(time(NULL));

    // Make Path:
    char *full_path = malloc(1024);
	if (strncmp(path, "/", 1) == 0) {
		printf("Absolute Path Detected...\n");
		strcat(full_path, path);
		strcat(full_path, "/");

	} else {
		printf("Relative Path Detected...\n");
	    strcat(full_path, cwd);
	    strcat(full_path, "/");
	    strcat(full_path, path);
	    strcat(full_path, "/");
	}



    // printf("%s\n", full_path);

    /* Scanning the in directory */
    if ((FD = opendir(full_path)) == NULL) {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return 1;
    }

  
    // Iterate over files in dir:
    while ((in_file = readdir(FD))) {
        // Skip FS links:
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;

        char path_temp[1024];
        strcpy(path_temp, full_path);
        strcat(path_temp, in_file->d_name);;


        printf("FP: %s\n", path_temp);
        current_file = fopen(path_temp, "r+b");
        // printf("Trying to open %s...\n", full_path);
        if (current_file == NULL)
        {
        	// printf("Error w/ %s\n", full_path);
            fprintf(stderr, "Error : Failed to open current file - %s\n", strerror(errno));
            return 1;
        } else {
        	// Success!  Do stuff w/ file:
        	printf("Opened %s", full_path);

        	// Determine size:
        	fseek(current_file, 0L, SEEK_END);
			int sz = ftell(current_file);
			rewind(current_file);
        	printf("w/ size %i\n", sz);

        	// Choose a random byte within bounds:
        	int spot = rand() % (sz + 1);
        	printf("rand spot: %i\n", spot);

        	// Choose random byte for replace:
        	char c = (char)(rand() % 255);
        	printf("char chosen: %c\n", c);


        	// Change the byte:
        	fseek(current_file, spot, SEEK_SET);
		    fputc(c, current_file);
        }

        /* Doing some struf with current_file : */
        /* For example use fgets */
        // while (fgets(buffer, BUFSIZ, current_file) != NULL)
        // {
            /* Use fprintf or fwrite to write some stuff into common_file*/
        // }

        /* When you finish with the file, close it */
        fclose(current_file);
        // full_path = NULL;
    }

    /* Don't forget to close common file before leaving */
    // fclose(common_file);



	return 0;
}