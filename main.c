#include <stdio.h>

#if defined(__i386__)
    #include <x86intrin.h>
    static __inline__ unsigned long long rdtsc(void){
        unsigned long long int x;
        __asm__ volatile (".byte 0x0f, 0x31"
                                :   "=A" (x));
        return x;
    }
#elif defined(__x86_64__)
    #include <x86intrin.h>
    static __inline__ unsigned long long rdtsc(void){
        unsigned hi, lo;
        __asm__ __volatile__ ("rdtsc"
                                :   "=a"(lo),
                                    "=d"(hi));
        return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
    }
#elif defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    #include <intrin.h>
    #pragma intrinsic(__rdtsc)
    #define rdtsc() __rdtsc()
#endif

int main(int argc, char **argv)
{
    unsigned eax, ebx, ecx, edx;
    eax = 1;
    unsigned i, j;
    i = rdtsc();
    asm volatile("cpuid"
            :   "=a" (eax),
                "=b" (ebx),
                "=c" (ecx),
                "=d" (edx)
            :   "0"  (eax),
                "2"  (ecx));
    j = rdtsc();
    printf("%d", j-i);

}
