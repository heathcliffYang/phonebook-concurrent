#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#include IMPL
#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (!fp) {
        printf("cannot open the file\n");
        return -1;
    }

#if defined(OPT)
    /* get the file size and malloc an entry pool and a detail pool*/
    char line1[MAX_LAST_NAME_SIZE];
    int lineNum;
    while (fgets(line1, sizeof(line1), fp)) {
        lineNum++;
    }
    entry *entry_pool = (entry *) malloc(sizeof(entry)*lineNum);
//    printf("entry_pool is at %p\n", entry_pool);

    /* build the entry */
    entry *pHead, *e;
    pHead = entry_pool;
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

    /* build the detail */
    detail detail_pool[4], d[4];
    for (int i = 0; i < 4; i++) {
	detail_pool[i] = mmap(NULL, sizeof(detail)*lineNum/4, PROT_READ, MAP_SHARED, fp, 0);
	d[i] = detail_pool[i];
    }
    pHead->pdetail = detail_pool[0];
//    printf("e->pdetail : %p\n", e->pdetail);

#else
    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;
#endif

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif

#if defined(OPT)

#ifndef THREAD_NUM
#define THREAD_NUM 4
#endif
    pthread_t tid[THREAD_NUM];

    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
	
    }
    for (int t = 0; t < THREAD_NUM; t++) {
	printf("In main: creating thread %d\n", t);
	pthread_create(&tid[t], NULL, append_detail(d[t], t),NULL);
    }
    /* rove the detail_pool arry */

    /* rove the detail_pool arry */

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    pthread_exit(NULL);
#else
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

#endif

    /* close file as soon as possible */
    fclose(fp);

    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
    /* munmap */
    for (int i = 0; i < THREAD_NUM; i++)
	munmap(detail_pool[i], sizeof(detail)*lineNum/THREAD_NUM);
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);
    return 0;
}
