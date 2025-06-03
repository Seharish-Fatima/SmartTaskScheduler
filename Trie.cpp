#include "Trie.h"

Trie::Trie() {
    root = new TrieNode();
}

void Trie::insert(string word) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children[c - 'a'])
            node->children[c - 'a'] = new TrieNode();
        node = node->children[c - 'a'];
    }
    node->isEnd = true;
}

bool Trie::search(string word) {
    TrieNode* node = root;
    for (char c : word) {
        if (!node->children[c - 'a'])
            return false;
        node = node->children[c - 'a'];
    }
    return node->isEnd;
}
