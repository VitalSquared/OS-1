#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

#define ERR_SET_ENV_VAR -1

int main() {
    time_t now;
    struct tm *tm;

    int setenv_res = setenv("TZ", "PST8PDT", 1);
    if (setenv_res == ERR_SET_ENV_VAR) {
	perror("Unable to set TZ environment varialbe: ");
    }
	
    (void) time(&now);
    printf("%s", ctime(&now));     

    tm = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n", 
		    tm->tm_mon + 1, 
		    tm->tm_mday, 
		    tm->tm_year + 1900,
		    tm->tm_hour, 
		    tm->tm_min, 
		    tzname[tm->tm_isdst]);
    exit(0);
}
