#include <mpi.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

using namespace std;

// ==================================
// ======= PARALLEL VERSION =========
// ==================================



int main (int argc, char *argv[]) 
{
	int id;
	int num_processes;
	double wall_time; // MPI time
	map<string, long long int> word_dict;
	long long int tot_word_count = 0; // Total number of words
	long long int vocab_size = 0; // Number of unique words
	vector<string> const file_names{ argv + 1, argv + argc }; // Stores cmd line input in a vector
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	
	// Loops over the list of books
	for(string file: file_names) 
	{
		string input_file_name = file + ".txt";
    	string output_file_name = file + "_results.csv";
		
		if (id == 0) 
			wall_time = MPI_Wtime();
		
		// Only master thread prints results
		if (id == 0) {
			wall_time = MPI_Wtime() - wall_time;
			cout << "File: " << file << "  Vocab size: " << vocab_size << "  Word count: " << tot_word_count 
			     << "  Time: " << wall_time << endl;
		}
	}
	
	MPI_Finalize();
	return 0;
}

