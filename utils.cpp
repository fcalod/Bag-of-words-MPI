#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

// Probablemente innecesario
/*
// Loads vocab size across all books
int load_vocab_size(string file_name) {
	ifstream in(file_name);
    
    if (!in)
        cerr << "Couldn't read file" << file_name << "\n";
	
	string line;
	getline(in, line);
	return stoi(line);
}*/

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
