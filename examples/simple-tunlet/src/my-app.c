#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int my_val = 1;

void hello_world(){
    printf("The replacing function has worked !!\n");
    // If we put these two lines, the Analyzer
    // will enter the function just once
    //MPI_Finalize();
    //exit(0);
}

void instr_function(int value_to_change){

    if (value_to_change == 1)
        printf("[INFO]: Value not changed: %d\n", value_to_change);
    else {
        printf("[INFO]: Changed value to %d!!\n", value_to_change);
    }
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);
    
    while(true){    
        instr_function(my_val);
    }   
    // Finalize the MPI environment.
    MPI_Finalize();
}

