#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

struct MemoryAccess {
    ll time_stamp;
    ll index;
    enum AccessType {
        Read,
        Write
    } type;

   MemoryAccess(ll time_stamp, ll index, string _type) : time_stamp(time_stamp), index(index) {
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

bool outputJSON;
bool batch;

// Function to simulate the optimal cache replacement algorithm using std::set
void optimal_cache_replacement_with_set(int cache_size, const vector<long long int> &sequence, int piece_count) {
    multiset<pair<int, long long int>, CompareNextUse> cache;  // Cache uses a custom comparator for decreasing order
    unordered_map<long long int, multiset<pair<int, long long int>>::iterator> cache_map;  // Map storing iterators to set elements
    int cache_misses = 0, out_misses = 0, cold_misses = 0;

    for (int j = 0; j < piece_count; j++) {
        // Preprocess future occurrences of pages in the sequence
        unordered_map<long long int, vector<int>> future_occurrences = preprocess_future_occurrences(sequence, piece_count, j);

        // Simulate page requests
        for (int i = 0; i < (int)sequence.size() / piece_count; ++i) {
            long long int page = sequence[((int)sequence.size() / piece_count) * j + i];
            future_occurrences[page].pop_back();

            int next_use = future_occurrences[page].size() > 1 ? future_occurrences[page].back() : INT_MAX;

            if (cache_map.find(page) == cache_map.end()) {
                // Cache miss
                cache_misses++;

                if ((int)cache.size() == cache_size) {
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

    if(batch){
        cerr << fixed << setprecision(1);
        cerr << (100.0 * (sequence.size() - cache_misses ) / (int)sequence.size()) << endl;
        return;
    }

    // Print statistics
    cout << "Total misses: " << cache_misses << endl;
    cout << "Total out misses: " << out_misses << endl;
    cout << "Total cold misses: " << cold_misses << endl;
    cout << "Total hits: " << sequence.size() - cache_misses << endl;
    cout << endl;
    
    if ((int)sequence.size() > 0) {
        cout << fixed << setprecision(1);
        cout << "Cache Hit Rate: " << (100.0 * (sequence.size() - cache_misses ) / (int)sequence.size()) << "%" << endl;
        cout << "Cache Miss Rate: " << (100.0 * cache_misses / (int)sequence.size()) << "%" << endl;
    }

    if(outputJSON){
        std::ofstream outFile("output.json");

        // Output JSON formatted data to file
        if (outFile.is_open()) {
            outFile << std::fixed << std::setprecision(1);
            outFile << "{" << std::endl;
            outFile << "  \"TotalRequests\": " << sequence.size() << "," << std::endl;
            outFile << "  \"TotalCacheHit\": " << sequence.size() - cache_misses << "," << std::endl;
            outFile << "  \"TotalCacheMiss\": " << cache_misses << "," << std::endl;
            outFile << "  \"CacheHitRate\": " << (100.0 * (sequence.size() - cache_misses ) / (int)sequence.size()) << "," << std::endl;
            outFile << "  \"CacheMissRate\": " << (100.0 * cache_misses / (int)sequence.size()) << std::endl;
            outFile << "}" << std::endl;
            outFile.close();
            cout << "Data written to output.json" << std::endl;
        } else {
            cerr << "Unable to open file for writing." << std::endl;
        }
    }
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
        ll index = 0;
        string type = "Read";
        try {
            time_stamp = stoll(values[0]);
        } catch (const invalid_argument&) {
            cerr << "Warning: Invalid time stamp in: " << line << endl;
            continue;
        }
        try {
            index = stoll(values[2]);
        } catch (const invalid_argument&) {
            cerr << "Warning: Invalid index in: " << line << endl;
            continue;
        }
        type = values[4];

        sequence.push_back(MemoryAccess(time_stamp, index, type));
    }
    file.close();
    return sequence;
}

void run_batch(const vector<MemoryAccess> &memory_accesses, ll min_time_stamp){
    int cache_size;
    ll start_time, end_time;
    while (cin >> cache_size >> start_time >> end_time) {
        start_time += min_time_stamp;
        end_time += min_time_stamp;
        vector<MemoryAccess> filtered_accesses;
        for (auto access : memory_accesses) {
            if (start_time <= access.time_stamp && access.time_stamp <= end_time) {
                filtered_accesses.push_back(access);
            }
        }
        vector<long long int> sequence;
        for(auto access : filtered_accesses){
            sequence.push_back(access.index);
        }

        if (sequence.empty()) {
            cerr << "No valid data found in the first column." << endl;
            return;
        }
        int piece_count = 1;

        optimal_cache_replacement_with_set(cache_size, sequence, piece_count);
    }
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

    ll start_time = -1;
    ll end_time = -1;
    int cache_size = -1;

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--output-json"){
            outputJSON = 1;
        }
        if(arg == "--batch"){
            batch = 1;
        }

        if (arg.find("--start-time=") == 0) {
            if(arg == "--start-time=min") start_time = min_time_stamp;
            else start_time = stoll(arg.substr(13)) + min_time_stamp;
        } else if (arg.find("--end-time=") == 0) {
            if(arg == "--end-time=max") end_time = max_time_stamp;
            else end_time = stoll(arg.substr(11)) + min_time_stamp;
        } else if (arg.find("--cache-size=") == 0) {
            cache_size = stoi(arg.substr(13));
        }
    }

    if(batch){
        run_batch(memory_accesses, min_time_stamp);
        return 0;
    }

    if(start_time == -1 || end_time == -1){
        cout << "Minimum timestamp: " << 0 << endl;
        cout << "Maximum timestamp: " << max_time_stamp - min_time_stamp << endl;
    }

    if (start_time == -1) {
        cout << "Enter a start time: ";
        cin >> start_time;
        if (cin.fail()) {
            cerr << "Invalid start time." << endl;
            return 1;
        }
        start_time += min_time_stamp;
    }

    if (end_time == -1) {
        cout << "Enter an end time: ";
        cin >> end_time;
        if (cin.fail()) {
            cerr << "Invalid end time." << endl;
            return 1;
        }
        end_time += min_time_stamp;
    }

    if (cache_size == -1) {
        cout << "Enter cache size: ";
        cin >> cache_size;
        if (cin.fail() || cache_size <= 0) {
            cerr << "Invalid cache size." << endl;
            return 1;
        }
    }
    
    vector<MemoryAccess> filtered_accesses;
    for (auto access : memory_accesses) {
        if (start_time <= access.time_stamp && access.time_stamp <= end_time) {
            filtered_accesses.push_back(access);
        }
    }


    vector<long long int> sequence;
    for(auto access : filtered_accesses){
        sequence.push_back(access.index);
    }

    if (sequence.empty()) {
        cerr << "No valid data found in the first column." << endl;
        return 1;
    }
    int piece_count = 1;

    optimal_cache_replacement_with_set(cache_size, sequence, piece_count);

    return 0;
}