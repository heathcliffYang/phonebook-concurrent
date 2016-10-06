#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


#include IMPL
#define DICT_FILE "./dictionary/words.txt"
#define THREAD_NUM 4

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
    int fd = open(DICT_FILE, O_RDWR | O_NONBLOCK);
    FILE *fp_count = fopen(DICT_FILE,"r");
    /* get the file size and malloc an entry pool and a detail pool*/
    char line1[MAX_LAST_NAME_SIZE];
    int lineNum;
    while (fgets(line1, sizeof(line1), fp_count)) {
        lineNum++;
    }

    printf("The lineNum : %d\n", lineNum);

    char *map = mmap(NULL, sizeof(line1)*lineNum, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    entry *entry_pool = (entry *) malloc(sizeof(entry)*lineNum*64);
//    printf("entry_pool is at %p\n", entry_pool);

    /* build the entry */
    entry *pHead, *e;
    pHead = entry_pool;
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

    /* build the detail */
    detail *detail_pool[THREAD_NUM], *d[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
//	detail_pool[i] = (detail *) malloc(sizeof(detail)*lineNum/THREAD_NUM);
	detail_pool[i] = (detail *) malloc(sizeof(detail));
	d[i] = detail_pool[i];
    }
    pHead->pdetail = detail_pool[0];
    printf("e->pdetail : %p\nd[0]=%p\nd[1]=%p\nd[2]=%p\nd[3]=%p\n", e->pdetail, d[0], d[1], d[2], d[3]);

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
    int concurrency = pthread_setconcurrency(THREAD_NUM+1);
    pthread_t tid[THREAD_NUM];

    clock_gettime(CLOCK_REALTIME, &start);

    int a;

    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
        a++;
	printf("%d : ", a);
    }

//    for (int t = 0; t < THREAD_NUM; t++) {
//	printf("In main: creating thread %d\n", t);
	pthread_create(&tid[0], NULL, append_detail(d[0], 0, lineNum), NULL);
	pthread_create(&tid[1], NULL, append_detail(d[1], 1, lineNum), NULL);
	pthread_create(&tid[2], NULL, append_detail(d[2], 2, lineNum), NULL);
	pthread_create(&tid[3], NULL, append_detail(d[3], 3, lineNum), NULL);
//    }
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
