#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstring>
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
void process_book(string in_file_name, map<string, int>& vocab_indx, int* word_counts,
	int &tot_words_per_book, int &vocab_size_per_book) 
	{
	ifstream in(in_file_name);
    
    if (!in)
        cerr << "Couldn't read file: " << in_file_name << "\n";
	
	string line, val; // stores lines from the file and words within each line
	
	// Reads file line by line
	while(getline(in, line)) {
		stringstream ss(line);
		
		// Splits each line by spaces
		while(ss.good()) {
			getline(ss, val, ',');
			string word = val;
			word_counts[vocab_indx[word]]++; // vocab_indx maps words to their index in word_counts
			if (word_counts[vocab_indx[word]] == 1 ) vocab_size_per_book++; // if new word, increase vocabulary of the book
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
	int bag_of_words[num_processes][vocab_size];
	vector<string> const file_names{ argv + 1, argv + argc - 2 }; // Stores cmd line input in a vector
	double total_time = 0;
	double start, end;


	int data_to_be_collected[vocab_size*6];
	
	string in_file_name = "sample_data/" + file_names[process_id]  + ".txt";
    string out_file_name = "results/results_par.csv";
	
	// Master thread Tasks
	if (process_id == 0){
		// Start Timer
		wall_time = MPI_Wtime();
		
		// Write file headers for later 
		write_headers(out_file_name, vocab);
	}
	
	// Clear word counter
	memset(word_counts, 0, sizeof(word_counts));

	// Load Vocabulary
	string vocab_file = argv[argc - 2];
	load_vocab(vocab_file, vocab);
	load_vocab_indx(vocab, vocab_indx);
		
	// Counts words from the current book
	process_book(in_file_name, vocab_indx, word_counts, tot_words_per_book,vocab_size_per_book );
	
	cout << "File: " << file_names[process_id] << "  Vocab size: " << vocab_size_per_book
		 << "  Word count: " << tot_words_per_book << endl;

	// Writes results to file
	MPI_Gather(&word_counts, vocab_size, MPI_INT, data_to_be_collected,vocab_size , MPI_INT, 0, MPI_COMM_WORLD);
	if (process_id == 0) {
		int k = 0;
		for(size_t i = 0; i < num_processes; ++i)
		{
			for(size_t j = 0; j < vocab_size; ++j)
			{
				bag_of_words[i][j] = data_to_be_collected[k++];
			}
		}
		std::ofstream out("results/BagOfWords_Parallel.csv");
		for (auto& row : bag_of_words) {
			for (auto col : row)
				out << col <<',';
			out << '\n';
		}
		wall_time = MPI_Wtime() - wall_time;
		cout <<  wall_time << endl;		
	}
	MPI_Finalize();
	return 0;
}

