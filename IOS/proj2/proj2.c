/*
 * Author: Tomáš Smädo
 * login: xsmado00
 * Created on April 29, 2018, 4:21 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>


/**
 * function generates random number in range from 0 to top
 * @param top - defines the maximal number that could be generated
 * @return random number in range from 0 to top
 */
int random_gen (int top) {
    if (top == 0) {
        return top;
    }
    long x = rand() % (top * 1000);
    return x;
}

void clean_shared_memory();

/**
 * system error call function
 */
void error_call() {
    fprintf(stderr, "# ERROR: System call failed! #\n");
}

int *action_ID,     //number of current action
        *rid_ID,        //ID of rider
        *bus_ID,        //ID of bus
        *CR,            //number of riders waiting on the bus top
        *rid_done;      //number of riders left to transport

//pointers for shared memory creation
int action_ID_addr, rid_ID_addr, bus_ID_addr, CR_addr, rid_done_addr;

//semaphores
sem_t   *entry_denial,   //enables entry to bus stop for riders
        *bus,            //signals riders to board
        *boarded,        //signals bus that rider has boarded
        *master,         //master semaphore for shared mem
        *mem_prot,       //protects write into file
        *finish;         //allow riders to finish

/**
 * creates shared memory and semaphores
 */
void create_shared_memory() {
    if((action_ID_addr = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        error_call();
        clean_shared_memory();
    }
    if((rid_ID_addr = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        error_call();
        clean_shared_memory();
    }
    if((bus_ID_addr = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        error_call();
        clean_shared_memory();
    }
    if((CR_addr = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        error_call();
        clean_shared_memory();
    }
    if((rid_done_addr = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1) {
        error_call();
        clean_shared_memory();
    }

    if((action_ID = shmat(action_ID_addr, NULL, 0)) == NULL){
        error_call();
        clean_shared_memory();
    }
    if((rid_ID = shmat(rid_ID_addr, NULL, 0)) == NULL){
        error_call();
        clean_shared_memory();
    }
    if((bus_ID = shmat(bus_ID_addr, NULL, 0)) == NULL){
        error_call();
        clean_shared_memory();
    }
    if((CR = shmat(CR_addr, NULL, 0)) == NULL){
        error_call();
        clean_shared_memory();
    }
    if((rid_done = shmat(rid_done_addr, NULL, 0)) == NULL){
        error_call();
        clean_shared_memory();
    }

    *action_ID = 1;
    *rid_ID = 1;
    *bus_ID = 1;
    *CR = 0;
    *rid_done = 0;

    if((entry_denial = sem_open("/xsmado00_entry_denial", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }
    if((bus = sem_open("/xsmado00_bus", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }
    if((boarded = sem_open("/xsmado00_boarded", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }
    if((master = sem_open("/xsmado00_master", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }
    if((finish = sem_open("/xsmado00_finish", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }
    if((mem_prot = sem_open("/xsmado00_mem_prot", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) {
        error_call();
        clean_shared_memory();
    }


    sem_init(entry_denial, 1, 0);
    sem_init(bus, 1, 0);
    sem_init(boarded, 1, 0);
    sem_init(master, 1, 0);
    sem_init(finish, 1, 0);
    sem_init(mem_prot, 1, 0);
}

/**
 * cleans shared memory, unlinks semaphores, closes file
 */
void clean_shared_memory() {
    shmctl(action_ID_addr, IPC_RMID, NULL);
    shmctl(rid_ID_addr, IPC_RMID, NULL);
    shmctl(bus_ID_addr, IPC_RMID, NULL);
    shmctl(CR_addr, IPC_RMID, NULL);
    shmctl(rid_done_addr, IPC_RMID, NULL);
    sem_close(master);
    sem_unlink("/xsmado00_master");
    sem_close(bus);
    sem_unlink("/xsmado00_bus");
    sem_close(entry_denial);
    sem_unlink("/xsmado00_entry_denial");
    sem_close(boarded);
    sem_unlink("/xsmado00_boarded");
    sem_close(finish);
    sem_unlink("/xsmado00_finish");
    sem_close(mem_prot);
    sem_unlink("/xsmado00_mem_prot");
}

/**
 * @param a
 * @param b
 * @return minimum of two values
 */
int min_val(int a, int b) {
    return a <= b ? a : b;
}

/**
 * @param a
 * @param b
 * @return maximum of two values
 */
int max_val(int a, int b) {
    return a >= b ? a : b;
}

/**
 * function generates riders
 * @param total_riders - number of riders generated
 * @param ART - max time between new rider is generated
 * @param file - output file
 */
void generate_riders(int total_riders, int ART, FILE* file) {
    
    int i;
    for (i = 0; i < total_riders; i++) {
        if (ART != 0)
            usleep(random_gen(ART)); //delay of rider arrival

        pid_t rider = 0;
        rider = fork();
        if (rider < 0) {
            error_call();
            clean_shared_memory();
        } else if (rider == 0) {

            //sem_wait(master);
            sem_wait(mem_prot); file = fopen("proj2.out", "a");
            //rider starts
            fprintf(file, "%d: RID %d: start\n", (*action_ID)++, i+1);
            fflush(stdout);
            fclose(file); sem_post(mem_prot);

            //wait for bus departure
            sem_wait(entry_denial);
            //entering bus stop
            //increase the number of waiting riders
            (*CR)++;

            sem_wait(mem_prot); file = fopen("proj2.out", "a");
            fprintf(file, "%d: RID %d: enter: %d\n", (*action_ID)++, i+1, *CR);
            fflush(stdout);
            fclose(file); sem_post(mem_prot);

            sem_post(entry_denial);

            //wait for ack from bus
            sem_wait(bus);


            //wait for bus arrival

            //board
            sem_wait(mem_prot);
            file = fopen("proj2.out", "a");
            fprintf(file, "%d: RID %d: boarding\n", (*action_ID)++, i+1);
            fflush(stdout);
            fclose(file);
            sem_post(mem_prot);

            //ack bus about boarding
            sem_post(boarded);
            //increase counter of transported riders
            (*rid_done)++;

            //wait for bus to finish the ride
            sem_wait(finish);

            file = fopen("proj2.out", "a");
            fprintf(file, "%d: RID %d: finish\n", (*action_ID)++, i+1);
            fflush(stdout);
            fclose(file);

            exit(0);
        }
    }
}

/**
 * simulates bus
 * @param capacity - bus capacity
 * @param total_riders - total amount of riders to be transported
 * @param ABT - max time of bus travel
 * @param file - output file
 */
void bus_MHD(int capacity, int total_riders, int ABT, FILE* file) {

    file = fopen("proj2.out", "a");
    //bus starts
    fprintf(file, "%d: BUS: start\n", (*action_ID)++);
    fflush(stdout);
    fclose(file); sem_post(mem_prot);
    sem_post(entry_denial);

    //bus transports until there are no more riders
    while (((*CR) != 0) || ((*rid_done) != total_riders)) {

        //stop riders from entering bus stop
        //bus arrives to the stop
        sem_wait(entry_denial);
        int n = 0;

        sem_wait(mem_prot); file = fopen("proj2.out", "a+");
        fprintf(file, "%d: BUS: arrival\n", (*action_ID)++);
        fflush(stdout);
        fclose(file); sem_post(mem_prot);

        if ((*CR) > 0) { //riders waiting on the stop
            //boarding start

            sem_wait(mem_prot);
            file = fopen("proj2.out", "a");
            fprintf(file, "%d: BUS: start boarding:  %d\n", (*action_ID)++, *CR);
            fflush(stdout);
            fclose(file);
            sem_post(mem_prot);

            //signal boarding
            n = min_val((*CR), capacity);
            for (int i = 1; i <= n; i++) {
                sem_post(bus);
                sem_wait(boarded);
            }

            //update number of waiting riders
            (*CR) = max_val((*CR) - capacity, 0);

            sem_wait(mem_prot);
            file = fopen("proj2.out", "a");
            fprintf(file, "%d: BUS: end boarding: %d\n", (*action_ID)++, *CR);
            fflush(stdout);
            fclose(file);
            sem_post(mem_prot);

        }

        sem_wait(mem_prot); file = fopen("proj2.out", "a");
        fprintf(file, "%d: BUS: depart\n", (*action_ID)++);
        fflush(stdout);
        fclose(file); sem_post(mem_prot);

        //TODO signal departure
        sem_post(entry_denial);

        if (ABT != 0)
            usleep(random_gen(ABT)); //bus travels...

        //end of travel

        sem_wait(mem_prot);
        file = fopen("proj2.out", "a");
        fprintf(file, "%d: BUS: end\n", (*action_ID)++);
        fflush(stdout);
        fclose(file);
        sem_post(mem_prot);
        //allow riders to finish
        for (int j = 0; j < n; j++){
            sem_post(finish);
        }
    }

    //finish, all riders transported
    sem_wait(mem_prot); file = fopen("proj2.out", "a");
    fprintf(file, "%d: BUS: finish\n", (*action_ID)++);
    fflush(stdout);
    fclose(file); sem_post(mem_prot);
    sem_post(master);

    exit(0);

}


int main (int argc, char** argv){

    clean_shared_memory();

    int R, C, ART, ABT;
    FILE *file;
    if ((file = fopen("proj2.out", "w+")) == NULL){
        fprintf(stderr, "# ERROR: Couldnt open file for output! #\n");
        exit(1);
    }

    /** arguments control*/
    if (argc == 5) {
        R = atof(argv[1]);   //number of riders
        C = atof(argv[2]);	 //bus capacity
        ART = atof(argv[3]); //max time between new rider arrival
        ABT = atof(argv[4]); //max time for bus drive simulation
    } else {
        fprintf(stderr,"# ERROR: Bad number of arguments! #\n");
        exit(1);
    }
    if ((R <= 0) || (C <= 0) || (ART < 0) || (ART > 1000) || (ABT < 0) || (ABT > 1000)){
        fprintf(stderr,"# ERROR: Bad values of arguments! #\n");
        exit(1);
    }

    create_shared_memory();

    pid_t bus;
    bus = fork();
    if (bus < 0) {
        error_call();
        clean_shared_memory();
        exit (1);
    } else if (bus == 0)
    {
        bus_MHD(C, R, ABT, file);
    }
    else {
        generate_riders(R, ART, file);
    }

    sem_wait(master);
    clean_shared_memory();
    exit (0);
}