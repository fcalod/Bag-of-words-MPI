#include <mpi.h>
#include <iostream>
#include <cmath>
#include <chrono>

using namespace std;

// ==================================
// ======= PARALLEL VERSION =========
// ==================================



int main (int argc, char *argv[]) {
	int id;
	int num_processes;
	double wall_time; // MPI time
	long long int word_count;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	
	
	MPI_Finalize();
	return 0;
}

