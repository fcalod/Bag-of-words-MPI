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

// Ejecutar con ./bag_words_par 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv


int main (int argc, char *argv[]) {

	 // MPI Init
	const int MASTER = 0;
	int num_processes = 0;
	int process_id = 0; 
	int name_length = 0;
	char host_name[MPI_MAX_PROCESSOR_NAME];
	double wall_time; // MPI time
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Get_processor_name(host_name, &name_length);


	//Program Variables Init
	map <string, int> vocab; // Word counts for current book 
	int tot_word_count = 0; // Total number of words
	int vocab_size_per_book[num_processes]; // Unique words per book
	vector<string> const file_names{ argv + 1, argv + argc - 1 }; // Stores cmd line input in a vector
	int book_indx = 0;
	double total_time = 0;
	double start, end;
	
	// Root Tasks
	if (process_id == 0){
		// Load Vocabulary
		string vocab_file = argv[argc-1];
		load_vocab("vocab_file", vocab);
		////load_vocab("vocab.csv", vocab);
		
		// Start Timer
		 wall_time = MPI_Wtime();

	}
	
	string in_file_name = "sample_data/" + file_names[process_id]  + ".txt";
    string out_file_name = "results/" + file_names[process_id] + "_resultsPar.csv";
		
	// Counts words from the current book
	process_book(in_file_name, out_file_name, vocab, tot_word_count);
	save_results(out_file_name, vocab, vocab_size_per_book[process_id], tot_word_count);

	cout << "File: " << file_names[process_id] << "  Vocab size: " << vocab_size_per_book[process_id] 
		 << "  Word count: " << tot_word_count << endl;
		 //"  Time: " << (float)duration.count()/1000000 << "s" << endl;
	

 if (process_id == 0) {
      wall_time = MPI_Wtime() - wall_time;
      cout <<  wall_time << endl;
    }

	MPI_Finalize();

	return 0;
}

