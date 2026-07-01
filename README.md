# Spell Checker & Auto-Complete Engine (Trie + Fuzzy Search)

A C++ implementation of a spell checker and auto-complete system built on a **Trie** data structure, combining hash maps, DFS traversal, and edit-distance search to deliver fast prefix completion and typo correction.

## Overview

This project implements a dictionary-backed Trie that supports:
- Exact word lookup
- Prefix-based auto-complete, ranked by word frequency
- Typo correction using Levenshtein-distance fuzzy search directly over the Trie
- Word frequency tracking via a hash map

Instead of comparing a misspelled word against every dictionary entry individually (an O(n × m) approach), the spell checker walks the Trie once and propagates a running edit-distance row down each branch, pruning any subtree whose best-case distance already exceeds the allowed threshold. This makes suggestion lookups scale with the shape of the dictionary rather than its size.

## Data Structures & Algorithms

| Component | Technique |
|---|---|
| Word storage | Trie (prefix tree) |
| Trie node children | Hash map (`unordered_map<char, TrieNode*>`) |
| Auto-complete | DFS traversal from prefix node |
| Frequency lookup | Hash map (`unordered_map<string, int>`) |
| Typo correction | DFS + incremental Levenshtein distance (dynamic programming row) |

## Features

- **Insert** — add words to the dictionary with an optional frequency weight
- **Search** — O(word length) exact match lookup
- **Prefix search** — check whether any word starts with a given prefix
- **Auto-complete** — return top-k completions for a prefix, ranked by frequency
- **Spell check** — return the closest dictionary words within a configurable edit-distance, ranked by frequency
- **Word frequency** — O(1) frequency lookup for any word

## Build & Run

```bash
g++ -std=c++17 -O2 -Wall -o spell_checker spell_checker_autocomplete.cpp
./spell_checker
```

## Example Output

```
=== Autocomplete for 'app' ===
  apple (freq: 50)
  application (freq: 30)
  apply (freq: 20)
  appreciate (freq: 10)

=== Spell Check for 'aple' (typo of 'apple') ===
  apple (freq: 50)
  apply (freq: 20)
```

## Complexity

| Operation | Time Complexity |
|---|---|
| Insert | O(L) — L = word length |
| Search | O(L) |
| Prefix search | O(P) — P = prefix length |
| Auto-complete | O(P + K) — K = number of matching nodes in subtree |
| Spell check (fuzzy) | O(N × L) worst case, pruned in practice — N = Trie nodes visited |

## Possible Extensions

- Wrap in an interactive CLI/REPL for live typing demos
- Expose as a REST API (Flask/Express) with a lightweight front-end
- Load a large dictionary file (e.g. `words.txt`) instead of the hardcoded sample
- Add weighted ranking that combines edit distance and frequency
