/*
pthread test
*/
#include <unistd.h>
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
    cpu_set_t cpus;

    /* получаем дефолтные значения атрибутов */
    pthread_attr_init(&attr);
    pthread_attr_init(&lattr);
    CPU_ZERO(&cpus);
    CPU_SET(0, &cpus);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
    CPU_ZERO(&cpus);
    CPU_SET(1, &cpus);
    pthread_attr_setaffinity_np(&lattr, sizeof(cpu_set_t), &cpus);

    /* создаем новый поток */
    pthread_create(&tid,&attr,cpuidy,NULL);
    pthread_create(&ltid,&lattr,loopy,NULL);


    /* ждем завершения исполнения потока */
    pthread_join(tid,NULL);
    pthread_cancel(ltid);
    printf("%d", count);
    return 0;
}
/*
#include <stdio.h>
#include <pthread.h>

void* DoWork(void* args) {
    printf("ID: %lu, CPU: %d\n", pthread_self(), sched_getcpu());
    return 0;
}

int main() {

    int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of processors: %d\n", numberOfProcessors);

    pthread_t threads[numberOfProcessors];

    pthread_attr_t attr;
    cpu_set_t cpus;
    pthread_attr_init(&attr);

    for (int i = 0; i < numberOfProcessors; i++) {
       CPU_ZERO(&cpus);
       CPU_SET(i, &cpus);
       pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
       pthread_create(&threads[i], &attr, DoWork, NULL);
    }

    for (int i = 0; i < numberOfProcessors; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
*/
