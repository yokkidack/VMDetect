//gcc -o test -std=c99 -lpthread test_pthread.c

/*
pthread test
*/
#include <pthread.h>
#include <x86intrin.h>
#ifdef __APPLE__

// https://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html
#include <stdint.h>
#include <unistd.h>
// #include <thread_policy.h>
// #include <Kernel/thread_policy.h>
#include <mach/thread_policy.h>
#define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"

typedef struct cpu_set {
  uint32_t    count;
} cpu_set_t;

static inline void
CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

static inline void
CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

static inline int
CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }

int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set)
{
  int32_t core_count = 0;
  size_t  len = sizeof(core_count);
  int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
  if (ret) {
    printf("error while get core count %d\n", ret);
    return -1;
  }
  cpu_set->count = 0;
  for (int i = 0; i < core_count; i++) {
    cpu_set->count |= (1 << i);
  }

  return 0;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                           cpu_set_t *cpu_set)
{
  thread_port_t mach_thread;
  int core = 0;

  for (core = 0; core < 8 * cpu_size; core++) {
    if (CPU_ISSET(core, cpu_set)) break;
  }
  //printf("binding to core %d\n", core);
  thread_affinity_policy_data_t policy = { core };
  mach_thread = pthread_mach_thread_np(thread);
  thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                    (thread_policy_t)&policy, 1);
  return 0;
}

#endif

#include <stdio.h>
int count;

void *loopy(){
    for (;;count++){
        __asm__("nop");
    }
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
    CPU_ZERO(&cpus);
    CPU_SET(1, &cpus);
    #ifdef __APPLE__
    pthread_setaffinity_np(&tid, sizeof(cpu_set_t), &cpus);
    pthread_setaffinity_np(&ltid, sizeof(cpu_set_t), &cpus);
    #elif defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
    pthread_attr_setaffinity_np(&lattr, sizeof(cpu_set_t), &cpus);
    #endif

    //pthread_attr_setaffinity_np(&lattr, sizeof(cpu_set_t), &cpus);

    /* создаем новый поток */
    pthread_create(&tid,&attr,cpuidy,NULL);
    pthread_create(&ltid,&lattr,loopy,NULL);


    /* ждем завершения исполнения потока */
    pthread_join(tid,NULL);
    pthread_cancel(ltid);
    printf("%d", count);
    return 0;
}
