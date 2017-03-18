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

	printf("Scram Up!\n");
	
	// Get path to scramble from arg:
	if (argv[1] == NULL){
		printf("%s\n", "Usage: scram <dir_to_scram>");
		exit(0);
	} else {
		printf("Dir to Scram: %s...\n", argv[1]);
		path = argv[1];
	}

	DIR* FD;
    struct dirent* in_file;
    // FILE    *common_file;
    FILE    *current_file;
    char    buffer[BUFSIZ];

    // Seed rand once:
	srand(time(NULL));

    /* Openiing common file for writing */
    // common_file = fopen(path_to_your_common_file, "w");
    // if (common_file == NULL)
    // {
    //     fprintf(stderr, "Error : Failed to open common_file - %s\n", strerror(errno));

    //     return 1;
    // }

    /* Scanning the in directory */
    if (NULL == (FD = opendir (path))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        // fclose(common_file);

        return 1;
    }
    while ((in_file = readdir(FD))) 
    {
        /* On linux/Unix we don't want current and parent directories
         * On windows machine too, thanks Greg Hewgill
         */
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        /* Open directory entry file for common operation */
        /* TODO : change permissions to meet your need! */
        char path_temp[sizeof(path)];
        strcpy(path_temp, path);
        char *full_path = strcat(path_temp, in_file->d_name);
        // printf("FP: %s\n", full_path);
        current_file = fopen(full_path, "r+b");
        // printf("Trying to open %s...\n", full_path);
        if (current_file == NULL)
        {
        	// printf("Error w/ %s\n", full_path);
            fprintf(stderr, "Error : Failed to open current file - %s\n", strerror(errno));
            // fclose(common_file);

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
        	char c = (char)rand() % 255;
        	printf("char chosen: %c\n", c);


        	// Change the byte
        	fseek(current_file, sz, SEEK_SET);
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