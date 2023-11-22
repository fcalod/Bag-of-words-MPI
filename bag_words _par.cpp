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

// Loads vocab size across all books
int load_vocab_size(string file_name) {
	ifstream in(file_name);
    
    if (!in)
        cerr << "Couldn't read file" << file_name << "\n";
	
	string line;
	getline(in, line);
	return stoi(line);
}

// Loads the word map
void load_vocab(string file_name, map<string, int> vocab) {
	ifstream in(file_name);
	cout << "Trying to read: " << file_name<< "\n";
    
    if (!in)
        cerr << "Couldn't read file: " << file_name << "\n";
	
	string line, val; // store lines from the file and words within each line
	
	// Only reads the header
	getline(in, line);
	stringstream ss(line);
	
	// Splits the line by commas and reads each value
	while(!ss.eof()) {
		getline(ss, val, ',');
		string word = val;
		vocab[word] = 0;
	}
}

// Writes output to file
void save_results(string out_file_name, map<string, int>& vocab, int& vocab_size, int& tot_word_count) {
	ofstream out;
	out.open(out_file_name);
	out << "";
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

