#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

#define ERR_SETENV -1

int main() {
    time_t now_sec;
    struct tm *now_full;

    int setenv_res = setenv("TZ", "PST8PDT", 1);
    if (setenv_res == ERR_SETENV) {
	perror("Unable to set TZ environment variable");
    }
	
    (void) time(&now_sec);
    printf("%s", ctime(&now_sec));     

    now_full = localtime(&now_sec);
    printf("%d/%d/%02d %d:%02d %s\n", 
		    now_full->tm_mon + 1, 
		    now_full->tm_mday, 
		    now_full->tm_year + 1900,
		    now_full->tm_hour, 
		    now_full->tm_min, 
		    tzname[now_full->tm_isdst]);
    exit(0);
}
