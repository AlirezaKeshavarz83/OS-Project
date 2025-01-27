// main.cpp

#include <bits/stdc++.h>
using namespace std;

#include "cache_manager.h"
#include "lru.h"
#include "nhit.h"
#include "arc.h"
#include "larc.h"

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

bool outputJSON;

// Process the memory accesses through a given cache
void processMemoryAccesses(CacheManager& cacheManager, const vector<MemoryAccess>& accesses) {
    int readHitCount = 0;
    int writeHitCount = 0;
    int readMissCount = 0;
    int writeMissCount = 0;

    for (const auto& access : accesses) {
        int result = cacheManager.access(access.index);
        // result == 1 -> miss, 0 -> hit
        if (result) {
            if (access.type == MemoryAccess::Write) writeMissCount++;
            else readMissCount++;
        } else {
            if (access.type == MemoryAccess::Write) writeHitCount++;
            else readHitCount++;
        }
    }

    int totalReadRequests = readHitCount + readMissCount;
    int totalWriteRequests = writeHitCount + writeMissCount;
    int totalRequests = totalReadRequests + totalWriteRequests;
    int totalCacheHit = readHitCount + writeHitCount;
    int totalCacheMiss = readMissCount + writeMissCount;

    cout << "Total Read Hit: " << readHitCount << endl;
    cout << "Total Read Miss: " << readMissCount << endl;
    cout << "Total Write Hit: " << writeHitCount << endl;
    cout << "Total Write Miss: " << writeMissCount << endl;
    cout << "Total Write Requests: " << totalWriteRequests << endl;
    cout << "Total Read Requests: " << totalReadRequests << endl << endl;

    cout << "Total Requests: " << totalRequests << endl;
    cout << "Total Cache Hit: " << totalCacheHit << endl;
    cout << "Total Cache Miss: " << totalCacheMiss << endl << endl;

    if (totalRequests > 0) {
        cout << fixed << setprecision(1);
        cout << "Cache Hit Rate: " << (100.0 * totalCacheHit / totalRequests) << "%" << endl;
        cout << "Cache Miss Rate: " << (100.0 * totalCacheMiss / totalRequests) << "%" << endl;
    }

    if(outputJSON){
        std::ofstream outFile("output.json");

        // Output JSON formatted data to file
        if (outFile.is_open()) {
            outFile << std::fixed << std::setprecision(1);
            outFile << "{" << std::endl;
            outFile << "  \"TotalReadHit\": " << readHitCount << "," << std::endl;
            outFile << "  \"TotalReadMiss\": " << readMissCount << "," << std::endl;
            outFile << "  \"TotalWriteHit\": " << writeHitCount << "," << std::endl;
            outFile << "  \"TotalWriteMiss\": " << writeMissCount << "," << std::endl;
            outFile << "  \"TotalWriteRequests\": " << totalWriteRequests << "," << std::endl;
            outFile << "  \"TotalReadRequests\": " << totalReadRequests << "," << std::endl;
            outFile << "  \"TotalRequests\": " << totalRequests << "," << std::endl;
            outFile << "  \"TotalCacheHit\": " << totalCacheHit << "," << std::endl;
            outFile << "  \"TotalCacheMiss\": " << totalCacheMiss << "," << std::endl;
            outFile << "  \"CacheHitRate\": " << (100.0 * totalCacheHit / totalRequests) << "," << std::endl;
            outFile << "  \"CacheMissRate\": " << (100.0 * totalCacheMiss / totalRequests) << std::endl;
            outFile << "}" << std::endl;
            outFile.close();
            cout << "Data written to output.json" << std::endl;
        } else {
            cerr << "Unable to open file for writing." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <algorithm> <filename>" << endl;
        cerr << "Possible algorithms: [lru | nhit | arc | larc]" << endl;
        return 1;
    }

    string algoType = argv[1];
    string filename = argv[2];

    if(algoType != "lru" && algoType != "nhit" && algoType != "arc" && algoType != "larc"){
        cerr << "Invalid Algorithm." << endl;
        return 0;
    }

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
    int insertion_threshold = -1;

    for (int i = 3; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--output-json"){
            outputJSON = 1;
        }

        if (arg.find("--start-time=") == 0) {
            if(arg == "--start-time=min") start_time = min_time_stamp;
            else start_time = stoll(arg.substr(13)) + min_time_stamp;
        } else if (arg.find("--end-time=") == 0) {
            if(arg == "--end-time=max") end_time = max_time_stamp;
            else end_time = stoll(arg.substr(11)) + min_time_stamp;
        } else if (arg.find("--cache-size=") == 0) {
            cache_size = stoi(arg.substr(13));
        } else if (arg.find("--threshold=") == 0) {
            insertion_threshold = stoi(arg.substr(12));
        }
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

    if(algoType == "lru"){
        LRU_Implementation cache(cache_size);
        processMemoryAccesses(cache, filtered_accesses);
    } else if(algoType == "nhit"){
        if(insertion_threshold == -1){
            cout << "Enter Insertion Threshold: ";
            cin >> insertion_threshold;
            if (cin.fail()) {
                cerr << "Invalid Insertion Threshold." << endl;
                return 1;
            }
        }
        N_Hit_Implementation cache(cache_size, insertion_threshold);
        processMemoryAccesses(cache, filtered_accesses);
    } else if(algoType == "arc"){
        ARC_Implementation cache(cache_size);
        processMemoryAccesses(cache, filtered_accesses);
    } else if(algoType == "larc"){
        LARC_Implementation cache(cache_size);
        processMemoryAccesses(cache, filtered_accesses);
    }
    

    return 0;
}