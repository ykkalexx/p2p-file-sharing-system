#include "DHT.h"

void DHT::addFile(const std::string& filename, const std::string& nodeAddress, const std::string& fileHash) {
    dht[filename] = { nodeAddress, fileHash };
}

std::string DHT::lookupFile(const std::string& filename) {
    return dht.count(filename) ? dht[filename].first : "";
}

std::string DHT::lookupFileHash(const std::string& filename) {
    return dht.count(filename) ? dht[filename].second : "";
}

void DHT::removeFile(const std::string& filename) {
    dht.erase(filename);
}

std::vector<std::pair<std::string, std::pair<std::string, std::string>>> DHT::getAllEntries() const {
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> entries;
    for (const auto& entry : dht) {
        entries.push_back(entry);
    }
    return entries;
}