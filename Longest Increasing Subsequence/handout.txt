Implement longest increasing subsequence (LIS) algorithm.

LIS problem: given a sequence of objects (integers for this lab) find the 
longest subsequence elements of which are in strictly increasing order
(strictly means that next element has to be greater than the current).

Example of strictly increasing sequence:
1 3 4 8 20
the next one is not strictly increasing:
1 3 3 7 9
because of the "3 3"

Given a sequence 
4 1 3 1 5 0
there are the longest strictly increasing subsequence:
1 3 5 (values)
    or
1 2 4 (indices)

Your algorithm should be dynamic programming.

Hint: 
 1) table for this problem is 1-dimensional.
 2) the "main logic" contains a loop that tries to figure out if the current 
    element may be used to "extend" one of the previous subsequences (by adding
    the element in the end of it - remember to make sure "longest increasing"
    is maintained).
