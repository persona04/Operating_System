#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex;

// for pass vals to thread
struct ThreadParams{
    int* resArr;
    char filepass[20];
    int index;
    int kval;
};

int counter_func(char filename[]) {
    FILE *ptr;
    ptr = fopen(filename, "r");
    int counter = 0;
    int val;

    while (fscanf(ptr, "%d", &val) == 1)
        counter++;

    fclose(ptr);

    return counter;
}

// thread function
void * fileThread(void *ptr){
    struct ThreadParams * params = (struct ThreadParams *)ptr; 
    FILE * fptr = fopen(params->filepass,"r");
    int lines = counter_func(params->filepass);
    int vals[lines];
    int o;
    int u;
    for(o=0;o<lines;o++){
        fscanf(fptr,"%d",&vals[o]);
    }
    fclose(fptr);
    int temp;
    
    // sorting
    for(o=0;o<lines-1;o++){
        for(u=0;u<lines-1-o;u++){
            if(vals[u]<vals[u+1]){
                temp = vals[u];
                vals[u] = vals[u+1];
                vals[u+1] = temp;
            }
        }
    }
    pthread_mutex_lock(&mutex);
    params->resArr[params->index] = vals[params->kval-1];
    pthread_mutex_unlock(&mutex);
}
 
int main(int argc, char *argv[]){
    
    clock_t start_time = clock();
    // for information
    if (argc == 1) {
        printf("findtopk_pthread <k> <N> <infile1> ...<infileN> <outfile>");
        exit(1);
    }

    //// input validation

    // validates k value
    int k;
    k = atoi(argv[1]);
    if (k == 0) {
        printf("k param should be number! %s value is not number!", argv[1]);
        exit(1);
    } else if (k > 1000 || k < 0) {
        printf("k param should be in 0 < k < 1000 range!");
        exit(1);
    }

    // validates N value and file names
    int n;
    n = atoi(argv[2]);
    if (n == 0) {
        printf("N param should be number which one bigger than 0.");
        exit(1);
    } else if (argc != n + 4) {
        printf("You must enter %d file names. No much or no less.", n);
        exit(0);
    }

    // taking outfile
    char outfile[10];
    strcpy(outfile, argv[n + 3]);

    // taking filenames to array
    char fileArr[n][20];
    int i;
    for (i = 0; i < n; i++) {
        strcpy(fileArr[i], argv[i + 3]);
    }
    
    // creating threads
    
    pthread_t threads[n];
    
    int results[n];
    struct ThreadParams params[n];
    
    
    int t;
    for(t=0;t<n;t++){
        
    }

    // creating threads and their params
    for(t=0;t<n;t++){
        params[t].index = t;
        params[t].kval = k;
        params[t].resArr = results;
        strcpy(params[t].filepass,fileArr[t]);
        pthread_create(&threads[t],NULL,fileThread,(void *)&params[t]);
    }

    // waiting for threads to finish
    for(t=0;t<n;t++){
        pthread_join(threads[t],NULL);
    }

    // sorting
    int y;
    int ctemp;
    for(t=0;t<n-1;t++){
        for(y=0;y<n-1-t;y++){
            if(results[y]<results[y+1]){
                ctemp = results[y];
                results[y] = results[y+1];
                results[y+1] = ctemp;
            }
        }
    }

    // writing to outfile
    FILE * resPtr = fopen(outfile,"w");
    
    for(t=0;t<n;t++){
        fprintf(resPtr,"%d\n",results[t]);
    }
    
    fclose(resPtr);
    
    clock_t finish_time = clock();

    double elapsed_time =  ((double)finish_time-start_time)/CLOCKS_PER_SEC;
    printf("%lf",elapsed_time);
    fflush(stdout);

    return 0;
}