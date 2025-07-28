# Shamir's Secret Sharing Implementation

This project implements Shamir's Secret Sharing algorithm in C++. The algorithm allows a secret to be divided into multiple "shares" such that:

1. Any k or more shares can reconstruct the secret
2. Fewer than k shares cannot reconstruct the secret
3. The implementation can identify and discard corrupted shares

## Features

- Handles arbitrary-precision integers using GMP library
- Parses and evaluates mathematical expressions in share values
- Identifies corrupted shares by finding the most frequent secret
- Supports large numbers (20+ digits)

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- GMP (GNU Multiple Precision Arithmetic Library)
- nlohmann/json (automatically fetched by CMake)

## Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./shamir_secret_sharing <input_file.json>
```

## Input Format

The input file should be a JSON file with the following structure:

```json
{
    "n": 4,
    "k": 3,
    "shares": [
        {
            "x": 1,
            "y": "1507"
        },
        {
            "x": 2,
            "y": "1518"
        },
        {
            "x": 3,
            "y": "1533"
        },
        {
            "x": 4,
            "y": "9999"
        }
    ]
}
```

The `y` values can be either direct numbers or simple mathematical expressions like:
- `sum(a, b)` - Adds a and b
- `multiply(a, b)` - Multiplies a and b
- `subtract(a, b)` - Subtracts b from a
- `divide(a, b)` - Divides a by b (integer division)

## Output

The program outputs:
1. The reconstructed secret
2. The list of wrong shares (if any)

## Algorithm

The implementation uses Lagrange interpolation to reconstruct the polynomial from k points. The secret is the value of the polynomial at x=0.

To identify corrupted shares, the program:
1. Tries all combinations of k shares
2. Reconstructs the secret for each combination
3. Finds the most frequent result, which is the true secret
4. Identifies shares that are part of combinations yielding incorrect secrets as corrupted