#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <climits>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

typedef long long ll;

struct MemoryAccess {
    ll time_stamp;
    ll offset;
    enum AccessType {
        Read,
        Write
    } type;

   MemoryAccess(ll _time_stamp, ll _offset, string _type) {
    time_stamp = _time_stamp;
        offset = _offset;
        if (_type == "Write") {
            type = Write;
        } else {
            type = Read;
        }
    }

};

// Custom comparator to sort cache by decreasing "next use" time
struct CompareNextUse {
    bool operator()(const pair<int, long long int> &a, const pair<int, long long int> &b) const {
        return a.first > b.first;  // Sort by "next use" time in descending order
    }
};

// Function to preprocess future occurrences of pages
unordered_map<long long int, vector<int>> preprocess_future_occurrences(const vector<long long int> &sequence, int piece_count, int piece_num) {
    unordered_map<long long int, vector<int>> future_occurrences;
    int size = sequence.size();
    int ceill = (size + piece_count - 1) / piece_count;
    int start = ceill * piece_num;
    int end = min(ceill * (piece_num + 1), size) - 1;

    for (int i = end; i >= start; --i) {
        long long int page = sequence[i];
        if (future_occurrences.find(page) == future_occurrences.end()) {
            future_occurrences[page].push_back(0);  // Dummy value for cold miss detection
        }
        future_occurrences[page].push_back(i);  // Store future occurrence index
    }
    return future_occurrences;
}

// Function to simulate the optimal cache replacement algorithm using std::set
void optimal_cache_replacement_with_set(int cache_size, const vector<long long int> &sequence, int piece_count) {
    multiset<pair<int, long long int>, CompareNextUse> cache;  // Cache uses a custom comparator for decreasing order
    unordered_map<long long int, multiset<pair<int, long long int>>::iterator> cache_map;  // Map storing iterators to set elements
    int cache_misses = 0, out_misses = 0, cold_misses = 0;

    for (int j = 0; j < piece_count; j++) {
        // Preprocess future occurrences of pages in the sequence
        unordered_map<long long int, vector<int>> future_occurrences = preprocess_future_occurrences(sequence, piece_count, j);

        // Simulate page requests
        for (int i = 0; i < sequence.size() / piece_count; ++i) {
            long long int page = sequence[(sequence.size() / piece_count) * j + i];
            future_occurrences[page].pop_back();

            int next_use = future_occurrences[page].size() > 1 ? future_occurrences[page].back() : INT_MAX;

            if (cache_map.find(page) == cache_map.end()) {
                // Cache miss
                cache_misses++;

                if (cache.size() == cache_size) {
                    // Evict the page with the biggest "next use" (i.e., the one at the beginning)
                    auto it = cache.begin();
                    if (it->first > next_use) {
                        cache_map.erase(it->second);  // Remove from cache_map
                        cache.erase(it);  // Remove from cache set

                        if (future_occurrences[page][0] == 0) {
                            cold_misses++;  // Detect cold miss
                            future_occurrences[page][0] = 1;
                        }

                        auto insert_it = cache.insert({next_use, page});  // Insert new page into cache
                        cache_map[page] = insert_it;  // Store iterator in cache_map
                    } else {
                        out_misses++;  // Page can't be inserted because its next use is too far
                    }
                } else {
                    // Cache is not full, insert page directly
                    if (future_occurrences[page][0] == 0) {
                        cold_misses++;  // Detect cold miss
                        future_occurrences[page][0] = 1;
                    }
                    auto insert_it = cache.insert({next_use, page});  // Insert new page into cache
                    cache_map[page] = insert_it;  // Store iterator in cache_map
                }
            } else {
                // Page hit, update next use in the cache
                auto old_it = cache_map[page];  // Retrieve the iterator for the current page
                cache.erase(old_it);  // Erase the old entry using the iterator
                auto insert_it = cache.insert({next_use, page});  // Re-insert with updated next_use
                // cout << page << " : " << insert_it->second << endl;
                cache_map[page] = insert_it;  // Update the iterator in cache_map
            }
        }
    }

    // Print statistics
    cout << "Total misses: " << cache_misses << endl;
    cout << "Total out misses: " << out_misses << endl;
    cout << "Total cold misses: " << cold_misses << endl;
    cout << "Total hits: " << sequence.size() - cache_misses << endl;
}

// Reads CSV file; returns a list of MemoryAccess
vector<MemoryAccess> read_csv(const string& filename) {
    int number_of_columns = 8;
    vector<MemoryAccess> sequence;
    ifstream file(filename);

    if (!file) {
        cerr << "Error: Could not open the file." << endl;
        return sequence;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<string> values;

        // Read and store columns in values
        for (int i = 0; i < number_of_columns; ++i) {
            if (!getline(ss, value, ',')) {
                cerr << "Warning: Insufficient columns in line: " << line << endl;
                values.clear();
                break;
            }
            values.push_back(value);
        }

        // If not enough columns, skip
        if (values.size() < (size_t)number_of_columns) {
            continue;
        }

        ll time_stamp = 0;
        ll offset = 0;
        string type = "Read";
        try {
            time_stamp = stoll(values[0]);
        } catch (const invalid_argument&) {
            cerr << "Warning: Invalid time stamp in: " << line << endl;
            continue;
        }
        try {
            offset = stoll(values[2]);
        } catch (const invalid_argument&) {
            cerr << "Warning: Invalid offset in: " << line << endl;
            continue;
        }
        type = values[4];

        sequence.push_back(MemoryAccess(time_stamp, offset, type));
    }
    file.close();
    return sequence;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];

    auto memory_accesses = read_csv(filename);
    if (memory_accesses.empty()) {
        cerr << "No data available." << endl;
        return 1;
    }

    ll min_time_stamp = memory_accesses.front().time_stamp;
    ll max_time_stamp = memory_accesses.back().time_stamp;

    cout << "Minimum timestamp: " << min_time_stamp << endl;
    cout << "Maximum timestamp: " << max_time_stamp << endl;

    ll start_time, end_time;
    cout << "Enter a start time: ";
    cin >> start_time;
    cout << "Enter an end time: ";
    cin >> end_time;

    vector<MemoryAccess> filtered_accesses;
    for (auto access : memory_accesses) {
        if (start_time <= access.time_stamp && access.time_stamp <= end_time) {
            filtered_accesses.push_back(access);
        }
    }

    cout << "Enter cache size: ";
    int cache_size;
    cin >> cache_size;
    
    if (cin.fail() || cache_size <= 0) {
        cerr << "Invalid cache size." << endl;
        return 1;
    }

    vector<long long int> sequence;
    for(auto access : filtered_accesses){
        sequence.push_back(access.offset);
    }

    if (sequence.empty()) {
        cerr << "No valid data found in the first column." << endl;
        return 1;
    }
    int piece_count = 1;

    optimal_cache_replacement_with_set(cache_size, sequence, piece_count);

    return 0;
}