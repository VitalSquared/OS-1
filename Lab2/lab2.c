#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main() {
    time_t now;
    struct tm *tm;

    if (setenv("TZ","PST8PDT",1) == -1) {
	perror("Unable to set TZ");
    }
	
    (void) time(&now);
    printf("%s", ctime(&now));     

    tm = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n", 
		    tm->tm_mon + 1, 
		    tm->tm_mday, 
		    tm->tm_year, 
		    tm->tm_hour, 
	   	    tm->tm_min, 
		    tzname[tm->tm_isdst]);
    exit(0);
}
