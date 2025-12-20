#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace algo::strings {

struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEnd;
    bool hasChildren;
    TrieNode() : isEnd(false), hasChildren(false) {}
};

class Trie {
  private:
    std::unique_ptr<TrieNode> root;

  public:
    Trie() { root = std::make_unique<TrieNode>(); }

    void insert(const std::string &s) {
        TrieNode *curr = root.get();
        for (const char c : s) {
            if (!curr->children[c]) {
                curr->children[c] = std::make_unique<TrieNode>();
                curr->hasChildren = true;
            }
            curr = curr->children[c].get();
        }
        curr->isEnd = true;
    }

    bool search(const std::string &s) {
        TrieNode *curr = root.get();
        for (const char c : s) {
            if (!curr->children[c]) {
                return false;
            }
            curr = curr->children[c].get();
        }
        return curr->isEnd;
    }
};

struct BinaryTrieNode {
    std::vector<std::unique_ptr<BinaryTrieNode>> children;
    BinaryTrieNode() : children(2) {}
};

class BinaryTrie {
  private:
    std::unique_ptr<BinaryTrieNode> root;

  public:
    BinaryTrie() { root = std::make_unique<BinaryTrieNode>(); }

    void insert(const int num) {
        BinaryTrieNode *curr = root.get();
        for (int i = 30; i >= 0; i--) {
            const int bit = (num >> i) & 1;
            if (!curr->children[bit]) {
                curr->children[bit] = std::make_unique<BinaryTrieNode>();
            }
            curr = curr->children[bit].get();
        }
    }

    int maxXOR(const int num) {
        BinaryTrieNode *curr = root.get();
        int result = 0;
        for (int i = 30; i >= 0; i--) {
            const int bit = (num >> i) & 1;
            const int oppositeBit = 1 - bit;
            if (curr->children[oppositeBit]) {
                result |= (1 << i);
                curr = curr->children[oppositeBit].get();
            } else {
                curr = curr->children[bit].get();
            }
        }
        return result;
    }
};

} // namespace algo::strings
