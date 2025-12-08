#pragma once

#include <map>
#include <string>

namespace algo::trie {

struct TrieNode {
    std::map<char, TrieNode *> children;
    bool isEnd = false;
};

class Trie {
  private:
    TrieNode *root;

  public:
    Trie() { root = new TrieNode(); }

    void insert(const std::string &s) {
        TrieNode *curr = root;
        for (const char c : s) {
            if (!curr->children[c])
                curr->children[c] = new TrieNode();
            curr = curr->children[c];
        }
        curr->isEnd = true;
    }

    bool search(const std::string &s) {
        TrieNode *curr = root;
        for (const char c : s) {
            if (!curr->children[c]) {
                return false;
            }
            curr = curr->children[c];
        }
        return curr->isEnd;
    }
};

struct BinaryTrieNode {
    BinaryTrieNode *child[2] = {nullptr, nullptr}; // 0 and 1
};

class BinaryTrie {
  private:
    BinaryTrieNode *root;

  public:
    BinaryTrie() { root = new BinaryTrieNode(); }

    void insert(const int num) {
        BinaryTrieNode *curr = root;
        for (int i = 30; i >= 0; i--) { // 31 bits, MSB first
            const int bit = (num >> i) & 1;
            if (!curr->child[bit]) {
                curr->child[bit] = new BinaryTrieNode();
            }
            curr = curr->child[bit];
        }
    }

    int maxXOR(const int num) {
        BinaryTrieNode *curr = root;
        int result = 0;
        for (int i = 30; i >= 0; i--) {
            const int bit = (num >> i) & 1;
            // Try to go opposite direction for max XOR
            const int oppositeBit = 1 - bit;
            if (curr->child[oppositeBit]) {
                result |= (1 << i); // This bit will be 1 in XOR
                curr = curr->child[oppositeBit];
            } else {
                curr = curr->child[bit];
            }
        }
        return result;
    }
};

} // namespace algo::trie
