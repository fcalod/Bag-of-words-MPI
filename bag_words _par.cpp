#include <mpi.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include "utils.cpp"

using namespace std;

// ==================================
// ======= PARALLEL VERSION =========
// ==================================

// Counts words in the current book
void process_book(string in_file_name, string out_file_name, map<string, int>& vocab, int &tot_word_count) {
	ifstream in(in_file_name);
    
    if (!in)
        cerr << "Couldn't read file: " << in_file_name << "\n";
	
	string line, val; // store lines from the file and words within each line
	
	// Reads file line by line
	while(getline(in, line)) {
		stringstream ss(line);
		
		// Splits each line by spaces
		while(!ss.eof()) {
			getline(ss, val, ',');
			string word = val;
			vocab[word] += 1;
		}		
	}
}

// Ejecutar con ./bag_words_ser 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv num_proc


int main (int argc, char *argv[]) {
	int id;
	int num_proc = argv[argc-1]; // Last input
	double wall_time; // MPI time
	
	map <string, int> vocab; // Word counts for current book 
	int tot_word_count = 0; // Total number of words
	int vocab_size = 0; // Number of unique words
	vector<string> const file_names{ argv + 1, argv + argc - 2 }; // Stores cmd line input in a vector
	int book_indx = 0;
	double total_time = 0;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	
	load_vocab("vocab.csv", vocab);
	
	// Loops over the list of books
	for(string file: file_names) {
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
	cout << "\nTotal time: " << total_time << "s" << endl;
	
	return 0;
}

