#ifndef TRIE_H
#define TRIE_H

struct TrieNode {
    TrieNode* children[26];
    bool isEnd;
};

class Trie {
private:
    TrieNode* root;
public:
    Trie();
    void insert(string word);
    bool search(string word);
};

#endif
