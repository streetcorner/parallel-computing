#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
using namespace std;

static long N=10000000; //
static int T=8;  //
static long length=N/T;
static double PI=0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

timeval tvpre, tvafter;



void *thread(void *ID){
    int id=(unsigned long int)ID;
    double sum=0;

    if (id!=T){
        for (int i=id*length;i<(id+1)*length;i++){
            double tmp=(i+0.5)/N;
            sum+=4/(1+tmp*tmp);
        }
    }
    else{
        for (int i=id*length;i<=N;i++){ //
            double tmp=(i+0.5)/N;
            sum+=4/(1+tmp*tmp);
        }
    }

    pthread_mutex_lock(&mutex);
    PI+=sum;
    pthread_mutex_unlock(&mutex);

}


int main(int argc, char const *argv[])
{
    gettimeofday(&tvpre, NULL); //
    if (argc==3){
        N=atoi(argv[1]);
        T=atoi(argv[2]);
        length=N/T;
    }
    cout<<N<<"   "<<T<<endl;
    pthread_t *threads;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*T);
    for (int i=0;i<T;i++){
        if (pthread_create(&threads[i],NULL,thread,(void*)i)){
            printf("%d failed\n",i);
        }
    }
    for (int i=0;i<T;i++){
        pthread_join(threads[i],NULL);
    }
    printf("%0.40f\n", PI/N);
    gettimeofday(&tvafter, NULL);//
    printf("cost time: %ld msecond\n", (tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000);
    return 0;
}
