#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "phonebook_opt.h"

#define THREAD_NUM 4

entry *findName(char lastName[], entry *pHead)
{
    while (pHead != NULL) {
	if (strcasecmp(lastName, pHead->lastName) == 0)
	    return pHead;
	pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    e->pNext = e + sizeof(entry);
//    printf("%p ;", sizeof(entry));
    e = e->pNext;
    printf("e is at %p, append %s, entry is %p\n", e->pNext, lastName, sizeof(entry));
    strcpy(e->lastName, lastName);

    return e;
}

void *append_detail(detail *pool, int t, int lineNum)
{
    for (int count_d = 1; count_d <= lineNum; count_d+=THREAD_NUM) {
//	pool->pdNext = pool + (count_d+t)*sizeof(detail);
	pool->pdNext = (detail *) malloc(sizeof(detail));
	pool = pool->pdNext;
	/* put the detail data in it at this step */
//	printf("Thread %d malloc %p finish! This is %dth name.\n", t, pool,count_d);
    }
    pthread_exit(pool);
}
