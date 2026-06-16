#pragma once

#include <DoubleLinkedList.hpp>

#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <queue>

class Word{
    private:
        DoubleLinkedList dll;
        int freq;
    
    public:
        // Constructor
        Word(DoubleLinkedList dll, int freq);

        friend class BPE;
};


class BPE{
    private:
        std::map<std::pair<std::string, std::string>, int> merge_rule;
        std::map<std::pair<std::string, std::string>, int> pair_freq;
        std::map<std::pair<std::string, std::string>, std::unordered_set<Word>> pair_to_word;
        std::vector<Word> words;
        std::priority_queue<std::pair<int, std::pair<std::string, std::string>>> pq;

        std::priority_queue<std::pair<int, std::pair<std::string, std::string>>> create_pair();
        void merge(std::pair<int, std::pair<std::string, std::string>>& candidate, int rank);

    public:
        // Constructor 
        BPE();

        // APIs
        void train(std::vector<std::string>& corpus, int num_merges);
        std::vector<std::string> encode(std::string& text);
        std::string decode(std::vector<std::string>& encoded_text);
        void BPE::export_merges(std::string path);
        void BPE::load_merges(std::string path);

        // Destructor
        ~BPE();
};