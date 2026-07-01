#include <bits/stdc++.h>
using namespace std;

// ---------------------------------------------------------
// Trie Node: uses a Hash Map for children (O(1) avg access)
// ---------------------------------------------------------
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
    int frequency = 0;
};

class Trie {
private:
    TrieNode* root;
    unordered_map<string, int> wordFrequencyMap; // global hash map for O(1) freq lookup

    TrieNode* findNode(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            auto it = node->children.find(ch);
            if (it == node->children.end()) return nullptr;
            node = it->second;
        }
        return node;
    }

    // DFS to collect all complete words under a given node
    void dfsCollect(TrieNode* node, string& current, vector<pair<string, int>>& results) {
        if (node->isEndOfWord) {
            results.push_back({current, node->frequency});
        }
        for (auto& [ch, child] : node->children) {
            current.push_back(ch);
            dfsCollect(child, current, results);
            current.pop_back();
        }
    }

    // DFS + Levenshtein-distance DP row, propagated down the trie.
    // Classic technique: instead of comparing against every dictionary
    // word individually, we walk the trie once and carry a running
    // edit-distance row, pruning branches whose minimum possible
    // distance already exceeds maxDistance.
    void searchFuzzy(TrieNode* node, char ch, const string& target,
                      vector<int>& prevRow, string& current,
                      vector<pair<string, int>>& suggestions, int maxDistance) {
        int columns = (int)target.size() + 1;
        vector<int> currentRow(columns);
        currentRow[0] = prevRow[0] + 1;

        for (int i = 1; i < columns; i++) {
            int insertCost  = currentRow[i - 1] + 1;
            int deleteCost  = prevRow[i] + 1;
            int replaceCost = prevRow[i - 1] + (target[i - 1] == ch ? 0 : 1);
            currentRow[i] = min({insertCost, deleteCost, replaceCost});
        }

        if (currentRow[columns - 1] <= maxDistance && node->isEndOfWord) {
            suggestions.push_back({current, node->frequency});
        }

        if (*min_element(currentRow.begin(), currentRow.end()) <= maxDistance) {
            for (auto& [nextCh, child] : node->children) {
                current.push_back(nextCh);
                searchFuzzy(child, nextCh, target, currentRow, current, suggestions, maxDistance);
                current.pop_back();
            }
        }
    }

public:
    Trie() { root = new TrieNode(); }

    // Insert a word, optionally with a frequency weight
    void insert(const string& word, int freq = 1) {
        TrieNode* node = root;
        for (char ch : word) {
            if (!node->children.count(ch))
                node->children[ch] = new TrieNode();
            node = node->children[ch];
        }
        node->isEndOfWord = true;
        node->frequency += freq;
        wordFrequencyMap[word] += freq;
    }

    // Exact word search
    bool search(const string& word) {
        TrieNode* node = findNode(word);
        return node != nullptr && node->isEndOfWord;
    }

    // Prefix existence check
    bool startsWith(const string& prefix) {
        return findNode(prefix) != nullptr;
    }

    // O(1) frequency lookup via hash map
    int getFrequency(const string& word) {
        auto it = wordFrequencyMap.find(word);
        return it != wordFrequencyMap.end() ? it->second : 0;
    }

    // Auto-complete: DFS from the prefix node, ranked by frequency
    vector<pair<string, int>> autocomplete(const string& prefix, int limit = 5) {
        TrieNode* node = findNode(prefix);
        vector<pair<string, int>> results;
        if (!node) return results;

        string current = prefix;
        dfsCollect(node, current, results);

        sort(results.begin(), results.end(),
             [](auto& a, auto& b) { return a.second > b.second; });

        if ((int)results.size() > limit) results.resize(limit);
        return results;
    }

    // Spell check: exact match first, else fuzzy suggestions within maxDistance
    vector<pair<string, int>> spellCheck(const string& word, int maxDistance = 2, int limit = 5) {
        if (search(word)) return {{word, getFrequency(word)}};

        vector<pair<string, int>> suggestions;
        vector<int> initialRow(word.size() + 1);
        iota(initialRow.begin(), initialRow.end(), 0);

        string current;
        for (auto& [ch, child] : root->children) {
            current.push_back(ch);
            searchFuzzy(child, ch, word, initialRow, current, suggestions, maxDistance);
            current.pop_back();
        }

        sort(suggestions.begin(), suggestions.end(),
             [](auto& a, auto& b) { return a.second > b.second; });

        if ((int)suggestions.size() > limit) suggestions.resize(limit);
        return suggestions;
    }
};

int main() {
    Trie trie;
    vector<pair<string, int>> dictionary = {
        {"apple", 50}, {"application", 30}, {"apply", 20}, {"appreciate", 10},
        {"banana", 25}, {"band", 15}, {"bandana", 5}, {"bandwidth", 8},
        {"cat", 40}, {"catalog", 12}, {"category", 18}, {"cattle", 6}
    };
    for (auto& [word, freq] : dictionary) trie.insert(word, freq);

    cout << "=== Search ===\n";
    cout << "apple -> " << (trie.search("apple") ? "found" : "not found") << "\n";
    cout << "appl  -> " << (trie.search("appl") ? "found" : "not found") << "\n";

    cout << "\n=== Prefix Search ===\n";
    cout << "startsWith('app') -> " << (trie.startsWith("app") ? "true" : "false") << "\n";

    cout << "\n=== Autocomplete for 'app' ===\n";
    for (auto& [w, f] : trie.autocomplete("app", 5))
        cout << "  " << w << " (freq: " << f << ")\n";

    cout << "\n=== Autocomplete for 'ban' ===\n";
    for (auto& [w, f] : trie.autocomplete("ban", 5))
        cout << "  " << w << " (freq: " << f << ")\n";

    cout << "\n=== Spell Check for 'aple' (typo of 'apple') ===\n";
    for (auto& [w, f] : trie.spellCheck("aple", 2, 5))
        cout << "  " << w << " (freq: " << f << ")\n";

    cout << "\n=== Spell Check for 'catt' ===\n";
    for (auto& [w, f] : trie.spellCheck("catt", 2, 5))
        cout << "  " << w << " (freq: " << f << ")\n";

    cout << "\n=== Word Frequency ===\n";
    cout << "apple frequency: " << trie.getFrequency("apple") << "\n";

    return 0;
}
