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

// Maps each word to its index
void load_vocab_indx(map<string, int> &vocab, map<string, int> &vocab_indx) {
	int word_index = 0;
	
	for(auto const& [word, count] : vocab) {
		vocab_indx[word] = word_index;
		word_index++;
	}
}

// Counts words in the current book
void process_book(string in_file_name, map<string, int>& vocab_indx, int* word_counts, int &tot_words_per_book) {
	ifstream in(in_file_name);
    
    if (!in)
        cerr << "Couldn't read file: " << in_file_name << "\n";
	
	string line, val; // stores lines from the file and words within each line
	
	// Reads file line by line
	while(getline(in, line)) {
		stringstream ss(line);
		
		// Splits each line by spaces
		while(!ss.eof()) {
			getline(ss, val, ',');
			string word = val;
			word_counts[vocab_indx[word]]++; // vocab_indx maps words to their index in word_counts
			tot_words_per_book++; // Counts words, with repetition
		}		
	}
	
	in.close();
}

// Writes output to file
void save_results(string out_file_name, int* word_counts, int& vocab_size) {
	ofstream out;
	out.open(out_file_name, ios_base::app); // Append mode
	
	// Saves word counts
	for(int i = 0; i < vocab_size; i++)
		out << word_counts[i]  << ",";
	
	out << "\n";
	out.close();
}

// LINUX //
// Ejecutar con mpiexec -n 6 --use-hwthread-cpus bag_words_par 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv 15164

//Windows //
// Ejecutar con mpiexec -n 6 bag_words_par 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv 15164

int main (int argc, char *argv[]) {
	// MPI Init
	const int MASTER = 0;
	int num_processes = 0;
	int process_id = 0; 
	int name_length = 0;
	char host_name[MPI_MAX_PROCESSOR_NAME];
	double wall_time; // MPI time
	MPI_Request request[2];
   //MPI_Status status[4];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Get_processor_name(host_name, &name_length);
	
	//Program Variables Init
	map<string, int> vocab; // Word counts for current book
	map<string, int> vocab_indx; // Word indices for current book
	int vocab_size = stoi(argv[argc-1]); // Total unique words across all books
	int word_counts[vocab_size]; // Word counts for current book
	int vocab_size_per_book = 0; // Unique words per book
	int tot_words_per_book = 0; // Total number of words per book
	vector<string> const file_names{ argv + 1, argv + argc - 2 }; // Stores cmd line input in a vector
	double total_time = 0;
	double start, end;
	
	string in_file_name = "sample_data/" + file_names[process_id]  + ".txt";
    string out_file_name = "results/results_par.csv";
	
	// Master thread Tasks
	if (process_id == 0){
		// Load Vocabulary
		string vocab_file = argv[argc - 2];
		cout << vocab_file;
		load_vocab(vocab_file, vocab);
		load_vocab_indx(vocab, vocab_indx);
		
		// Start Timer
		wall_time = MPI_Wtime();
		
		// Write file headers for later 
		write_headers(out_file_name, vocab);
	}
	
	// Counts words from the current book
	//cout << "Soy proc " << process_id << " size_per_book " << vocab_size_per_book << endl;
	process_book(in_file_name, vocab_indx, word_counts, tot_words_per_book);
	save_results(out_file_name, &word_counts[process_id], vocab_size);
	cout << "File: " << file_names[process_id] << "  Vocab size: " << vocab_size_per_book
		 << "  Word count: " << tot_words_per_book << endl;
		 //"  Time: " << (float)duration.count()/1000000 << "s" << endl;
	
	// Sends the results to master thread
	if(process_id != 0) {
		//for(int id = 1; id < num_processes; id++) {
			MPI_Isend(&word_counts[process_id], 1, MPI_INT, 0, 101, MPI_COMM_WORLD, &request[0]); 
		//}
	}
	
	// Writes results to file
	if (process_id == 0) {
		wall_time = MPI_Wtime() - wall_time;
		cout <<  wall_time << endl;
		
		for(int id = 1; id < num_processes; id++) {
			MPI_Irecv(&word_counts[id], 1, MPI_INT, id, 101, MPI_COMM_WORLD, &request[1]);
			save_results(out_file_name, word_counts, vocab_size);
		}
	}
	
	MPI_Finalize();
	return 0;
}

