#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#define STAT_VALID 0

char *default_args[] = { ".", NULL };

void print_stats(char *name, struct stat *stat) {
	char mode[11] = { 0 };
	
	if (S_ISREG(stat->st_mode)) {
		mode[0] = '-';
	}
	else if (S_ISDIR(stat->st_mode)) {
		mode[0] = 'd';
	}
	else {
		mode[0] = '?';
	}

    	// user
    	mode[1] = (stat->st_mode & S_IRUSR) ? 'r' : '-';
    	mode[2] = (stat->st_mode & S_IWUSR) ? 'w' : '-';
    	mode[3] = (stat->st_mode & S_IXUSR) ? 'x' : '-';

    	// group
    	mode[4] = (stat->st_mode & S_IRGRP) ? 'r' : '-';
    	mode[5] = (stat->st_mode & S_IWGRP) ? 'w' : '-';
    	mode[6] = (stat->st_mode & S_IXGRP) ? 'x' : '-';

    	// others
    	mode[7] = (stat->st_mode & S_IROTH) ? 'r' : '-';
    	mode[8] = (stat->st_mode & S_IWOTH) ? 'w' : '-';
    	mode[9] = (stat->st_mode & S_IXOTH) ? 'x' : '-';

    	mode[10] = 0;

	printf("%s", mode);
	printf("\t%lu", stat->st_nlink);

	struct passwd* uent = getpwuid(stat->st_uid);
	struct group* gent = getgrgid(stat->st_gid);

    	if (uent == NULL)
		printf("\t%d", stat->st_uid);
    	else
		printf("\t%s", uent->pw_name);

    	if (gent == NULL)
		printf("\t%d", stat->st_gid);
    	else
		printf("\t%s", gent->gr_name);
	
	char lbuf[FILENAME_MAX] = { 0 };
	char fname[FILENAME_MAX];
    	strftime(lbuf, FILENAME_MAX, "%H:%M %e %b %Y", localtime(&(stat->st_ctime)));
    	printf("\t%lu\t%s\t%s\n", stat->st_size, lbuf, basename(name));
}

int main(int argc, char **argv) {
	char **args = (argc == 1) ? default_args : (argv + 1);
	for (int i = 0; args[i] != NULL; i++) {
		struct stat stat;
		int get_stat_res = lstat(args[i], &stat);
		if (get_stat_res == STAT_VALID) {
			print_stats(args[i], &stat);
		}
		else {
			perror(args[i]);
		}
	}
	return 0;
}
