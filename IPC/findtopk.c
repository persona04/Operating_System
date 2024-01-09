#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int counter_func(char filename[]){

    FILE * ptr;
    ptr = fopen(filename,"r");
    int counter = 0;
    int val;
    
    while(fscanf(ptr,"%d",&val) == 1) counter++;
    
    fclose(ptr);

    return counter;
}


void create_family(int n,int k,char fileArr[][20],int cn, char outfile[10],clock_t start_time){
    if(n == 0){ // stops creating child. final touch from parent process
        wait(NULL);
        wait(NULL);
        wait(NULL);
        int lastArr[cn];
        int lastcounter;
        int lastsecondcounter;
        FILE * lptr;
        FILE * iptr;
        iptr = fopen("inter","w");
        char lastfilename[10];
        int tempvar;
        for(lastcounter=1;lastcounter<cn+1;lastcounter++){
            sprintf(lastfilename,"%d",lastcounter);
            lptr = fopen(lastfilename,"r");
            fscanf(lptr,"%d",&tempvar);
            fprintf(iptr,"%d\n",tempvar);                
            fclose(lptr);
        }
        fclose(iptr);

        // last step. taking vals from inter file. sorting them and print k. value

        iptr = fopen("inter","r");
        int itemp;
        int f;
        for(f=0;f<cn;f++){
            fscanf(iptr,"%d",&itemp);
            lastArr[f] = itemp;
        } 
        fclose(iptr);
        
        int si,sj;
        for(si=0;si<cn-1;si++){
            for(sj=0;sj<cn-1-si;sj++){
                if(lastArr[sj]<lastArr[sj+1]){
                    itemp = lastArr[sj];
                    lastArr[sj] = lastArr[sj+1];
                    lastArr[sj+1] = itemp;
                }
            }
        }
        
        FILE * optr;
        optr = fopen(outfile,"w");
        for(f=0;f<cn;f++){
            fprintf(optr,"%d\n",lastArr[f]);
        }
        fclose(optr);

        // deleting intermediate files 
        
        remove("inter");
        char delfile[5];
        for(f=1;f<cn+1;f++){
            sprintf(delfile,"%d",f);
            remove(delfile);
        }

        clock_t finish_time = clock();
        double elapsed_time = ((double)finish_time-start_time)/CLOCKS_PER_SEC ;
        printf("Elapsed time %lf",elapsed_time);
        fflush(stdout);
        exit(0);
    }
    int pid;
    int file_index = n-1; 
    pid = fork();
    if(pid > 0){ // parent process
        
        create_family(--n,k,fileArr,cn,outfile,start_time);
    }
    else if (pid == 0){ // child process
        fflush(stdout);
        
        // taking file name by setted index from array
        char myFile[20];
        strcpy(myFile,fileArr[file_index]);
        
        // taking line numbers of file for declaring int array
        int lines = counter_func(myFile);
        
        // creating array for values
        int vals[lines];
        FILE * ptr;
        ptr = fopen(myFile,"r");
        int i = 0;
        while(fscanf(ptr,"%d",&vals[i]) == 1) ++i;
        fclose(ptr);
        //sorting with bubble sort
        int r;
        int l;
        int temp;
        for(r=0;r<lines-1;r++){
            for(l=0;l<lines-1-r;l++){
                if(vals[l+1]>vals[l]){
                    temp = vals[l];
                    vals[l] = vals[l+1];
                    vals[l+1] = temp;
                }
            }
        }

        // taking firs k numbers and writing intermediate file as n index
        
        char intermediateFile[10];
        sprintf(intermediateFile,"%d",n);
        ptr = fopen(intermediateFile,"w");
        fprintf(ptr,"%d\n",vals[k-1]);
        fclose(ptr);

        exit(0);
    }
}



int main (int argc, char * argv[]){

    clock_t start_time = clock();
    // for information
    if(argc == 1) {printf("findtopk <k> <N> <infile1> ...<infileN> <outfile>"),exit(1);}
    
    //// input validation
    
    // validates k value
    int k;
    k = atoi(argv[1]);
    if(k == 0){ printf("k param should be number! %s value is not number!",argv[1]);exit(1);}
    else if( k>1000 || k<0){printf("k param should be in 0<k<1000 range!"); exit(1);}
    
    // validates N value and file names
    int n;
    n = atoi(argv[2]);
    if(n==0){printf("N param should be number which one bigger than 0."); exit(1);}
    else if(argc != n + 4){printf("You must enter 3 file names. No much or no less.",n);exit(0);}

    //taking outfile
    char outfile[10];
    strcpy(outfile,argv[n+3]);
    

    
    // taking filenames to array
    char fileArr[n][20];
    int i;
    for(i=0;i<n;i++){
        strcpy(fileArr[i],argv[i+3]);
        }
    
    create_family(n,k,fileArr,n,outfile,start_time);

    return 0;
}