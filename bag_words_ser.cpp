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
void process_book(string in_file_name, map<string, int>& vocab, int &tot_word_count) {
	ifstream in(in_file_name);
    
    if(!in)
        cerr << "Couldn't read file: " << in_file_name << "\n";
	
	string line, val; // store lines from the file and words within each line
	
	// Reads file line by line
	while(getline(in, line)) {
		stringstream ss(line);
		
		// Splits each line by commas  
		while(ss.good()) {//!ss.eof()
			getline(ss, val, ',');
			string word = val;
			
			// Checks if the word is in the vocab
			// (maps don't throw errors when accessing non-existant keys)
			if(vocab.count(word) > 0) 
				vocab[word]++;
			
			tot_word_count++;
		}		
	}
	
	in.close();
}

// Writes output to file
void save_results(string out_file_name, map<string, int>& vocab, int& vocab_size_per_book) {
	ofstream out;
	out.open(out_file_name, ios_base::app); // Append mode
	vocab_size_per_book = 0;
	
	int counter = 0;
	
	// Writes word counts
	for(auto const& [word, count] : vocab) {
		out << count << ",";
		
		if(count > 0)
			vocab_size_per_book++;
	}
	
	out << "\n";
	out.close();
}

// Ejecutar con ./bag_words_ser 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities vocab.csv 15164

// Ejecutar con .bag_words_ser 6_test vocab.csv para probar
 
int main (int argc, char *argv[]) {
	map <string, int> vocab; // Word counts for current book 
	int tot_word_count = 0; // Total number of words per book
	int vocab_size_per_book[argc - 3]; // Unique words per book
	vector<string> const file_names{ argv + 1, argv + argc - 2 }; // Stores cmd line input in a vector
	int book_indx = 0;
	double total_time = 0;
	double start, end;
	
	string vocab_file = argv[argc - 2];
	load_vocab(vocab_file, vocab);
	string out_file_name = "results/BagOfWords_Serial.csv";
	
	write_headers(out_file_name, vocab);
	
	// Loops over the list of books
	for(string file: file_names) {
		auto start = chrono::high_resolution_clock::now();
		string in_file_name = "sample_data/" + file + ".txt";
		
		// Counts words from the current book
		process_book(in_file_name, vocab, tot_word_count);
		save_results(out_file_name, vocab, vocab_size_per_book[book_indx]);
		
		auto end = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
		cout << "Book: " << book_indx << "    Vocab size: " << vocab_size_per_book[book_indx] 
			 << "  Word count: " << tot_word_count << "  Time: " << (float)duration.count()/1000000 << "s" << endl;
		
		total_time += (float)duration.count()/1000000;
		book_indx++;
		
		// Resets variables
		for(auto const& [word, count] : vocab)
			vocab[word] = 0;
		
		tot_word_count = 0;
	}
	
	cout << "\nTotal time: " << total_time << "s" << endl;
	
	return 0;
}

