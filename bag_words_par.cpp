#include <mpi.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
//#include "utils.cpp"

using namespace std;

// ==================================
// ======= PARALLEL VERSION =========
// ==================================

// Loads the word map
void load_vocab(string file_name, map<string, int> &vocab) {
	ifstream in(file_name);
	cout << "Trying to read: " << file_name<< "\n";
    
    if (!in)
        cerr << "Couldn't read file: " << file_name << "\n";
	
	string line, val; // store lines from the file and words within each line
	
	// Only reads the header
	getline(in, line);
	stringstream ss(line);
	
	int word_index = 0;
	
	// Splits the line by commas and reads each value
	while(!ss.eof()) {
		getline(ss, val, ',');
		string word = val;
		vocab[word] = word_index;
		word_index++;
	}
}

// Counts words in the current book
void process_book(string in_file_name, map<string, int>& vocab, int* word_counts, int &word_count_per_book) {
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
			word_counts[vocab[word]]++;
			word_count_per_book++;
		}		
	}
}

// Writes output to file
void save_results(string out_file_name, map<string, int>& vocab, int* word_counts, int& vocab_size, int& tot_word_count) {
	ofstream out;
	out.open(out_file_name, ios_base::app); // Append mode
	out << ""; // 
	vocab_size = 0;
	
	// Saves headers (words found)
	for(auto const& [word, count] : vocab) {
		if(count > 0)
			out << word << ",";
		
		tot_word_count += count;
		vocab_size++;
	}
	
	out << "\n";
	out << "0,";
	
	// Saves word counts
	for(auto const& [word, count] : vocab)
		if(count > 0)
			out << count << "," ;
	
	out << "\n";
	out.close();
}

// Ejecutar con mpiexec -n 6 --use-hwthread-cpus bag_words_par 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv 15164

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
	map<string, int> vocab; // Word indices for current book
	int vocab_size = stoi(argv[argc-1]); // Total unique words across all books
	int word_counts[vocab_size]; // Word counts for current book
	int vocab_size_per_book[num_processes]; // Unique words per book
	int word_count_per_book = 0; // Total number of words per book
	vector<string> const file_names{ argv + 1, argv + argc - 2 }; // Stores cmd line input in a vector
	int book_indx = 0;
	double total_time = 0;
	double start, end;
	
	// Root Tasks
	if (process_id == 0){
		// Load Vocabulary
		string vocab_file = argv[argc - 2];
		load_vocab(vocab_file, vocab);
		//load_vocab("vocab.csv", vocab);
		
		// Start Timer
		 wall_time = MPI_Wtime();
	}
	
	string in_file_name = "sample_data/" + file_names[process_id]  + ".txt";
    string out_file_name = "results/" + file_names[process_id] + "_resultsPar.csv";
	
	// Counts words from the current book
	//cout << "Soy proc " << process_id << " size_per_book " << vocab_size_per_book[process_id] << endl;
	process_book(in_file_name, vocab, word_counts, word_count_per_book);
	cout << "File: " << file_names[process_id] << "  Vocab size: " << vocab_size_per_book[process_id] 
		 << "  Word count: " << word_count_per_book << endl;
		 //"  Time: " << (float)duration.count()/1000000 << "s" << endl;
	
	// Sends the results to master thread
	if(process_id != 0) {
		for(int id = 1; id < num_processes; i++) {
			MPI_Isend(&process_id, 1, MPI_INT, id, 101, MPI_COMM_WORLD, &request[2]);

		}
	}
	
	// Writes results to file
	if (process_id == 0) {
		wall_time = MPI_Wtime() - wall_time;
		cout <<  wall_time << endl;
		
		for(int id = 1; id < num_processes; i++) {
			MPI_Irecv(&messages[1], 1, MPI_INT, right, 101, MPI_COMM_WORLD, &);
			save_results(out_file_name, vocab, vocab_size_per_book[process_id]);
		}
	}
	
	MPI_Finalize();
	return 0;
}

