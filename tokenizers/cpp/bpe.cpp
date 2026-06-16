#include <headers/bpe.hpp>
#include <stringUtil.hpp>
#include <json.hpp>

#include <map>
#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <fstream>

using json = nlohmann::json;

Word::Word(DoubleLinkedList dll, int freq) {
    this->dll = dll;
    this->freq = freq;
}




BPE::BPE() {
    // does nothing
}


BPE::~BPE() = default;


std::priority_queue<std::pair<int, std::pair<std::string, std::string>>> BPE::create_pair() {
    for(auto& element : BPE::words) {
        Node* curr = element.dll.head;

        while(curr->data != "</w>") {
            std::pair<std::string, std::string> curr_pair = {curr->data, curr->next->data};
            BPE::pair_to_word[curr_pair].insert(element);
            BPE::pair_freq[curr_pair] += element.freq;
            curr = curr->next;
        }
    }

    std::priority_queue<std::pair<int, std::pair<std::string, std::string>>> pq;
    for(auto& it : BPE::pair_freq) {
        pq.push({it.second, it.first});
    }

    return pq;
}


void BPE::merge(std::pair<int, std::pair<std::string, std::string>>& candidate, int rank) {
    auto pair = candidate.second;
    BPE::merge_rule[pair] = rank;

    std::vector<Word> local_copy(BPE::pair_to_word[pair].begin(), BPE::pair_to_word[pair].end());
    for(auto& element : local_copy) {
        Node* curr = element.dll.head;

        while(curr != nullptr && curr->next != nullptr) {
            if(curr->data == pair.first && curr->next->data == pair.second) {
                BPE::pair_freq[pair] -= element.freq;
                BPE::pq.push({BPE::pair_freq[pair], pair});
                
                curr->data = pair.first + pair.second;

                if(curr->next->next != nullptr) {
                    BPE::pair_freq[{pair.second, curr->next->next->data}] -= element.freq;
                    BPE::pq.push({BPE::pair_freq[{pair.second, curr->next->next->data}], {pair.second, curr->next->next->data}});

                    curr->next = curr->next->next;
                    curr->next->prev = curr;

                    BPE::pair_freq[{curr->data, curr->next->data}] += element.freq;
                    BPE::pair_to_word[{curr->data, curr->next->data}].insert(element);
                    BPE::pq.push({BPE::pair_freq[{curr->data, curr->next->data}], {curr->data, curr->next->data}});
                }
                else {
                    curr->next = nullptr;
                    element.dll.tail = curr;
                }

                if(curr->prev != nullptr) {
                    BPE::pair_freq[{curr->prev->data, pair.first}] -= element.freq;
                    BPE::pq.push({BPE::pair_freq[{curr->prev->data, pair.first}], {curr->prev->data, pair.first}});

                    BPE::pair_freq[{curr->prev->data, curr->data}] += element.freq;
                    BPE::pair_to_word[{curr->prev->data, curr->data}].insert(element);
                    BPE::pq.push({BPE::pair_freq[{curr->prev->data, curr->data}], {curr->prev->data, curr->data}});
                }
            }

            curr = curr->next;
        }
    }

    BPE::pair_freq.erase(pair);
    BPE::pair_to_word.erase(pair);

    return;
}


void BPE::train(std::vector<std::string>& corpus, int num_merges) {
    std::map<std::string, int> word_freq;
    for(auto& it : corpus) {
        word_freq[it] ++;
    }

    for(auto& it : word_freq) {
        DoubleLinkedList dll;

        for(auto& ch : it.first) {
            dll.push_back(std::to_string(ch));
        }
        dll.push_back("</w>");

        Word element(dll, it.second);
        BPE::words.push_back(element);
    }

    BPE::pq = BPE::create_pair();

    int rank = 0;
    while(!BPE::pq.empty() && num_merges > 0) {
        auto candidate = BPE::pq.top();
        BPE::pq.pop();

        if(candidate.first > 0 && candidate.first == BPE::pair_freq.count(candidate.second)) {
            rank += 1;
            BPE::merge(candidate, rank);
            num_merges -= 1;
        }
    }

    std::cout << "BPE Done" << std::endl;

    return;
}


std::vector<std::string> BPE::encode(std::string& text) {
    std::vector<std::string> words_1 = StringUtil::split(text, ' ');
    std::vector<DoubleLinkedList> words_2(words_1.size());

    for(size_t i = 0; i < words_1.size(); i ++) {
        DoubleLinkedList dll;

        for(auto& ch : words_1[i]) {
            dll.push_back(std::string(1, ch));
        }
        dll.push_back("</w>");

        words_2[i] = dll;
    }

    std::vector<std::string> encoded_text;

    for(auto& word : words_2) {
        std::priority_queue<std::tuple<int, int, Node*, std::string, std::string>,
                            std::vector<std::tuple<int, int, Node*, std::string, std::string>>,
                            std::greater<std::tuple<int, int, Node*, std::string, std::string>>> pq;

        Node* curr = word.head;
        int tiebreak = 0;

        while(curr->data != "</w>") {
            int rank = INT_MAX;
            if(auto it = BPE::merge_rule.find({curr->data, curr->next->data}); it != BPE::merge_rule.end()) {
                rank = it->second;
            }

            if(rank != INT_MAX) {
                tiebreak += 1;
                pq.push({rank, tiebreak, curr, curr->data, curr->next->data});
            }

            curr = curr->next;
        }

        while(!pq.empty()) {
            auto smallest = pq.top();
            pq.pop();

            Node* node = std::get<2>(smallest);

            if(node->next != nullptr && node->data == std::get<3>(smallest) && node->next->data == std::get<4>(smallest)) {
                node->data = std::get<3>(smallest) + std::get<4>(smallest);

                if(node->next->next != nullptr) {
                    node->next = node->next->next;
                    node->next->prev = node;
                }
                else {
                    node->next = nullptr;
                    word.tail = node;
                }

                if(node->prev != nullptr) {
                    int rank = INT_MAX;
                    if(auto it = BPE::merge_rule.find({node->prev->data, node->data}); it != BPE::merge_rule.end()) {
                        rank = it->second;
                    }

                    if(rank != INT_MAX) {
                        tiebreak += 1;
                        pq.push({rank, tiebreak, node->prev, node->prev->data, node->data});
                    }
                }
            }
        }

        curr = word.head;
        while(curr != nullptr) {
            encoded_text.push_back(curr->data);
            curr = curr->next;
        }
    }

    return encoded_text;
}


std::string BPE::decode(std::vector<std::string>& encoded_text) {
    std::string full_text = "";
    for (const auto& token : encoded_text) {
        full_text += token;
    }

    std::vector<std::string> words = StringUtil::split(full_text, "</w>");

    std::string decoded_output = "";
    size_t limit = words.empty() ? 0 : words.size() - 1;

    for (size_t i = 0; i < limit; i++) {
        if (i > 0) {
            decoded_output += " ";
        }
        decoded_output += words[i];
    }

    return decoded_output;
}


void BPE::export_merges(std::string path) {
    std::vector<std::pair<std::pair<std::string, std::string>, int>> sorted_rules(BPE::merge_rule.begin(), BPE::merge_rule.end());

    std::sort(sorted_rules.begin(), sorted_rules.end(), 
        [](const auto& a, const auto& b) {
            return a.second < b.second; 
        }
    );

    json j_pairs = json::array();

    for (const auto& rule : sorted_rules) {
        json pair_arr = {rule.first.first, rule.first.second};
        j_pairs.push_back(pair_arr);
    }

    std::ofstream file(path);

    if (file.is_open()) {
        file << j_pairs.dump(2); 
        file.close();
        std::cout << "Merge rules are saved at : " << path << std::endl;
    }
    else {
        std::cerr << "Error: Could not open file for writing at " << path << std::endl;
    }

    return;
}

void BPE::load_merges(std::string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading at " << path << std::endl;
        
        return;
    }

    json j_pairs;
    file >> j_pairs;
    file.close();

    BPE::merge_rule.clear();

    int rank = 0;
    for (const auto& item : j_pairs) {
        
        if (item.is_array() && item.size() == 2) {
            std::string a = item[0].get<std::string>();
            std::string b = item[1].get<std::string>();
            
            BPE::merge_rule[{a, b}] = rank;
            rank++;
        }
    }

    std::cout << "Merge rules loaded from: " << path << std::endl;

    return;
}