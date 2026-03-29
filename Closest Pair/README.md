Implement closest pair algorithm using divide and conquer.


1) your code should take care of special case when x-ordering cannot split
the given set into 2 non-empty sets, like in
(1,0) (1,1) (1,2)
all 3 points are on one vertical line, so x-ordering (by vertical line) 
cannot split the set. Switch to y-ordering (by horizontal line).
Notice that when neither of the 2 ordering can split, you have a special case:
all points in the set are equal - thus distance is 0.
