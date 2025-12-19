#pragma once

#include <memory>
#include <string>
#include <vector>

namespace algo::ds {

struct TrieNode {
    std::vector<std::unique_ptr<TrieNode>> children;
    bool isEnd;
    bool hasChildren;
    TrieNode() : children(62), isEnd(false), hasChildren(false) {}
};

class Trie {
  private:
    std::unique_ptr<TrieNode> root;

    int charToIdx(const char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'A' && c <= 'Z') {
            return c - 'A' + 10;
        }
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 36;
        }
        return -1;
    }

  public:
    Trie() { root = std::make_unique<TrieNode>(); }

    void insert(const std::string &s) {
        TrieNode *curr = root.get();
        for (const char c : s) {
            const int idx = charToIdx(c);
            if (!curr->children[idx]) {
                curr->children[idx] = std::make_unique<TrieNode>();
                curr->hasChildren = true;
            }
            curr = curr->children[idx].get();
        }
        curr->isEnd = true;
    }

    bool search(const std::string &s) {
        TrieNode *curr = root.get();
        for (const char c : s) {
            const int idx = charToIdx(c);
            if (!curr->children[idx]) {
                return false;
            }
            curr = curr->children[idx].get();
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

} // namespace algo::ds
