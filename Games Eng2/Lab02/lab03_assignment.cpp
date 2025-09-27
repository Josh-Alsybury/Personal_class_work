// Lab03_Assignment.cpp
// Dr. Omer Ali
// 24 SEP 2025

/*
ASSIGNMENT: LAB 03 – House Mortgage & Car Loan Extensions)

A. Goals
   1) Build a simple user menu that offers TWO options:
        A) Calculate Car Loan (total amount / interest amount)
        B) Calculate House Mortgage (monthly payment)
   2) Implement these as class methods with correct formulas (see stubs below).
   3) Write tests (in a separate file you create, e.g., lab03_assignment_test.cpp), in the
      same style used previously (static tests, no external frameworks, as were given in lab03_calculator_test.cpp). Your tests must:
        - Check zero and negative values for both functions.
        - Check for infinite loops (if any) .
        - Check integer and double value behavior (inputs/outputs).
      Document all observations, assumptions, and edge cases in your README.md.

B. Notes
   - Keep headers minimal (no external headers/libraries).
   - You may parse rates/years/principal as integers from CLI, then convert to double
     inside the formulas (as needed).
   - For invalid inputs (e.g., P<=0, years<=0, compounding periods <=0), choose a consistent
     return policy (e.g., return a value like -1.0, or print error and/or force only valid input). It must also be documented in your Readme.MD.


C. Submission
   - Code + tests + README.md with a short write-up of results, coverage, and findings.
   - Commit after each fix/feature with clear messages.
   - Deadline, 01 October 2025, 09:00 AM.

*/

#include <iostream>

/* ============================= Boilerplate Code for this assignment =============================
You can either choose to work directly on this file, or create a new/clean copy.
   CarLoan
   -------
   Provide functions for simple/compound interest amounts (choose one or both based on your requirement).
   Typical formulas (document which one you use):
   You CAN also choose either of them, or both (if you want to do more work).

   Simple Interest (Total Amount):
     A = P * (1 + r * t)
     where:
       P = principal (loan amount)
       r = annual rate as a fraction (e.g., 7% -> 0.07)
       t = time in years

   Simple Interest (Interest Only):
     I = P * r * t

   Compound Amount (if you choose to implement compound):
     A = P * (1 + r/n)^(n*t)
     where:
       n = compounding periods per year (e.g., 12 for monthly)

   HouseMortgage
   -------------
   Standard monthly mortgage payment:
     i = (annual_rate_percent / 100.0) / 12.0
     N = years * 12
     if i == 0:
        M = P / N
     else:
        M = P * [ i * (1 + i)^N ] / [ (1 + i)^N - 1 ]

   IMPORTANT:
   - Implement with double return types (accuracy).
   

*/

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

    //// Optional: Interest-only amount I = P * r * t
    //static double simple_interest_only(long long principal_P,
    //                                   long long annual_rate_percent_r,
    //                                   long long time_years_t) {
    //    // TODO(Student): Implement or remove if not used (but keep signature for tests).
    //    (void)principal_P; (void)annual_rate_percent_r; (void)time_years_t;
    //    return -999.0; 
    //}

    //// Optional (if you choose compound):
    //// A = P * (1 + (r_percent/100.0) / n)^(n * t)
    //static double compound_total_amount(long long principal_P,
    //                                    long long annual_rate_percent_r,
    //                                    long long time_years_t,
    //                                    long long n_per_year) {
    //    // TODO(Student): Implement if required (handle n_per_year<=0 policy).
    //    (void)principal_P; (void)annual_rate_percent_r; (void)time_years_t; (void)n_per_year;
    //    return -999.0; // STUB
    //}

};

class HouseMortgage {
public:
    // Monthly payment:
    // i = (r_percent_annual/100.0)/12.0, N = years*12
    // if i == 0: M = P / N
    // else: M = P * [ i*(1+i)^N ] / [ (1+i)^N - 1 ]
    static double monthly_payment(long long principal_P,
        long long annual_rate_percent_r,
        long long years) {

        if (principal_P <= 0 || annual_rate_percent_r < 0 || years < 0) {
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


static void printMenu() {
    std::cout << "\n===== Lab 03 Assignment Menu =====\n"
              << "1) Calculate Car Loan (total/interest)\n"
              << "2) Calculate Mortgage (monthly payment)\n"
              << "q) Quit\n"
              << "Choose an option: ";
}

static bool readLL(const char* prompt, long long &out) {
    std::cout << prompt;
    std::cin >> out;
    if (!std::cin.good()) {
        std::cin.clear();
        char c;
        while (std::cin.get(c) && c != '\n') { /* discard */ }
        std::cout << "Invalid input. Integers only.\n";
        return false;
    }
    return true;
}

int main() {
    while (true) {
        printMenu();
        char choice;
        std::cin >> choice;
        if (!std::cin.good()) {
            std::cin.clear();
            char c; while (std::cin.get(c) && c != '\n') {}
            std::cout << "Try again.\n";
            continue;
        }
        if (choice == 'q' || choice == 'Q') {
            std::cout << "Goodbye!\n";
            break;
        }

        if (choice == '1') {
            // TODO(Student): Prompt for principal, rate %, years.
            // Call either simple_total_amount(...) or simple_interest_only(...),
            // print result, and loop back.
            long long P, rPercent, tYears;
            if (!readLL("Principal (int): ", P)) continue;
            if (!readLL("Annual Rate % (int): ", rPercent)) continue;
            if (!readLL("Years (int): ", tYears)) continue;

            
            double A = CarLoan::simple_total_amount(P, rPercent, tYears);
            if (A == -1.0) {
                std::cout << "Invalid input. Please try again.\n";
			}
			else {
				std::cout << "Total Amount (stub): " << A << "\n";
			}

        } else if (choice == '2') {
            // TODO(Student): Prompt for principal, rate %, years; call HouseMortgage::monthly_payment(...)
            long long P, rPercent, years;
            if (!readLL("Principal (int): ", P)) continue;
            if (!readLL("Annual Rate % (int): ", rPercent)) continue;
            if (!readLL("Years (int): ", years)) continue;

            double M = HouseMortgage::monthly_payment(P, rPercent, years);
            if (M == -1.0) {
                std::cout << "Invalid input. Please try again.\n";
            }
            else {
                std::cout << "Monthly Mortgage Payment: " << M << "\n";
            }

        } else {
            std::cout << "Invalid selection.\n";
        }
    }
    return 0;
}

/* ============================= Test Case Declarations (EMPTY) =============================

   Create a separate test file (e.g., lab03_assignment_test.cpp) modeled on your previous
   static test style. In that file, re-declare the classes OR include headers (if you break them out)
   and implement tests that:

   1) Zero and Negative Values:
      - CarLoan::simple_total_amount with r=0, t=0, P<=0, t<0, etc.
      - HouseMortgage::monthly_payment with r=0 (zero-rate branch), P<=0, years<=0.

   2) Infinite Loop Checks:
      - There should be no loops that run unbounded here, but you can still create a watchdog
        pattern by calling the function many times in a bounded loop (e.g., 100 iterations) to
        demonstrate termination.

   3) Integer vs Double Behavior:
      - Feed integer prompts (menu-style) but compare outputs using double tolerances in tests
        (expect_close style). Demonstrate truncation/rounding expectations in README.md.

   4) README.md:
      - Summarize observed edge-case behavior (invalid inputs policy, zero-rate branch, rounding).
      - State coverage achieved and which node/edge scenarios your tests covered.

   Below are ONLY declarations to guide your structure in the separate test file. Do not implement
   tests here. Keep names consistent across files if that helps you organize.

*/

// Modeled on lab03_calculator_test.cpp style (no frameworks):
// (In your test file, define g_pass/g_fail, expect_close/expect_true helpers, coverage struct, etc.)

//// ---- Declarations (to be implemented in lab03_calculator_test.cpp) ----
//void test_car_loan_zero_and_negative_values();   // TODO(Student): implement in test file
//void test_mortgage_zero_and_negative_values();   // TODO(Student)
//void test_car_loan_watchdog_no_infinite_loops(); // TODO(Student)
//void test_mortgage_watchdog_no_infinite_loops(); // TODO(Student)
//void test_integer_vs_double_behavior_car_loan(); // TODO(Student)
//void test_integer_vs_double_behavior_mortgage(); // TODO(Student)
//

