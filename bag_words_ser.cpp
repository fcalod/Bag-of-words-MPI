#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "utils.cpp"

using namespace std;

// ==================================
// ======== SERIAL VERSION ==========
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

// Ejecutar con ./bag_words_ser 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv

// Ejecutar con .bag_words_ser 6_test vocab.csv para probar

int main (int argc, char *argv[]) {
	map <string, int> vocab; // Word counts for current book 
	//int vocab_size = load_vocab_size("vocab_size.csv"); // Number of unique words
	int tot_word_count = 0; // Total number of words
	int vocab_size_per_book[argc-1]; // Unique words per book
	vector<string> const file_names{ argv + 1, argv + argc - 1 }; // Stores cmd line input in a vector
	int book_indx = 0;
	double total_time = 0;
	double start, end;
	
	string vocab_file = argv[argc-1];
	//load_vocab("vocab_file", vocab);
	load_vocab("vocab.csv", vocab);
	
	// Loops over the list of books
	for(string file: file_names) {
		auto start = chrono::high_resolution_clock::now();
		string in_file_name = "sample_data/" + file + ".txt";
    	string out_file_name = "results/" + file + "_results.csv";
		
		// Counts words from the current book
		process_book(in_file_name, out_file_name, vocab, tot_word_count);
		save_results(out_file_name, vocab, vocab_size_per_book[book_indx], tot_word_count);
		
		auto end = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
		cout << "File: " << file << "  Vocab size: " << vocab_size_per_book[book_indx] 
			 << "  Word count: " << tot_word_count << "  Time: " << (float)duration.count()/1000000 << "s" << endl;
		
		total_time += (float)duration.count()/1000000;
		
		// vocab_size_per_book[book_indx] = ;
		book_indx++;
		
		// Resets variables
		for(auto const& [word, count] : vocab) {
			//cout << word << ": " << vocab[word] << "\n";
			vocab[word] = 0;
		}
		//for(int i = 0; i < ; i++)
			//vocab[i] = 0;
		
		tot_word_count = 0;
	}
	
	cout << "\nTotal time: " << total_time << "s" << endl;
	
	return 0;
}

