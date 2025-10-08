// Week4 - Lab 4: Interest & Mortgage with <cassert>
// Dr. Omer Ali
// 02 OCT 2025

/*
CHANGES & RATIONALE (PLEASE READ FIRST)
--------------------------------
1) What changed from Lab 03?
   - We removed the custom "static test"  and now rely on the
     standard C/C++ assertion facility via <cassert>. This keeps the lab minimal:
       - The financial functions are implemented in two cohesive classes:
       * CarLoan: simple interest (total amount and interest-only)
       * HouseMortgage: monthly payment (standard amortization formula)
   - Tests are written as plain `assert(...)` statements in main(). When an assertion
     fails, the program aborts and clearly shows which check failed.
     Although not ideal for production code, but serves as a good starting point for labs. 
     Again, the idea is move from (Static --> dynamic --> automation) with Complete coverage. 

2) Why use <cassert>?
   - `assert` is part of the standard library; it requires no external framework.
   - It is ideal for quick, deterministic unit checks inside a lab environment. 
   This is what we 'tried' to achieve in Lab 03 where static test cases were designed to test various inputs/branches.
   Asserts will be used to replace those static test cases, and we will see how we can increase coverage by adding more asserts.
   - A failing assert immediately pinpoints a defect without extra test scaffolding. That means that at runtime, it can simply abort the
   program after returning the error. Needless to mention that assertions are best used for debugs and testing, and must be
   removed in production code. (Typically done through defining NDEBUG macro). But, as we'll move along, we will incorporate
   dynamic and industry leading testing frameworks that helps to automate the testing process and integrates well with CI/CD pipelines.

3) How does `assert` test the code?
   - `assert(condition)` verifies that `condition` is true at runtime. You could think of adding assertions at every branch/condition level
   since its output could either be TRUE or FALSE, that means we will have complete branch coverages (provided assertions are written properly)
   - If `condition` is false, `assert` prints the expression, file, and line number,
     then terminates the program (when NDEBUG is not defined). Remember to use NDEBUG flag when code is ready for production.
   - We combine `assert` with simple "close to" checks for doubles (manual tolerance)
     to handle floating-point comparisons without introducing additional headers.

4) Edge-case policy (documented for testing):
   - For invalid inputs we return a sentinel negative value (-1.0). --> As it was asked in Lab 03. (By default the value was set to -999, but it could be anything)
    can choose a different policy (throw/print), but tests below assume (-1.0).
    But why -1.0? Because for boolean tests, you can either have TRUE (1) or FALSE (0), so -1.0 is a clear sentinel for invalid inputs.
    Also, for our real-world loan and financial application, we will NEVER receive a negative interest or loan amount.

   - All rates are in percent (e.g., 6 means 6% APR). Internals convert to fractions.

INSTRUCTIONS (BUILD / RUN / COVERAGE)
-------------------------------------
A) Build normally:
   g++ -std=c++17 -O0 -g -Wall -Wextra week4_lab04.cpp -o week4_lab04

B) Build WITH coverage instrumentation (GCC/Clang):
      g++ -std=c++17 -O0 -g --coverage -Wall -Wextra week4_lab04.cpp -o week4_lab04
      

C) Run the instrumented binary to generate coverage data:
   ./week4_lab04

D) Produce coverage report with gcov:
   # Generates week4_lab04.cpp.gcov with per-line execution counts & percentages
   gcov week4_lab04.cpp

   # Look for lines like:
   #   Lines executed:XX.XX% of YY
   # and scan annotated file week4_lab04.cpp.gcov to see which lines executed (and how many times).

E) Increasing coverage:
   - Add more `assert(...)` tests in main() to execute branches you missed:
     * invalid inputs (P<=0, years<=0, n<=0)
     * zero-rate mortgages (i==0 branch)
     * different time horizons (1 year vs 30 years)
     * more compounding or alternative inputs for car loan if you extend
   - Rebuild & rerun with coverage flags, then re-run `gcov` to see the new percentages.

NOTES
-----
- We are avoiding any other additional libraries, particularly <cmath>; instead we implement a tiny power helper (pow1p) via a loop.
- We are also going to tolerate small floating-point rounding differences using a manual EPS tolerance.
- Assertions are compiled out when NDEBUG is defined; keep assertions enabled during testing.
*/

#include <iostream>
#include <cassert>

// Simple power helper without <cmath>: computes (1 + i)^N for N >= 0
static double pow1p(double i, long long N) {
    double base = 1.0 + i;
    double acc  = 1.0;
    for (long long k = 0; k < N; ++k) {
        acc = acc * base;
    }
    return acc;
}

class CarLoan {
public:
    // Simple total: A = P * (1 + r * t), r = r_percent/100
    static double simple_total_amount(long long P, long long r_percent, long long t_years) {
        if (P <= 0 || t_years < 0) return -1.0;
        double r = static_cast<double>(r_percent) / 100.0;
        return static_cast<double>(P) * (1.0 + r * static_cast<double>(t_years));
    }

    // Simple interest only: I = P * r * t
    static double simple_interest_only(long long P, long long r_percent, long long t_years) {
        if (P <= 0 || t_years < 0) return -1.0;
        double r = static_cast<double>(r_percent) / 100.0;
        return static_cast<double>(P) * r * static_cast<double>(t_years);
    }
};

class HouseMortgage {
public:
    // Monthly payment:
    // i = (annual_rate_percent/100)/12, N = years*12
    // if i == 0 => M = P / N
    // else      => M = P * [ i*(1+i)^N ] / [ (1+i)^N - 1 ]
    static double monthly_payment(long long P, long long annual_rate_percent, long long years) {
        if (P <= 0 || years <= 0) return -1.0;
        double i = (static_cast<double>(annual_rate_percent) / 100.0) / 12.0;
        long long N = years * 12;
        if (i == 0.0) return static_cast<double>(P) / static_cast<double>(N);

        double factor = pow1p(i, N);
        double denom  = factor - 1.0;
        if (denom == 0.0) return -1.0; // safety
        return (static_cast<double>(P) * i * factor) / denom;
    }
};

int main() {
    std::cout << "Week 4 Lab 4 running...\n";

    // --- CarLoan basic assertions  ---
    {
        // Nominal: A = 10000*(1 + 0.05*3) = 11500 (exact with doubles here)
        double A = CarLoan::simple_total_amount(10000, 5, 3);
        assert(A == 11500.0);
    }
    {
        // Zero years: A = P
        double A = CarLoan::simple_total_amount(10000, 7, 0);
        assert(A == 10000.0);
    }
    {
        // Interest only: I = 10000*0.05*3 = 1500
        double I = CarLoan::simple_interest_only(10000, 5, 3);
        assert(I == 1500.0);
    }
    {
        // Invalid principal
        double A = CarLoan::simple_total_amount(0, 5, 2);
        assert(A < 0.0);
    }
    {
        // Invalid time
        double A_invalid_time = CarLoan::simple_total_amount(10000, 5, -2);
        assert(A_invalid_time < 0.0);

    }

    // --- Mortgage assertions (partial coverage) ---
    {
        // Zero-rate: M = P/N
        double M = HouseMortgage::monthly_payment(120000, 0, 10);
        assert(M == 1000.0); // exact division
    }
    {
        // Invalid years
        double M = HouseMortgage::monthly_payment(200000, 5, 0);
        assert(M < 0.0);
    }
    {
        // positive intrest morgate
        double M = HouseMortgage::monthly_payment(200000, 6, 30);
        double expected = (200000 * (0.06 / 12) * pow1p(0.06 / 12, 360)) / (pow1p(0.06 / 12, 360) - 1);
        assert(std::abs(M - expected) < 0.01);
    }
    {
        double M = HouseMortgage::monthly_payment(0, 5, 10);
        assert(M< 0.0);
    }


    // HINT: Can we add more assertions for positive rate mortgage branch?
    

    std::cout << "Initial assertions passed.\n";
    return 0;
}
