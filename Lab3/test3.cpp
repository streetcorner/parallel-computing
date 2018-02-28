#include <iostream>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define N 600 //the size of the matrix
//#define M 15 //the size of the matrix displayed
using namespace std;

int a[N][N];
timeval tvpre, tvafter;


void read_array(){
    ifstream fin("matrix.txt");
    for(int i  = 0; i < N; i++){
        for(int j = 0 ; j < N; j++){
            fin>>a[i][j];
        }
    }
    fin.close();
}

int main(int argc, char *argv[])
{
    gettimeofday(&tvpre, NULL);

    int l,r,m;//left right limitation and length of the partition

    int myid;//cur id
    int p;// number of core
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);/*得到当前进程号*/
    MPI_Comm_size(MPI_COMM_WORLD,&p);/*得到总的进程数*/
    MPI_Status *status = (MPI_Status *)malloc(sizeof(MPI_Status));//apply for allocation

    m = N/p;
    l = myid *m;
    r = l+m;
    if(myid == p-1){
            r= N;
    }

    read_array();
    //transmission of the partition
    for(int i = l;i < r;i++)
    {
        for(int j = 0;j < i;j++)
        {
            int tmp = a[i][j];
            a[i][j] = a[j][i];
            a[j][i] = tmp;
        }
    }
    //convey other partition to p0 and file out
    if(myid == 0){
        //convey partition i to p0
        for(int i = 1; i < p;i++){
            int pl = i*m;
            int pr = pl+m;
            //convey by row,j is row number
            //address start from column
            for(int j = 0;j<pl;j++){
                MPI_Recv(a[j]+pl,pr-pl,MPI_INT,i,j,MPI_COMM_WORLD,status);
            }
            for(int j = pl;j<pr;j++){
                MPI_Recv(a[j],pr,MPI_INT,i,j,MPI_COMM_WORLD,status);
            }
        }
        //fout
        ofstream fout("transmatrix.txt");
        fout<<("After transposed:")<<endl;
        for(int i  = 0; i < N; i++){
            for(int j = 0 ; j < N; j++)
            {
                if(a[i][j]/10 ==0) fout<<a[i][j]<<"  ";
                else fout<<a[i][j]<<" ";
            }
            fout<<endl;
        }
        fout.close();
    }
    //p1 to p(p-1)
    else{
        //i is the row of cur id
        for(int i = 0;i < l;i++){
             MPI_Send(a[i]+l,r-l,MPI_INT,0,i,MPI_COMM_WORLD);
        }
        for(int i = l; i<r;i++){
            MPI_Send(a[i],r,MPI_INT,0,i,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();

    printf("The core number is %d",p);
    gettimeofday(&tvafter, NULL);
    printf("cost time: %ld msecond\n", (tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000);

    return 0;
}
