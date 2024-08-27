#include "DHT.h"

void DHT::addFile(const std::string& filename, const std::string& nodeAddress) {
	dht[filename] = nodeAddress;
}

std::string DHT::lookupFile(const std::string& filename) {
	return dht.count(filename) ? dht[filename] : "";
}

void DHT::removeFile(const std::string& filename) {
	dht.erase(filename);
}

std::vector<std::pair<std::string, std::string>> DHT::getAllEntries() const {
    std::vector<std::pair<std::string, std::string>> entries;
    for (const auto& entry : dht) {
        entries.push_back(entry);
    }

    return entries;
}