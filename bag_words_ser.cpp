#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <chrono>

using namespace std;

// ==================================
// ======== SERIAL VERSION ==========
// ==================================

// Loads the word dict
void load_vocab(string file_name, map<string, long long int> &word_dict, long long int &vocab_size) {
	ifstream in(file_name);
    cout << "Trying to read: " << file_name<< "\n";
    if (!in){
        cerr << "Couldn't read file: " << file_name << "\n";
	}
	string line, val; // store lines from the file and words within each line
	/*
	// Reads file line by line
	while(getline(in, line) && point_number < size) {
		stringstream ss(line);
		
		// Splits each line by spaces
		/*
		while() {
			getline(ss, val, ' ');
			string word = stof(val);
			word_dict[word] = 0;
			
			

			if(word_dict.count(word) == 0)
				vocab_size++;
		}
		
    }
	
    */
}

// Counts words in the current book
void process_book(string file_name, map<string, long long int> &word_dict, long long int &tot_word_count) {
	ifstream in(file_name);
    
    if (!in)
        cerr << "Couldn't read file: " << file_name << "\n";
	
	string line, val; // store lines from the file and words within each line
	
	// Reads file line by line
	//while(getline(in, line) && point_number < size) {
		//stringstream ss(line);
		
		// Splits each line by spaces
		/*
		while() {
			if(word_dict.count(word) == 0)
				word_dict[word] = 1;
			else
				word_dict[word] += 1;
		}		
		*/
	//}
}

// Ejecutar con ./bag_words_ser.exe 0_shakespeare_the_merchant_of_venice 1_shakespeare_romeo_juliet 2_shakespeare_hamlet 3_dickens_a_christmas_carol 4_dickens_oliver_twist 5_dickens_a_tale_of_two_cities

int main (int argc, char *argv[]) {
	map<string, long long int> word_dict;
	long long int tot_word_count = 0; // Total number of words
	long long int vocab_size = 0; // Number of unique words
	vector<string> const file_names{ argv + 1, argv + argc }; // Stores cmd line input in a vector
	
	// Loops over the list of books
	for(string file: file_names) {
		auto start = chrono::high_resolution_clock::now();
		string input_file_name = file + ".txt";
    	string output_file_name = file + "_results.csv";
		
		
		
		
		auto end = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
		cout << "File: " << file << "  Vocab size: " << vocab_size << "  Word count: " << tot_word_count 
		     << "  Time: " << (float)duration.count()/1000000 << "s" << endl;
		
		// Resets variables
		for(auto const& [word, count] : word_dict)
			word_dict[word] = 0;
		
		tot_word_count = 0;
	}
	
	return 0;
}

