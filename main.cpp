#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>
#include "ShamirSecretSharing.h"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.json>" << std::endl;
        return 1;
    }
    
    // Read input file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string jsonInput = buffer.str();
    inputFile.close();
    
    // Parse JSON to get n and k
    int n = 0, k = 0;
    try {
        nlohmann::json j = nlohmann::json::parse(jsonInput);
        n = j["n"].get<int>();
        k = j["k"].get<int>();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON for n and k: " << e.what() << std::endl;
        return 1;
    }
    
    // Create ShamirSecretSharing instance
    ShamirSecretSharing sss(n, k);
    
    // Parse input and initialize shares
    sss.parseInput(jsonInput);
    
    // Find the secret and identify wrong shares
    sss.findSecretAndWrongShares();
    
    // Output results
    std::cout << "Secret: " << sss.getSecret() << std::endl;
    
    std::cout << "Wrong Shares: ";
    std::set<int> wrongShares = sss.getWrongShares();
    if (wrongShares.empty()) {
        std::cout << "None";
    } else {
        bool first = true;
        for (int shareId : wrongShares) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << shareId;
            first = false;
        }
    }
    std::cout << std::endl;
    
    return 0;
}