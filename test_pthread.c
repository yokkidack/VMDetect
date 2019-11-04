/*
pthread test
*/

#include <pthread.h>
#include <x86intrin.h>

#include <stdio.h>
int count;
//gcc -o test -std=c99 -lpthread test_pthread.c

void *loopy(){
    for (;;count++);
}

void *cpuidy(){
    unsigned eax, ebx, ecx, edx;
    eax = 1;
    unsigned i, j;
    __asm__ __volatile__("cpuid"
            :   "=a" (eax),
                "=b" (ebx),
                "=c" (ecx),
                "=d" (edx)
            :   "0"  (eax),
                "2"  (ecx));
    return 0;
}

int main (){

    pthread_t tid, ltid; /* идентификатор потока */
    pthread_attr_t attr, lattr; /* отрибуты потока */
    /* получаем дефолтные значения атрибутов */
    pthread_attr_init(&attr);
    pthread_attr_init(&lattr);


    /* создаем новый поток */
    pthread_create(&tid,&attr,cpuidy,NULL);
    pthread_create(&ltid,&lattr,loopy,NULL);


    /* ждем завершения исполнения потока */
    pthread_join(tid,NULL);
    pthread_cancel(ltid);
    printf("%d", count);
    return 0;
}
