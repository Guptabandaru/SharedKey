#include "ShamirSecretSharing.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <regex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Share constructor
Share::Share(int id) : id(id), isValid(true) {
    mpz_init(value);
}

// Share destructor
Share::~Share() {
    mpz_clear(value);
}

// Share copy constructor
Share::Share(const Share& other) : id(other.id), isValid(other.isValid) {
    mpz_init(value);
    mpz_set(value, other.value);
}

// Share assignment operator
Share& Share::operator=(const Share& other) {
    if (this != &other) {
        id = other.id;
        isValid = other.isValid;
        mpz_set(value, other.value);
    }
    return *this;
}

// ShamirSecretSharing constructor
ShamirSecretSharing::ShamirSecretSharing(int n, int k) : n(n), k(k) {
    mpz_init(secret);
}

// ShamirSecretSharing destructor
ShamirSecretSharing::~ShamirSecretSharing() {
    mpz_clear(secret);
}

// Helper function to evaluate a mathematical expression
void ShamirSecretSharing::evaluateExpression(const std::string& expression, mpz_t result) {
    // Check if the expression is a simple number
    if (std::regex_match(expression, std::regex("^\\d+$"))) {
        mpz_set_str(result, expression.c_str(), 10);
        return;
    }
    
    // Parse sum(a, b) expression
    std::regex sumRegex("sum\\((\\d+),\\s*(\\d+)\\)");
    std::smatch sumMatch;
    if (std::regex_search(expression, sumMatch, sumRegex)) {
        mpz_t a, b;
        mpz_init(a);
        mpz_init(b);
        
        mpz_set_str(a, sumMatch[1].str().c_str(), 10);
        mpz_set_str(b, sumMatch[2].str().c_str(), 10);
        
        mpz_add(result, a, b);
        
        mpz_clear(a);
        mpz_clear(b);
        return;
    }
    
    // Parse multiply(a, b) expression
    std::regex multiplyRegex("multiply\\((\\d+),\\s*(\\d+)\\)");
    std::smatch multiplyMatch;
    if (std::regex_search(expression, multiplyMatch, multiplyRegex)) {
        mpz_t a, b;
        mpz_init(a);
        mpz_init(b);
        
        mpz_set_str(a, multiplyMatch[1].str().c_str(), 10);
        mpz_set_str(b, multiplyMatch[2].str().c_str(), 10);
        
        mpz_mul(result, a, b);
        
        mpz_clear(a);
        mpz_clear(b);
        return;
    }
    
    // Parse subtract(a, b) expression
    std::regex subtractRegex("subtract\\((\\d+),\\s*(\\d+)\\)");
    std::smatch subtractMatch;
    if (std::regex_search(expression, subtractMatch, subtractRegex)) {
        mpz_t a, b;
        mpz_init(a);
        mpz_init(b);
        
        mpz_set_str(a, subtractMatch[1].str().c_str(), 10);
        mpz_set_str(b, subtractMatch[2].str().c_str(), 10);
        
        mpz_sub(result, a, b);
        
        mpz_clear(a);
        mpz_clear(b);
        return;
    }
    
    // Parse divide(a, b) expression
    std::regex divideRegex("divide\\((\\d+),\\s*(\\d+)\\)");
    std::smatch divideMatch;
    if (std::regex_search(expression, divideMatch, divideRegex)) {
        mpz_t a, b;
        mpz_init(a);
        mpz_init(b);
        
        mpz_set_str(a, divideMatch[1].str().c_str(), 10);
        mpz_set_str(b, divideMatch[2].str().c_str(), 10);
        
        mpz_tdiv_q(result, a, b);  // Integer division
        
        mpz_clear(a);
        mpz_clear(b);
        return;
    }
    
    // If no pattern matches, set to 0
    mpz_set_ui(result, 0);
}

// Parse JSON input and initialize shares
void ShamirSecretSharing::parseInput(const std::string& jsonInput) {
    try {
        json j = json::parse(jsonInput);
        
        // Extract n and k if they are in the JSON
        if (j.contains("n")) {
            n = j["n"].get<int>();
        }
        
        if (j.contains("k")) {
            k = j["k"].get<int>();
        }
        
        // Extract shares
        if (j.contains("shares")) {
            for (const auto& shareJson : j["shares"]) {
                int id = shareJson["x"].get<int>();
                std::string valueExpr = shareJson["y"].get<std::string>();
                
                Share share(id);
                evaluateExpression(valueExpr, share.value);
                
                shares.push_back(share);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}

// Generate all combinations of k shares from n shares
std::vector<std::vector<int>> ShamirSecretSharing::generateCombinations(int n, int k) {
    std::vector<std::vector<int>> result;
    std::vector<bool> v(n);
    std::fill(v.begin(), v.begin() + k, true);
    
    do {
        std::vector<int> combination;
        for (int i = 0; i < n; ++i) {
            if (v[i]) {
                combination.push_back(i);
            }
        }
        result.push_back(combination);
    } while (std::prev_permutation(v.begin(), v.end()));
    
    return result;
}

// Lagrange interpolation to reconstruct the secret
void ShamirSecretSharing::lagrangeInterpolation(const std::vector<int>& indices) {
    mpz_t result;
    mpz_init_set_ui(result, 0);
    
    for (size_t j = 0; j < indices.size(); ++j) {
        int shareIndex = indices[j];
        mpz_t term;
        mpz_init_set(term, shares[shareIndex].value);
        
        mpz_t numerator, denominator;
        mpz_init_set_ui(numerator, 1);
        mpz_init_set_ui(denominator, 1);
        
        for (size_t m = 0; m < indices.size(); ++m) {
            if (m != j) {
                int xm = shares[indices[m]].id;
                int xj = shares[shareIndex].id;
                
                // Calculate numerator: x_m
                mpz_t num_term;
                mpz_init_set_si(num_term, 0 - xm);  // We want -x_m for P(0)
                mpz_mul(numerator, numerator, num_term);
                mpz_clear(num_term);
                
                // Calculate denominator: (x_j - x_m)
                mpz_t denom_term;
                mpz_init_set_si(denom_term, xj - xm);
                mpz_mul(denominator, denominator, denom_term);
                mpz_clear(denom_term);
            }
        }
        
        // Multiply y_j by the fraction
        mpz_t fraction;
        mpz_init(fraction);
        
        // Handle negative denominator
        if (mpz_sgn(denominator) < 0) {
            mpz_neg(numerator, numerator);
            mpz_neg(denominator, denominator);
        }
        
        // We need to compute numerator/denominator
        // Since we're working with integers, we need to ensure exact division
        // For simplicity, we'll use GMP's exact division if possible
        if (mpz_divisible_p(numerator, denominator)) {
            mpz_divexact(fraction, numerator, denominator);
        } else {
            // If not exactly divisible, use integer division (this is a simplification)
            mpz_tdiv_q(fraction, numerator, denominator);
        }
        
        mpz_mul(term, term, fraction);
        
        // Add term to result
        mpz_add(result, result, term);
        
        mpz_clear(numerator);
        mpz_clear(denominator);
        mpz_clear(fraction);
        mpz_clear(term);
    }
    
    mpz_set(secret, result);
    mpz_clear(result);
}

// Find the secret and identify wrong shares
void ShamirSecretSharing::findSecretAndWrongShares() {
    // Generate all combinations of k shares
    std::vector<std::vector<int>> combinations = generateCombinations(shares.size(), k);
    
    // Map to store frequency of each potential secret
    std::map<std::string, int> secretFrequency;
    std::map<std::string, std::vector<std::vector<int>>> secretCombinations;
    
    // For each combination, reconstruct the secret
    for (const auto& combination : combinations) {
        lagrangeInterpolation(combination);
        
        // Convert secret to string for map key
        char* secretStr = mpz_get_str(nullptr, 10, secret);
        std::string secretKey(secretStr);
        free(secretStr);
        
        // Increment frequency
        secretFrequency[secretKey]++;
        secretCombinations[secretKey].push_back(combination);
    }
    
    // Find the most frequent secret
    std::string mostFrequentSecret;
    int maxFrequency = 0;
    
    for (const auto& pair : secretFrequency) {
        if (pair.second > maxFrequency) {
            maxFrequency = pair.second;
            mostFrequentSecret = pair.first;
        }
    }
    
    // Set the most frequent secret
    mpz_set_str(secret, mostFrequentSecret.c_str(), 10);
    
    // Identify wrong shares
    std::set<int> correctShareIndices;
    for (const auto& combination : secretCombinations[mostFrequentSecret]) {
        for (int index : combination) {
            correctShareIndices.insert(index);
        }
    }
    
    // Any share not in the correct set is wrong
    for (size_t i = 0; i < shares.size(); ++i) {
        if (correctShareIndices.find(i) == correctShareIndices.end()) {
            wrongShares.insert(shares[i].id);
            shares[i].isValid = false;
        }
    }
}

// Get the reconstructed secret as a string
std::string ShamirSecretSharing::getSecret() const {
    char* secretStr = mpz_get_str(nullptr, 10, secret);
    std::string result(secretStr);
    free(secretStr);
    return result;
}

// Get the list of wrong shares
std::set<int> ShamirSecretSharing::getWrongShares() const {
    return wrongShares;
}