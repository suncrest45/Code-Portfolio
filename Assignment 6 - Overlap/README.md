Problem:
given a grid of up to 10^9 in each direction and a set of rectangles (up to
10^5) calculate the distribution of the number of rectangles covering each cell.

Return a map with the value at index C equal to the total area (or cell count) 
that are covered by exactly C rectangles modulo 7001 (the modulo is to avoid 
overflow).

Rectangles are defined by top-left and bottom right corners,
(assuming coordinate system is centered top-left with 
positive X going right and positive Y going down):

  |01234567   X
--+-----------> 
0 |
1 |          rectangle is provided as 3 2 6 5
2 |   C--+   where (3,2) is the top-left corner and (6,5) is bottom-right
3 |   |  |   
4 |   |  |   the area of the rectangle is 4*4 = 16
5 |   +--C                               
6 |          in general area( x1,y1,x2,y2 ) = (x2-x1+1)*(y2-y1+1)
Y V

Notes:
1x1 rectangle is defined as ( x,y,x,y ).

Rectangle 1: 1 0 8 4
Rectangle 2: 5 1 9 7
Rectangle 3: 3 2 6 5


  |0123456789   X
--+----------+--> 
0 | +------+ |
1 | |   +--++|
2 | | +-++ |||
3 | | | || |||
4 | +-+-++-+||
5 |   +-++  ||
6 |     |   ||
7 |     +---+|
8 +----------+
Y V


  |0123456789   X
--+----------+--> 
0 |0111111110|      number in each cell is the count of rectangles that cover it  
1 |0111122221|
2 |0112233221|      So: 0 -> 19, 1 -> 37, 2 -> 18, 3 -> 6
3 |0112233221|
4 |0112233221|
5 |0001122111|
6 |0000011111|
7 |0000011111|
8 +----------+
Y V

Return an std::vector<unsigned> = {19,37,18,6}


Algorithm:

Idea is to NOT process every cell or row of the grid (10^18 and 10^9 correspondingly). 
Use event driven programming approach (scan algorithm):
  
  |0123456789   X
--+----------+--> 
0 | +------+ | <--- 0111111110  rectangle starts
1 | |      | | 
2 | | +----++| <--- 0122222221  rectangle starts
3 | | |    |||
4 | +-+----+|| 
5 |   |     || <--- 0001111111  rectangle ends
6 |   |     ||
7 |   +-----+|
8 +----------+ <--- 0000000000  last rectangle ends (done)
Y V

using the diagram from above (top-down):
1)  row 0: rectangle starts.
    8 cells have count 1
    2            count 0
2) another rectangle starts, so counts will change.
    first update frequencies
    8 cells have count 1    x2  (since there were 2 rows with these counts)
    2            count 0    x2
    So frequencies: 0->4   (number of covering rectangles -> number of cells)
                    1->16

    Update counts taking new rectangle into account
    7 cells have count 2
    2            count 1
    1            count 0
3) rectangle ended (row 5)
    first update frequencies
    7 cells have count 2    x2
    2            count 1    x2
    1            count 0    x2
    So frequencies: 0->4+1*2    = 6
                    1->16+2*2   = 20
                    2->7*2      = 14
    Update counts taking new rectangle into account
    7            count 1
    3            count 0
4) LAST rectangle ended (row 8)
    first update frequencies (even though we will not use them again, can be skipped)
    7            count 1    x3  (there were 3 rows with these counts)
    3            count 0    x3
    So frequencies: 0->6    +3*3 = 15
                    1->20   +7*3 = 41
                    2->14        = 14


====================================
More examples: in0 (see in0.ps) and out0 (same in1,in2)
====================================



When performing updates of frequencies based on started, finished rectangles, you may also
use a scan like algorithm. Left as exercise - should get some extra credit.
