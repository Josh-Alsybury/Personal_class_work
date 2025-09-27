// Lab03- Simple Command Line based Calculator
// based on the discussions we had in the class.
// Dr. Omer Ali
// 23 SEP 2025

#include <iostream>

// A small math-ops class: one function per operation.

class MathOps {
public:
    // Adds two integers and returns the integer sum.
    static long long add(long long a, long long b) {
        // Simple integer addition
        return a + b;
    }

    // Subtracts b from a and returns the integer difference.
    static long long subtract(long long a, long long b) {
        // Simple integer subtraction
        return a - b;
    }

    // Multiplies a and b and returns the integer product.
    // NOTE: May overflow long long for very large inputs (documented limitation).
    static long long multiply(long long a, long long b) {
        // Simple integer multiplication
        return a * b;
    }

    // Divides a by b (integer division). If b==0, sets ok=false and returns 0.
    static long long divide(long long a, long long b, bool &ok) {
        if (b == 0) {
            ok = false;      // Signal invalid division
            return 0;        // Return value ignored by caller when ok==false
        }
        ok = true;
        // Integer (truncating) division is intentional per "integers only"
        return a / b;
    }

    // Returns the (integer) average given a running sum and a count n>0.
    // Caller ensures n>0. The result is floor(sum/n) by integer division.
    static long long average_from_sum(long long sum, int n) {
        // Precondition: n > 0
        return sum / n;
    }

    // Computes factorial(n) for n >= 0, using iterative multiplication.
    // For n<0, sets ok=false; for n>=0, ok=true and returns the value.
    // NOTE: Will overflow long long for moderately large n; we keep it simple.
    static long long factorial(int n, bool &ok) {
        if (n < 0) {
            ok = false;        // factorial undefined for negative integers
            return 0;
        }
        ok = true;
        long long result = 1;
        // Iterative loop (terminates after n steps)
        for (int i = 2; i <= n; ++i) {
            result *= i;       // Potential overflow is accepted by spec constraints
        }
        return result;
    }
};


static bool readInt(const char *prompt, long long &out) {
    std::cout << prompt;
    std::cin >> out;
    if (!std::cin.good()) {
        // Clear error state and discard bad input so loop can continue
        std::cin.clear();
        char trash;
        while (std::cin.get(trash) && trash != '\n') { /* discard */ }
        std::cout << "Invalid input (integers only). Try again.\n";
        return false;
    }
    return true;
}

// Read a strictly positive integer count for average.
static int readPositiveCount(const char *prompt) {
    long long n = 0;
    while (true) {
        if (!readInt(prompt, n)) continue;
        if (n <= 0) {
            std::cout << "Count must be > 0.\n";
            continue;
        }
        if (n > 1000000) {
            // Arbitrary sanity limit to avoid silly loops.
            std::cout << "Please choose a smaller count (<= 1,000,000).\n";
            continue;
        }
        return static_cast<int>(n);
    }
}

// Menu rendering helper.
static void printMenu() {
    std::cout << "\n===== Integer Calculator =====\n"
              << "1) Add (a + b)\n"
              << "2) Subtract (a - b)\n"
              << "3) Multiply (a * b)\n"
              << "4) Divide (a / b)\n"
              << "5) Average (N integers)\n"
              << "6) Factorial (n!)\n"
              << "q) Quit\n"
              << "Choose an option: ";
}

int main() {
    while (true) {
        printMenu();

        // Read a single char for choice; tolerate surrounding whitespace/newlines
        char choice;
        std::cin >> choice;
        if (!std::cin.good()) {
            // Recover from stream error
            std::cin.clear();
            char trash;
            while (std::cin.get(trash) && trash != '\n') { /* discard */ }
            std::cout << "Invalid selection. Try again.\n";
            continue;
        }

        if (choice == 'q' || choice == 'Q') {
            std::cout << "Goodbye!\n";
            break;
        }

        if (choice == '1') {
            long long a, b;
            if (!readInt("Enter a: ", a)) continue;
            if (!readInt("Enter b: ", b)) continue;
            long long r = MathOps::add(a, b);
            std::cout << "Result: " << r << "\n";
        } else if (choice == '2') {
            long long a, b;
            if (!readInt("Enter a: ", a)) continue;
            if (!readInt("Enter b: ", b)) continue;
            long long r = MathOps::subtract(a, b);
            std::cout << "Result: " << r << "\n";
        } else if (choice == '3') {
            long long a, b;
            if (!readInt("Enter a: ", a)) continue;
            if (!readInt("Enter b: ", b)) continue;
            long long r = MathOps::multiply(a, b);
            std::cout << "Result: " << r << "\n";
        } else if (choice == '4') {
            long long a, b;
            if (!readInt("Enter a (numerator): ", a)) continue;
            if (!readInt("Enter b (denominator): ", b)) continue;
            bool ok = true;
            long long r = MathOps::divide(a, b, ok);
            if (!ok) {
                std::cout << "Error: divide by zero is not allowed.\n";
            } else {
                std::cout << "Result (integer division): " << r << "\n";
            }
        } else if (choice == '5') {
            int n = readPositiveCount("How many integers to average? ");
            long long sum = 0;
            for (int i = 0; i < n; ++i) {
                long long v;
                // Read each integer value; if invalid, re-ask this one
                while (!readInt("Enter value: ", v)) { /* re-ask */ }
                sum += v; // note: may overflow long long for extreme totals
            }
            long long r = MathOps::average_from_sum(sum, n);
            std::cout << "Average (integer): " << r << "\n";
        } else if (choice == '6') {
            long long nv;
            if (!readInt("Enter n (>= 0): ", nv)) continue;
            if (nv < -2000000000LL || nv > 2000000000LL) {
                std::cout << "n out of reasonable range.\n";
                continue;
            }
            bool ok = true;
            long long r = MathOps::factorial(static_cast<int>(nv), ok);
            if (!ok) {
                std::cout << "Error: factorial is undefined for negative integers.\n";
            } else {
                std::cout << nv << "! = " << r << " (may overflow for large n)\n";
            }
        } else {
            std::cout << "Invalid selection. Try again.\n";
        }
    }
    return 0;
}
