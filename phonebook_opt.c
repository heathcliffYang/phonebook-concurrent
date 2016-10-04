#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"
#include "debug.h"

entry *findName(char lastname[], entry *pHead)
{
    size_t len = strlen( lastname);
    while (pHead != NULL) {
        if (strncasecmp(lastname, pHead->lastName, len) == 0
                && (pHead->lastName[len] == '\n' ||
                    pHead->lastName[len] == '\0')) {
            pHead->lastName = (char *) malloc( sizeof(char) * MAX_LAST_NAME_SIZE);
            memset(pHead->lastName, '\0', MAX_LAST_NAME_SIZE);
            strcpy(pHead->lastName, lastname);
            pHead->dtl = (pdetail) malloc( sizeof( detail));
            return pHead;
        }
        dprintf("find string = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
//    printf("The pointer is at %p\n", e);
    e->pNext = e + sizeof(entry);
//    printf("Now shift to %p\n", e->pNext);
    e = e->pNext;
    strcpy(e->lastName, lastName);

    return e;
}

void *append_detail(detail *pool, int t)
{
    static int count_d = 1;
    pool->pdNext = pool + (count_d+t)*sizeof(detail);
    pool = pool->pdNext;
    /* put the detail data in it at this step */
    printf("Thread %d malloc %p finish!", t, pool);
    count_d+=4;
    pthread_exit(NULL);
}
