#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main() {
    time_t now;
    struct tm *tm;

    setenv("TZ","PST8PDT",1);
    (void) time(&now);
    tm = localtime(&now);
    printf("%s - %s\n", tzname[tm->tm_isdst], asctime(tm));

    exit(0);
}
