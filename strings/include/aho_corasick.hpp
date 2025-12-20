#pragma once

#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace algo::strings {

struct Match {
    std::size_t patternId;
    std::size_t endPos;
    Match(const std::size_t id, const std::size_t end) : patternId(id), endPos(end) {}
    std::size_t startPos(const std::size_t patternLength) const {
        return endPos - patternLength + 1;
    }
    bool operator<(const Match &other) const { return endPos < other.endPos; }
};

struct MatchSpan {
    std::size_t patternId;
    std::size_t startPos;
    std::size_t endPos;
    MatchSpan(const std::size_t id, const std::size_t start, const std::size_t end)
        : patternId(id), startPos(start), endPos(end) {}
    std::size_t length() const { return endPos - startPos + 1; }
    bool overlaps(const MatchSpan &other) const {
        return !(endPos < other.startPos || other.endPos < startPos);
    }
    bool operator<(const MatchSpan &other) const { return startPos < other.startPos; }
};

/**
 * Aho-Corasick Automaton for multi-pattern string matching.
 *
 * Time Complexity: O(n + m + z) where:
 *   - n = text length
 *   - m = sum of all pattern lengths
 *   - z = number of matches found
 *
 * Three key components:
 * 1. Trie: Stores all patterns in a prefix tree
 * 2. Failure Links: Like KMP's LPS array, but for tree nodes - Points to longest proper
 * suffix that exists in the trie
 * 3. Output Links: Points to other patterns that are suffixes of current match - Allows
 * reporting all overlapping matches efficiently
 */
class AhoCorasick {
  private:
    struct Node {
        std::unordered_map<char, std::unique_ptr<Node>> children;
        Node *fail;
        Node *output;
        // Pattern IDs that end at this node (can have multiple due to duplicates)
        std::vector<std::size_t> patternIds;
        Node() : fail(nullptr), output(nullptr) {}
    };

    std::unique_ptr<Node> root;
    std::size_t patternCount;
    std::vector<std::string> patterns;
    bool built;
    bool caseInsensitive;

    std::string normalise(const std::string &s) const {
        if (!caseInsensitive) {
            return s;
        }
        std::string result = s;
        for (char &c : result) {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        return result;
    }

    /**
     * This is the KMP-style fallback: keep following failure links
     * until we find a node that has a child matching our character
     *
     * Sets f to the node representing c in the failure function, if it exists.
     */
    void failAndFind(Node *&f, const char c) const {
        while (f != root.get() && !f->children.count(c)) {
            f = f->fail;
        }
        // Try to extend the match
        const auto it = f->children.find(c);
        if (it != f->children.end()) {
            f = it->second.get();
        }
    }

  public:
    explicit AhoCorasick(bool caseInsensitive = false)
        : patternCount(0), built(false), caseInsensitive(caseInsensitive) {
        root = std::make_unique<Node>();
        root->fail = root.get();
        root->output = root.get();
    }

    /**
     * @param pattern The pattern string to insert
     * @return The ID assigned to this pattern (0-indexed)
     *
     * Time: O(|pattern|)
     *
     * This builds the trie structure, identical to a standard trie insertion.
     * Each pattern gets a unique ID for later identification in search results.
     * Must call build() after all patterns are inserted.
     */
    std::size_t insert(const std::string &pattern) {
        if (built) {
            built = false;
        }
        const std::string normalisedPattern = normalise(pattern);
        Node *at = root.get();
        for (const char c : normalisedPattern) {
            if (!at->children[c]) {
                at->children[c] = std::make_unique<Node>();
            }
            at = at->children[c].get();
        }
        const std::size_t id = patternCount++;
        at->patternIds.emplace_back(id);
        patterns.emplace_back(pattern);
        return id;
    }

    /**
     * Build failure links and output links using BFS. Must be called after all patterns
     * are inserted and before searching.
     *
     * Time: O(total characters in all patterns)
     *
     * Algorithm:
     * 1. BFS from root to process nodes level by level
     * 2. For each node, find where its failure link should point
     * 3. The failure link points to the longest proper suffix that exists in the trie
     * 4. Output links chain together all patterns that are suffixes of each other
     */
    void build() {
        std::queue<Node *> q;
        for (const auto &[c, childNode] : root->children) {
            childNode->fail = root.get();
            childNode->output = root.get();
            q.push(childNode.get());
        }
        while (!q.empty()) {
            Node *at = q.front();
            q.pop();
            for (const auto &[c, childNode] : at->children) {
                Node *child = childNode.get();
                q.push(child);
                Node *f = at->fail;
                failAndFind(f, c);
                const auto it = f->children.find(c);
                if (it != f->children.end() && it->second.get() != child) {
                    child->fail = it->second.get();
                } else {
                    child->fail = root.get();
                }
                if (!child->fail->patternIds.empty()) {
                    child->output = child->fail;
                } else {
                    child->output = child->fail->output;
                }
            }
        }
        built = true;
    }

    /**
     * Search for all pattern occurrences in text.
     *
     * @param text The text to search in
     * @return Vector of Match
     *
     * Time: O(|text| + number of matches)
     *
     * The search process is like KMP, but on a trie:
     * 1. Start at root and process each character
     * 2. Try to extend current match by following child edges
     * 3. On mismatch, follow failure links (like KMP fallback)
     * 4. At each node, check for pattern matches (including via output links)
     *
     * Note: endPosition is the index of the last character of the match.
     * To get startPosition: endPosition - patternLength + 1
     */
    std::vector<Match> search(const std::string &text) const {
        std::vector<Match> results;
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                for (const std::size_t patternId : found->patternIds) {
                    results.emplace_back(patternId, i);
                }
                // Follow output link to find patterns that are suffixes
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return results;
    }

    /**
     * Search and return matches grouped by pattern.
     *
     * @param text The text to search in
     * @return Vector where result[i] contains all end positions where pattern i matches
     *
     * This is a convenience wrapper that organises results by pattern ID.
     */
    std::vector<std::vector<std::size_t>> searchByPattern(const std::string &text) const {
        std::vector<std::vector<std::size_t>> results(patternCount);
        const std::vector<Match> matches = search(text);
        for (const Match &match : matches) {
            results[match.patternId].emplace_back(match.endPos);
        }
        for (auto &positions : results) {
            std::sort(positions.begin(), positions.end());
        }
        return results;
    }

    /**
     * Check if any pattern exists in the text (boolean search).
     * More efficient than full search if you only need to know if ANY pattern matches.
     *
     * @param text The text to search in
     * @return true if any pattern is found, false otherwise
     *
     * Time: O(|text|) worst case, but can early-exit on first match
     */
    bool contains(const std::string &text) const {
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                if (!found->patternIds.empty()) {
                    // Early exit on first match
                    return true;
                }
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return false;
    }

    /**
     * Count total number of pattern occurrences in text.
     * Useful when you only need the count, not the positions.
     *
     * @param text The text to search in
     * @return Total number of pattern matches (counting overlaps)
     *
     * Time: O(|text| + number of matches)
     */
    std::size_t count(const std::string &text) const {
        std::size_t totalMatches = 0;
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *at = at;
            while (at != root.get()) {
                totalMatches += at->patternIds.size();
                if (at->output == root.get()) {
                    break;
                }
                at = at->output;
            }
        }
        return totalMatches;
    }

    /**
     * Find the first occurrence of any pattern in text.
     * Returns the earliest position where any pattern matches.
     *
     * @param text The text to search in
     * @return Pair of (patternId, endPosition) for first match, or (SIZE_MAX, SIZE_MAX)
     * if none
     *
     * Time: O(|text|) worst case, early-exits on first match
     */
    std::pair<std::size_t, std::size_t> findFirst(const std::string &text) const {
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                if (!found->patternIds.empty()) {
                    return {found->patternIds[0], i};
                }
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return {SIZE_MAX, SIZE_MAX};
    }

    /**
     * Replace all pattern occurrences with replacement strings.
     * Handles overlapping patterns by replacing left-to-right (leftmost match wins).
     *
     * @param text The text to process
     * @param replacements Vector of replacement strings (indexed by pattern ID)
     * @return Modified text with all patterns replaced
     *
     * Time: O(|text| + |result|)
     *
     * Note: For overlapping matches, the leftmost match takes precedence.
     * If a pattern is matched, any overlapping matches to the right are skipped.
     */
    std::string replace(const std::string &text,
                        const std::vector<std::string> &replacements) const {
        const auto matches = search(text);
        if (matches.empty()) {
            return text;
        }
        std::vector<MatchSpan> spans;
        for (const auto &match : matches) {
            const auto startPos = match.startPos(patterns[match.patternId].length());
            spans.emplace_back(startPos, match.endPos, match.patternId);
        }
        std::sort(spans.begin(), spans.end());
        std::string result;
        std::size_t lastPos = 0;
        for (const MatchSpan &span : spans) {
            if (span.startPos < lastPos) {
                continue;
            }
            result += text.substr(lastPos, span.startPos - lastPos);
            result += replacements[span.patternId];
            lastPos = span.endPos + 1;
        }
        result += text.substr(lastPos);
        return result;
    }

    /**
     * Search with a maximum number of matches.
     * Useful for limiting results in large texts or implementing "find next"
     * functionality.
     *
     * @param text The text to search in
     * @param maxMatches Maximum number of matches to return
     * @return Vector of (patternId, endPosition) pairs (at most maxMatches)
     *
     * Time: O(|text|) worst case, but early-exits after maxMatches found
     */
    std::vector<Match> searchLimited(const std::string &text,
                                     std::size_t maxMatches) const {
        std::vector<Match> results;
        results.reserve(maxMatches);
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                for (const std::size_t patternId : found->patternIds) {
                    results.emplace_back(patternId, i);
                    if (results.size() >= maxMatches) {
                        return results;
                    }
                }
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return results;
    }

    /**
     * Search within a range of the text.
     * Useful for chunked processing or searching specific sections.
     *
     * @param text The text to search in
     * @param start Starting index (inclusive)
     * @param end Ending index (exclusive)
     * @return Vector of (patternId, endPosition) pairs within range
     *
     * Time: O(|range| + matches)
     *
     * Note: The automaton state carries over from start, so matches may be found
     * that started before the range but end within it. For true range isolation,
     * use searchRange with reset (create a substring).
     */
    std::vector<Match> searchRange(const std::string &text, const std::size_t start,
                                   std::size_t end) const {
        std::vector<Match> results;
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        end = std::min(end, normalisedText.length());
        for (std::size_t i = start; i < end; i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                for (const std::size_t patternId : found->patternIds) {
                    results.emplace_back(patternId, i);
                }
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return results;
    }

    /**
     * Search with a callback function.
     * Calls the callback for each match found, allowing early termination.
     *
     * @param text The text to search in
     * @param callback Function called with (patternId, endPosition) for each match.
     *                 Return false to stop searching early.
     * @return Number of matches processed before stopping (or all matches if completed)
     *
     * Time: O(|text| + matches) or less if early termination
     *
     * Example:
     *   ac.searchWithCallback(text, [](size_t id, size_t pos) {
     *       std::cout << "Found pattern " << id << " at " << pos << "\n";
     *       return true; // continue
     *   });
     */
    std::size_t searchWithCallback(
        const std::string &text,
        const std::function<bool(std::size_t, std::size_t)> callback) const {
        std::size_t matchCount = 0;
        const std::string normalisedText = normalise(text);
        Node *at = root.get();
        for (std::size_t i = 0; i < normalisedText.length(); i++) {
            const char c = normalisedText[i];
            failAndFind(at, c);
            Node *found = at;
            while (found != root.get()) {
                for (const std::size_t patternId : found->patternIds) {
                    matchCount++;
                    if (!callback(patternId, i)) {
                        return matchCount;
                    }
                }
                if (found->output == root.get()) {
                    break;
                }
                found = found->output;
            }
        }
        return matchCount;
    }

    /**
     * Iterator for streaming search results.
     * Allows processing matches one at a time without storing all results in memory.
     * Useful for very large texts or when you want to stop early.
     */
    class SearchIterator {
      private:
        const AhoCorasick *ac;
        std::string text;
        std::size_t pos;
        Node *at;
        std::pair<std::size_t, std::size_t> currentMatch;
        bool hasMatch;
        // For iterating through output links at current position
        Node *outputIter;
        std::size_t outputIdx;

        void advance() {
            hasMatch = false;
            while (outputIter != ac->root.get()) {
                if (outputIdx < outputIter->patternIds.size()) {
                    currentMatch = {outputIter->patternIds[outputIdx], pos - 1};
                    outputIdx++;
                    hasMatch = true;
                    return;
                }
                if (outputIter->output == ac->root.get()) {
                    break;
                }
                outputIter = outputIter->output;
                outputIdx = 0;
            }
            while (pos < text.length()) {
                const char c = text[pos];
                while (at != ac->root.get() && !at->children.count(c)) {
                    at = at->fail;
                }
                const auto it = at->children.find(c);
                if (it != at->children.end()) {
                    at = it->second.get();
                }
                pos++;
                outputIter = at;
                outputIdx = 0;
                while (outputIter != ac->root.get()) {
                    if (outputIdx < outputIter->patternIds.size()) {
                        currentMatch = {outputIter->patternIds[outputIdx], pos - 1};
                        outputIdx++;
                        hasMatch = true;
                        return;
                    }
                    if (outputIter->output == ac->root.get()) {
                        break;
                    }
                    outputIter = outputIter->output;
                    outputIdx = 0;
                }
            }
        }

      public:
        SearchIterator(const AhoCorasick *ac, const std::string &text)
            : ac(ac), text(ac->normalise(text)), pos(0), at(ac->root.get()),
              hasMatch(false), outputIter(nullptr), outputIdx(0) {
            advance();
        }

        bool hasNext() const { return hasMatch; }

        std::pair<std::size_t, std::size_t> next() {
            if (!hasMatch) {
                return {SIZE_MAX, SIZE_MAX};
            }
            const auto result = currentMatch;
            advance();
            return result;
        }
    };

    /**
     * Create an iterator for streaming search results. Potentially a little bit of a
     * weird use case, but could be really cool
     *
     * @param text The text to search in
     * @return SearchIterator that yields matches one at a time
     *
     * Example usage:
     *   auto it = ac.iterate(text);
     *   while (it.hasNext()) {
     *       auto [patternId, pos] = it.next();
     *       // Process match...
     *   }
     */
    SearchIterator iterate(const std::string &text) const {
        return SearchIterator(this, text);
    }

    /**
     * Get statistics about the automaton structure.
     * Useful for debugging and understanding memory usage.
     */
    struct Statistics {
        std::size_t nodeCount;
        std::size_t edgeCount;
        std::size_t maxDepth;
        std::size_t patternCount;
    };

    void dfs(Node *node, std::size_t depth, Statistics &stats) const {
        stats.nodeCount++;
        stats.maxDepth = std::max(stats.maxDepth, depth);
        for (const auto &[c, childNode] : node->children) {
            stats.edgeCount++;
            dfs(childNode.get(), depth + 1, stats);
        }
    }

    Statistics getStatistics() const {
        Statistics stats = {0, 0, 0, patternCount};
        dfs(root.get(), 0, stats);
        return stats;
    }

    bool isBuilt() const { return built; }

    bool isCaseInsensitive() const { return caseInsensitive; }

    /**
     * Clear all patterns from the automaton.
     * Resets to initial empty state.
     */
    void clear() {
        root = std::make_unique<Node>();
        root->fail = root.get();
        patternCount = 0;
        patterns.clear();
        built = false;
    }
};

} // namespace algo::strings
