#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#define random(x) (rand()%x)

using namespace std;
const int n =600; //size of array
const int P =16; //processor
const int p = (int) (sqrt( (double) (P) ) );
const int m = n/p;
int a[n][n];
bool flag[P];
timeval tvpre, tvafter;

struct processNode{
    int lx,ly,rx,ry;//upper left and lower right
}PN[P];

void init_array(){
    ifstream fin("matrix.txt");
    for(int i  = 0; i < n; i++)
    {
        for(int j = 0 ; j < n; j++)
        {
            fin>>a[i][j];
        }
    }
    fin.close();
}

void init_processNode(){
    int px,py;
    for(int i = 0 ; i < P;i++){
        px = i/p;
        py = i%p;
        PN[i].lx =px *m;
        PN[i].ly =py *m;
        PN[i].rx =PN[i].lx +m-1;
        PN[i].ry =PN[i].ly +m-1;
    }
}
void *thread1(void *ID){
    int id = (long int)ID;
    for(int i = PN[id].lx+1 ,k=0; i <=PN[id].rx; i++,k++){
        for(int j = PN[id].ly; j<=PN[id].ly+k;j++){
            swap(a[i][j],a[PN[id].lx+j-PN[id].ly][PN[id].ly+i-PN[id].lx]);
       }
    }
}
void *thread2(void *ID)
{
    int id = (long int)ID;
    int px,py;
    px = id/p;
    py = id%p;
    if(px != py && !flag[id])
    {
        int opid = px + py*p;
        for(int i1 =PN[id].lx,i2 =PN[opid].lx;i1<=PN[id].rx;i1++,i2++){
            for(int j1 =PN[id].ly,j2 =PN[opid].ly;j1<=PN[id].ry;j1++,j2++){
                swap(a[i1][j1],a[i2][j2]); //every process can be detected
            }
        }
        flag[id] =1;
        flag[opid] =1;
    }
}
void display(){
    for(int i  = 0; i < 15; i++)
    {
        for(int j = 0 ; j < 15; j++)
        {
            if(a[i][j]/10 ==0) printf("%d  ",a[i][j]);
            else printf("%d ",a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
int main(int arg, char *argv[])
{
    gettimeofday(&tvpre, NULL); //
    init_array();
    init_processNode();
    printf("Original matrix\n");
    display();
    pthread_t *threads;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*P);
    for (int i=0;i<P;i++){
        pthread_create(&threads[i],NULL,thread1,(void*)(long int)i);
    }
    for (int i=0;i<P;i++){
        pthread_join(threads[i],NULL);
    }
    for (int i=0;i<P;i++){
        pthread_create(&threads[i],NULL,thread2,(void*)(long int)i);
    }
    for (int i=0;i<P;i++){
        pthread_join(threads[i],NULL);
    }
    printf("After transposed:\n");
    display();
    gettimeofday(&tvafter, NULL);//
    printf("cost time: %ld msecond\n", (tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000);
    printf("\n");
    return 0;
}
