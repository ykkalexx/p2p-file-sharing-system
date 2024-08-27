#ifndef DHT_H
#define DHT_H

#include <string>
#include <unordered_map>
#include <vector>

class DHT
{
private: 
	/*
	We use this to map the file name to the IP address of the peer that has the file.
	*/
	std::unordered_map<std::string, std::string> dht;

public:
	void addFile(const std::string& filename, const std::string& nodeAddress);
	std::string lookupFile(const std::string& filename);
	void removeFile(const std::string& filename);
	std::vector<std::pair<std::string, std::string>> getAllEntries() const;
};

#endif // DHT_H