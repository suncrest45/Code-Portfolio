Implement convex hull brute force:
1) hullBruteForce: for each pair points determine whether all other points are one side of the
line formed by the pair of points. If it does - add the points (or rather 
indices to the hull). Since hull is represented by a std::set, you do not have 
to worry about duplicates.

2) hullBruteForce2: find the first point that is in the hull (smalles or biggest x or y coordinate),
then find the next vertex of the hull in counter-clockwise order by considering all lines through the
previous vertex and requiring that there are no points to the right of it.
