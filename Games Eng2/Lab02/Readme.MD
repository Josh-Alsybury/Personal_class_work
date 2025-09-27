   a). Can you think of other ways to test loops?
   b). What additional test cases you chose for improving test coverage and why?


### a) Other ways to test loops
Besides running a bounded loop, loops can be tested by:
- Verify loop results against other formulas.
- Test smallest and largest valid `n` to ensure termination at edges.
- Insert counters or timers to detect unusually long execution.
- Prove the loop has a decrementing condition that must eventually terminate.

### b) Additional test cases for coverage
To achieve full node/edge coverage, we added:
- Negative rate, or years → exercise error branch.
- Special branch in mortgage code.
- Special branch in car loan code.
- Ensures truncation is tested.
- Demonstrates that repeated calls to loops terminate.

----------------------Output--Log----------------------

Coverage Criteria: Node/Edge-based (approx.)
 * CarLoan: tested invalid inputs, zero-rate branch, normal case.
 * Mortgage: tested invalid inputs, zero-interest branch, normal case.
 * Watchdog: repeated calls to prove no infinite loops.

Key Edge Cases:
 - Negative or zero principal, rate, or time => error.
 - Zero-interest mortgage => P/N monthly payment.
 - Normal case verified against known financial formulas.

Interpretation:
 - PASS lines mean expected behavior confirmed.
 - FAIL lines indicate mismatches that need investigation.

==== SUMMARY ====
Passed: 11
Failed: 0

==== ASSIGNMENT TEST COVERAGE REPORT ====
Covered 7 of 7 targeted nodes/edges => 100%
Detail:
 - CarLoan (invalid / zero-rate / normal): COVERED
 - Mortgage (invalid / zero-interest / normal): COVERED
 - Watchdog loop check: COVERED
 
-------------------------------------------------------