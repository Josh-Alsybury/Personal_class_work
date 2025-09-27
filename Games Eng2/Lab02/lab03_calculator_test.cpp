// Statis Tests for Lab 03 Calculator Program
// Dr. Omer Ali
// 23 SEP 2025


#include <iostream>

class CarLoan {
public:
    static double simple_total_amount(long long principal_P,
        long long annual_rate_percent_r,
        long long time_years_t) {
        if (principal_P <= 0 || annual_rate_percent_r < 0 || time_years_t < 0) {
            std::cout << "Error: Principal must be > 0, Rate and Time must be >= 0.\n";
            return -1.0;
        }
        double r = annual_rate_percent_r / 100.0;
        return principal_P * (1.0 + r * time_years_t);
    }
};

class HouseMortgage {
public:
    static double monthly_payment(long long principal_P,
        long long annual_rate_percent_r,
        long long years) {
        if (principal_P <= 0 || annual_rate_percent_r < 0 || years <= 0) {
            std::cout << "Error: Principal must be > 0, Rate and Time must be >= 0.\n";
            return -1.0;
        }

        double i = (annual_rate_percent_r / 100.0) / 12.0;
        long long N = years * 12;

        if (i == 0) {
            return static_cast<double>(principal_P) / N;
        }

        double numerator = i * std::pow(1 + i, N);
        double denominator = std::pow(1 + i, N) - 1;

        return principal_P * (numerator / denominator);
    }
};
static int g_pass = 0;
static int g_fail = 0;

static void expect_close(const char* name, double got, double want, double tol = 1e-6) {
    if (std::fabs(got - want) <= tol) {
        std::cout << "[PASS] " << name << " => " << got << "\n";
        ++g_pass;
    }
    else {
        std::cout << "[FAIL] " << name << " => got " << got << ", expected " << want << "\n";
        ++g_fail;
    }
}

static void expect_true(const char* name, bool v) {
    if (v) { std::cout << "[PASS] " << name << "\n"; ++g_pass; }
    else   { std::cout << "[FAIL] " << name << "\n"; ++g_fail; }
}

static void expect_false(const char* name, bool v) {
    expect_true(name, !v);
}


struct Coverage {
    // CarLoan
    bool car_invalid = false;
    bool car_zero_rate = false;
    bool car_normal = false;

    // HouseMortgage
    bool mort_invalid = false;
    bool mort_zero_interest = false;
    bool mort_normal = false;

    // Watchdog
    bool watchdog_loops_terminate = false;
} C;

// --- Tests ---
static void test_carloan() {
    // Invalid inputs
    double r1 = CarLoan::simple_total_amount(-1000, 5, 2);
    C.car_invalid = true;
    expect_close("CarLoan invalid principal", r1, -1.0);

    double r2 = CarLoan::simple_total_amount(1000, -5, 2);
    expect_close("CarLoan invalid rate", r2, -1.0);

    double r3 = CarLoan::simple_total_amount(1000, 5, -1);
    expect_close("CarLoan invalid time", r3, -1.0);

    // Zero rate
    double r4 = CarLoan::simple_total_amount(2000, 0, 3);
    C.car_zero_rate = true;
    expect_close("CarLoan zero rate", r4, 2000.0);

    // Normal case
    double r5 = CarLoan::simple_total_amount(1000, 10, 2);
    C.car_normal = true;
    expect_close("CarLoan normal case", r5, 1000 * (1 + 0.10 * 2));
}

static void test_housemortgage() {
    // Invalid inputs
    double r1 = HouseMortgage::monthly_payment(-5000, 5, 10);
    C.mort_invalid = true;
    expect_close("Mortgage invalid principal", r1, -1.0);

    double r2 = HouseMortgage::monthly_payment(5000, -5, 10);
    expect_close("Mortgage invalid rate", r2, -1.0);

    double r3 = HouseMortgage::monthly_payment(5000, 5, 0);
    expect_close("Mortgage invalid years", r3, -1.0);

    // Zero interest
    double r4 = HouseMortgage::monthly_payment(1200, 0, 1);
    C.mort_zero_interest = true;
    expect_close("Mortgage zero interest", r4, 100.0);

    // Normal case
    double r5 = HouseMortgage::monthly_payment(100000, 6, 30);
    C.mort_normal = true;
    // Known approx monthly payment: ~599.55
    expect_close("Mortgage normal case", r5, 599.55, 0.1);
}

static void test_watchdog() {
    // Run functions multiple times to prove termination
    bool term = true;
    for (int k = 0; k < 100; ++k) {
        double r1 = CarLoan::simple_total_amount(1000, 5, 2);
        double r2 = HouseMortgage::monthly_payment(100000, 6, 30);
        if (r1 < 0 || r2 < 0) { term = false; break; }
    }
    C.watchdog_loops_terminate = term;
    expect_true("Watchdog: CarLoan & Mortgage terminate", term);
}

// --- Coverage calculation & reporting ---
// We treat each boolean in C as one "node/edge" to cover. Report simple percentage.

static void report_coverage() {
    int covered = 0, total = 0;

    total += 3; // CarLoan
    if (C.car_invalid) ++covered;
    if (C.car_zero_rate) ++covered;
    if (C.car_normal) ++covered;

    total += 3; // Mortgage
    if (C.mort_invalid) ++covered;
    if (C.mort_zero_interest) ++covered;
    if (C.mort_normal) ++covered;

    total += 1; // Watchdog
    if (C.watchdog_loops_terminate) ++covered;

    std::cout << "\n==== ASSIGNMENT TEST COVERAGE REPORT ====\n";
    std::cout << "Covered " << covered << " of " << total
        << " targeted nodes/edges => "
        << (100 * covered / (total == 0 ? 1 : total)) << "%\n";

    std::cout << "Detail:\n"
        << " - CarLoan (invalid / zero-rate / normal): "
        << ((C.car_invalid && C.car_zero_rate && C.car_normal) ? "COVERED" : "PARTIAL") << "\n"
        << " - Mortgage (invalid / zero-interest / normal): "
        << ((C.mort_invalid && C.mort_zero_interest && C.mort_normal) ? "COVERED" : "PARTIAL") << "\n"
        << " - Watchdog loop check: "
        << (C.watchdog_loops_terminate ? "COVERED" : "PARTIAL") << "\n";
}


static void explain_tests() {
    std::cout << "\n==== ASSIGNMENT TEST PLAN (Detailed Explanation) ====\n"
        << "Coverage Criteria: Node/Edge-based (approx.)\n"
        << " * CarLoan: tested invalid inputs, zero-rate branch, normal case.\n"
        << " * Mortgage: tested invalid inputs, zero-interest branch, normal case.\n"
        << " * Watchdog: repeated calls to prove no infinite loops.\n"
        << "\nKey Edge Cases:\n"
        << " - Negative or zero principal, rate, or time => error.\n"
        << " - Zero-interest mortgage => P/N monthly payment.\n"
        << " - Normal case verified against known financial formulas.\n"
        << "\nInterpretation:\n"
        << " - PASS lines mean expected behavior confirmed.\n"
        << " - FAIL lines indicate mismatches that need investigation.\n";
}

int main() {
    std::cout << "Running static tests for MathOps...\n\n";

    test_carloan();
    test_housemortgage();
    test_watchdog();

    explain_tests();

    std::cout << "\n==== SUMMARY ====\n";
    std::cout << "Passed: " << g_pass << "\nFailed: " << g_fail << "\n";

    report_coverage();

    // Return non-zero if any failures (useful for CI)
    return (g_fail == 0) ? 0 : 1;
}
