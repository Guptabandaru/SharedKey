#ifndef SHAMIR_SECRET_SHARING_H
#define SHAMIR_SECRET_SHARING_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <gmp.h>

// Structure to represent a share
struct Share {
    int id;             // x-coordinate
    mpz_t value;        // y-coordinate (using GMP for arbitrary precision)
    bool isValid;       // Flag to mark if the share is valid

    // Constructor
    Share(int id);
    
    // Destructor
    ~Share();
    
    // Copy constructor
    Share(const Share& other);
    
    // Assignment operator
    Share& operator=(const Share& other);
};

// Class for Shamir's Secret Sharing algorithm
class ShamirSecretSharing {
private:
    int n;                      // Total number of shares
    int k;                      // Minimum shares required to reconstruct the secret
    std::vector<Share> shares;  // Vector of all shares
    mpz_t secret;               // The reconstructed secret
    std::set<int> wrongShares;  // Set of indices of wrong shares

    // Helper function to evaluate a mathematical expression
    void evaluateExpression(const std::string& expression, mpz_t result);
    
    // Lagrange interpolation to reconstruct the secret
    void lagrangeInterpolation(const std::vector<int>& indices);
    
    // Generate all combinations of k shares from n shares
    std::vector<std::vector<int>> generateCombinations(int n, int k);

public:
    // Constructor
    ShamirSecretSharing(int n, int k);
    
    // Destructor
    ~ShamirSecretSharing();
    
    // Parse JSON input and initialize shares
    void parseInput(const std::string& jsonInput);
    
    // Find the secret and identify wrong shares
    void findSecretAndWrongShares();
    
    // Get the reconstructed secret as a string
    std::string getSecret() const;
    
    // Get the list of wrong shares
    std::set<int> getWrongShares() const;
};

#endif // SHAMIR_SECRET_SHARING_H