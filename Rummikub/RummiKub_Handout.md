# Implement RummiKub Solver: Handout Rewrite
- Note: Below is a rewrite of the original handout for the CS280 RummiKub assignment. The content of this handout should be identical to that of the original, but bits have been rephrased, reorganized and modified from the original. I have not completed the assignment as of writing this, so if pieces of this don't make sense, I have included the original handout verbatim at the bottom of this page. Let me know if you find any issues. Happy coding o7

## What:
### Input
- Your prgram will recieve a hand (std::vector) of Tiles:
  - A hand is a collection of Tiles (see definition of Tile).
    - Tile Atributes:
      - denomination: 
        - 0,1,2,...,11,12
      - color: 
        - R,G,B,Y (red, green, blue, yellow) see "enum Color"




### Output
- Your program should determine if it is possible to play ALL of the tiles in your hand using legal runs and groups.
  
- Group:
  1. A sequence of 3 or 4 tiles
  2. Same denominations
  3. No tiles of the same color
   - Examples:
       - Legal
           - 3-R 3-B 3-G
           - 12-R 12-B 12-G 12-Y
       - Illegal
           - 3-R 3-B too short
           - 3-R 3-B 4-G not the same denomination
           - 12-R 12-B 12-G 12-G - same color used twice or more times

- Run (classical):  
  1. A sequence of 3 or more tiles
  1. All tiles have the same color
  1. Denomination are consecutive
  - Examples:
    - Legal 
      - 2-R 3-R 4-R
      - 4-B 5-B 6-B 7-B 8-B
    - Illegal
      - 2-R 3-R - too short
      - 4-B 5-B 6-B 7-R - not the same color

- Run (generalized):
    - A run is a sequence that is made of several classical runs of the same color.
    - Examples:
      - Legal:
        - 2-R 3-R 4-R 7-R 8-R 9-R (two classical runs)
      - Illegal
        - 2-R 3-R 4-R 7-G 8-G 9-G (not same color)


## How:

Algorithm:
brute-force recursion:  
for each next tile T, you have the following options:
1. Add T to an existing run that is the same color as T, if T's denomination is not yet in the run
1. Add T to an existing group that is the same denomination as T if T's color is not yet in the group
1. Create a new run with T
1. Create a new group with T

Your solution should look something like this:
```cpp
solve( ... ) 
{
    // base case
    // out of tiles - check if runs and groups are legal
    // return true if yes and exit

    Action actions[] = GetOptions();

    for ( action : actions ) {
        if action is legal execute it
        solve( ... ); // recursive call
        if solved - return true;
        undo the action
    }

    return false; // could not find place for current tile - backtrack
}
```

A Note About Branches:
- As the number of runs and groups grows, so does the number of possible actions to take with your next tile. For example: Above, with 3 runs and 4 groups you could have up to 3+4+1+1=9 possible actions. These actions create branches in your tree. The amount of branching (the branching factor) grows with the depth of the tree, and for this assignment, the worst case scenario is 2 options for the first tile, 3 for the next, etc, so the maximum number of leaf nodes in your final tree is 2 \* 3\* ... \* n = n!, where n is the number of tiles.

Expected time:  
- Since test3 is randomized, I don't know the amount of time your code will take to run, so I cannot set a specific timeout to ensure your implementation is correct. So instead of setting a specific timeout I will do the following:
  1. I will run test3 several times giving a lot of time - this will be a test of correctness.
  2. Then I will run test4 (which is test3 1000 times) and give it 250s (so that average time per test3 test is 0.25s)

- Using CygWin to run 1000 tests and time them:  
`time for((i=0;i<1000;++i)); do echo $i; ./gcc0.exe 3 &> /dev/null; done`

***

# Original Handout:
```
Implement RummiKub solver:

You get a hand: a collection of tiles (see definition of Tile).
You have to answer a question whether it is possible to play ALL
of them using legal runs and groups.

Tile:
denomination 0,1,2,...,11,12
color        R,G,B,Y (red, green, blue, yellow) see "enum Color"

Group: a sequence
1) of 3 or 4 tiles
2) same denominations
3) no tiles of the same color
Legal
3-R 3-B 3-G
12-R 12-B 12-G 12-Y
Illegal
3-R 3-B too short
3-R 3-B 4-G not the same denomination
12-R 12-B 12-G 12-G - same color used twice or more times

Run (classical): a sequence 
1) of 3 or more tiles
2) all tiles have the same color
3) denomination are consecutive
Examples:
Legal 
2-R 3-R 4-R
4-B 5-B 6-B 7-B 8-B
Illegal
2-R 3-R - too short
4-B 5-B 6-B 7-R - not the same color

I'll use a slightly generalized definition of a run:
a run is a sequence that is made of several classical runs of the same color.

Legal:
2-R 3-R 4-R 7-R 8-R 9-R (two classical runs)
Illegal
2-R 3-R 4-R 7-G 8-G 9-G (not same color)

Algorithm:
brute-force recursion: for each next tile you have the following options
1) add it to an existing run with the same color as tile and tile's denomination is not yet in the run
2) add it to an existing group with the same denomination as tile and tile's color is not yet in the group
3) create a new run
4) create a new group

solve( ... ) 
{
    // base case
    // out of tiles - check if runs and groups are legal
    // return true if yes and exit

    Action actions[] = GetOptions();

    for ( action : actions ) {
        if action is legal execute it
        solve( ... ); // recursive call
        if solved - return true;
        undo the action
    }

    return false; // could not find place for current tile - backtrack
}

Note that as the number of runs and groups grows, so is the number of options
above, with 3 runs and 4 groups it will be up to 3+4+1+1=9 options - i.e. branching
factor grows with the depth of the tree. The worst case scenario is 2 options for 
the first tile, 3 for the next, etc, so the maximum for the total number of leaf
nodes is 2*3*...*n = n! where n is the number of tiles.

Expected time:
since test3 us randomized, I cannot set a specific timeout,
What I will do:
a) I'll run test3 several times giving a lot of time - this will be a test of correctness.
b) I'll run test4 (which is test3 1000 times) and give it 250s (so that average time per test3 test is 0.25s)

using CygWin to run 1000 tests and time them:
time for((i=0;i<1000;++i)); do echo $i; ./gcc0.exe 3 &> /dev/null; done

```