#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

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

struct message_buffer {
    long msg_type;
    int msg_val;
};

void create_family(int k, int n, char fileArr[][20], char outfile[10], int msgid, int cn,clock_t start_time) {
    if (n == 0) { // parent process

        wait(NULL);
        wait(NULL);

        int lastArr[cn];

        struct message_buffer pmessage;
        int li;
        for (li = 0; li < cn; li++) {
            msgrcv(msgid, &pmessage, sizeof(pmessage), 1, 0);
            lastArr[li] = pmessage.msg_val;
        }

        // sorting
        int lj, ltemp;
        for(li=0;li<cn-1;li++){
            for(lj=0;lj<cn-1-li;lj++){
                if(lastArr[lj]<lastArr[lj+1]){
                    ltemp = lastArr[lj];
                    lastArr[lj] = lastArr[lj+1];
                    lastArr[lj+1] = ltemp;
                }
            }
        }

        FILE * lfile;
        lfile = fopen(outfile,"w");
        for(li=0;li<cn;li++){
            fprintf(lfile,"%d\n",lastArr[li]);
        }  
        fclose(lfile);

        clock_t finish_time = clock();
        double elapsed_time = ((double)finish_time-start_time)/CLOCKS_PER_SEC;
        printf("%lf",elapsed_time);
        fflush(stdout); 

        exit(0);
    } else {
        int pid = fork();
        if (pid > 0) { // parent process
            create_family(k, --n, fileArr, outfile, msgid, cn, start_time);
        } else if (pid == 0) { // child process
            int lines = counter_func(fileArr[n - 1]);
            int vals[lines];
            char cfile[2];
            FILE *cptr;

            sprintf(cfile, "%d", n);
            cptr = fopen(fileArr[n - 1], "r");
            int cf = 0;
            while (fscanf(cptr, "%d", &vals[cf]) == 1)
                ++cf;
            fclose(cptr);

            //sorting 
            int ki,kj,ktemp;
            for(ki=0;ki<lines-1;ki++){
                for(kj=0;kj<lines-1-ki;kj++){
                    if(vals[kj]<vals[kj+1]){
                        ktemp=vals[kj];
                        vals[kj]=vals[kj+1];
                        vals[kj+1]=ktemp;
                    }

                }
            }

            struct message_buffer message;
            message.msg_type = 1;  // message type
            message.msg_val = vals[k - 1];
            
            // sending message
            msgsnd(msgid, &message, sizeof(message), 0);
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    
    clock_t start_time = clock();
    
    // for information
    if (argc == 1) {
        printf("findtopk_mqueue <k> <N> <infile1> ...<infileN> <outfile>");
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
        printf("You must enter 3 file names. No much or no less.", n);
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
    int cn = n;
    key_t key = ftok("/myKey", 64);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    create_family(k, n, fileArr, outfile, msgid, cn, start_time);

    return 0;
}