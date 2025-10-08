   a. All observation (as compared to Lab 03). 
   b. What was your initial branch coverage?
   c. What changes you made to improve the branch coverage?



### a) All observation (as compared to Lab 03). 
Lab 03 used manual static test cases with printed pass/fail logs.

Lab 04 replaces static tests with assert, which:
Aborts immediately on falure pinpoints exact failng branch
( tried to add a negative rate to the car loan it freaked out but i then new the failing was that negative branch)
Is simpler and requires no extrnal framework


### b) What was your initial branch coverage?

Before adding some asertions:
CarLoan invald  and invalid time were covered
HouseMortage zero-rate branch was covered

Positive interest mortgage branch not tested


### c) What changes you made to improve the branch coverage?
   Added asertions for:

Postive interest mortgage calculation
Invalid principal for mortgage
Invalid time and negative input for CarLoan