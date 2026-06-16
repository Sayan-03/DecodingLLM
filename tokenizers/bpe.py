from collections import Counter, defaultdict
import math
from utilities.DoubleLinkedList import DoubleLinkedList
import heapq
import json
import warnings


class Word:
    def __init__(self, dll, freq):
        self.dll = dll
        self.freq = freq




class BPE():
    def __init__(self):
        self.merge_rule = {} # map to rank the merges
        self.pair_freq = defaultdict(int) # map : tuple -> count
        self.pair_to_word = defaultdict(set) # map : tuple -> set
        self.words = []

    
    def __create_pair(self):
        for element in self.words:
            curr = element.dll.head

            while curr.data != '</w>':
                pair = (curr.data, curr.next.data)
                self.pair_to_word[pair].add(element)
                self.pair_freq[pair] += element.freq
                curr = curr.next
            
        pq = [] # max heap : (-frequence, pair)
        for pair in self.pair_freq.keys():
            heapq.heappush(pq, (-self.pair_freq[pair], pair))
        
        return pq


    def __merge(self, candidate, rank):
        pair = candidate[1]
        self.merge_rule[pair] = rank

        for element in set(self.pair_to_word[pair]):
            curr = element.dll.head

            while curr is not None and curr.next is not None:

                if curr.data == pair[0] and curr.next.data == pair[1]:
                    self.pair_freq[pair] -= element.freq
                    heapq.heappush(self.pq, (-self.pair_freq[pair], pair))

                    curr.data = pair[0] + pair[1]

                    if curr.next.next is not None:
                        self.pair_freq[(pair[1], curr.next.next.data)] -= element.freq
                        heapq.heappush(self.pq, (-self.pair_freq[(pair[1], curr.next.next.data)], (pair[1], curr.next.next.data)))

                        curr.next = curr.next.next
                        curr.next.prev = curr

                        self.pair_freq[(curr.data, curr.next.data)] += element.freq
                        self.pair_to_word[(curr.data, curr.next.data)].add(element)
                        heapq.heappush(self.pq, (-self.pair_freq[(curr.data, curr.next.data)], (curr.data, curr.next.data)))
                    else:
                        curr.next = None
                        element.dll.tail = curr
                    
                    if curr.prev is not None:
                        self.pair_freq[(curr.prev.data, pair[0])] -= element.freq
                        heapq.heappush(self.pq, (-self.pair_freq[(curr.prev.data, pair[0])], (curr.prev.data, pair[0])))

                        self.pair_freq[(curr.prev.data, curr.data)] += element.freq
                        self.pair_to_word[(curr.prev.data, curr.data)].add(element)
                        heapq.heappush(self.pq, (-self.pair_freq[(curr.prev.data, curr.data)], (curr.prev.data, curr.data)))

                curr = curr.next
        
        self.pair_freq.pop(pair, None)
        self.pair_to_word.pop(pair, None)

        return


    def train(self, corpus, num_merges):
        word_freq = dict(Counter(corpus))
        
        for word in word_freq.keys():
            dll = DoubleLinkedList()

            for character in list(word) + ['</w>']:
                dll.push_back(character)

            element = Word(dll, word_freq[word])
            self.words.append(element)

        self.pq = self.__create_pair()

        rank = 0
        while self.pq and num_merges > 0:
            candidate = heapq.heappop(self.pq)

            ## validate before merging
            if -candidate[0] > 0 and -candidate[0] == self.pair_freq.get(candidate[1], 0):
                rank += 1
                self.__merge(candidate, rank)
                num_merges -= 1
        
        print("BPE Done!")

        return


    def encode(self, text):
        words = text.split(" ")

        for i in range(len(words)):
            dll = DoubleLinkedList()

            for character in list(words[i]) + ['</w>']:
                dll.push_back(character)

            words[i] = dll

        encoded_text = []

        for word in words:
            pq = []
            curr = word.head
            tiebreak = 0

            while curr.data != '</w>':
                rank = self.merge_rule.get((curr.data, curr.next.data), math.inf)

                if rank != math.inf:
                    tiebreak += 1
                    heapq.heappush(pq, (rank, tiebreak, curr, curr.data, curr.next.data))
                curr = curr.next
            
            while pq:
                smallest = heapq.heappop(pq)
                node =  smallest[2]

                ## validate before merging
                if node.next is not None and node.data == smallest[3] and node.next.data == smallest[4]:
                    node.data = smallest[3] + smallest[4]

                    ## validate if we have merged the last two pairs or not
                    if node.next.next is not None:
                        node.next = node.next.next
                        node.next.prev = node
                    else:
                        node.next = None
                        word.tail = node

                    ## push the new pairs
                    ## check if the next element exists or not
                    if node.next is not None:
                        rank = self.merge_rule.get((node.data, node.next.data), math.inf)

                        if rank != math.inf:
                            tiebreak += 1
                            heapq.heappush(pq, (rank, tiebreak, node, node.data, node.next.data))

                    ## check if the previous element exists or not
                    if node.prev is not None:
                        rank = self.merge_rule.get((node.prev.data, node.data), math.inf)

                        if rank != math.inf:
                            tiebreak += 1
                            heapq.heappush(pq, (rank, tiebreak, node.prev, node.prev.data, node.data))

            curr = word.head
            
            while curr is not None:
                encoded_text.append(curr.data)
                curr = curr.next

        return encoded_text


    def decode(self, encoded_text):
        dec_str_1 = ["".join(word) for word in encoded_text]
        dec_str_2 = "".join(dec_str_1)
        
        output = dec_str_2.split('</w>')[ : -1]
        decoded_output = " ".join(output)

        return decoded_output
    
    def export_merges(self, path):
        # merge_rule is {(a,b): rank}; emit pairs ordered by rank
        pairs_in_order = [list(pair) for pair, _ in sorted(self.merge_rule.items(), key=lambda kv: kv[1])]
        with open(path, "w") as f:
            json.dump(pairs_in_order, f, indent=2)
        
        print(f"Merge rules are saved at : {path}")

        return

    def load_merges(self, path):
        with open(path) as f:
            pairs = json.load(f)
        self.merge_rule = {(a, b): rank for rank, (a, b) in enumerate(pairs)}

        print(f"Merge rules loaded from: {path}")

        return


if __name__ == "__main__":

    BPE_tokenizer = BPE()

    BPE_tokenizer.train([
        "low", "low", "low", "low", "low",
        "lower", "lower",
        "newest", "newest", "newest", "newest", "newest", "newest",
        "widest", "widest", "widest"], 10)
    
    BPE_tokenizer.export_merges("tokenizers/merge_rules.json")
    BPE_tokenizer.load_merges("tokenizers/merge_rules.json")

    test_text = "low newest zero"
    encoded_text = BPE_tokenizer.encode(test_text)
    decoded_text = BPE_tokenizer.decode(encoded_text)
    print(f"Original text: {test_text}")
    print(f"Encoded text: {encoded_text}")
    print(f"Decoded text: {decoded_text}")