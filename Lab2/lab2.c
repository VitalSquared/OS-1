#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main() {
    time_t now;
    struct tm *tm;

    putenv("TZ=PST");
    (void) time(&now);
    tm = localtime(&now);
    printf("%s - %s\n", tzname[tm->tm_isdst], asctime(tm));

    exit(0);
}
