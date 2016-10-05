#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"

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
    printf("The pointer is at %p\n", e);
    e->pNext = e + sizeof(entry);
    printf("Now shift to %p\n", e->pNext);
    e = e->pNext;
    strcpy(e->lastName, lastName);

    return e;
}

void *append_detail(detail *pool, int t)
{
    for (int count_d = 1; count_d <= 4; count_d+4) {
	pool->pdNext = pool + (count_d+t)*sizeof(detail);
	pool = pool->pdNext;
    /* put the detail data in it at this step */
//    printf("Thread %d malloc %p finish!", t, pool);
    }
    pthread_exit(pool);
}
