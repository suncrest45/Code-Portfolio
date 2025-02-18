You are exploring a problem whether a particular electric vehicle is acceptable
for a given town. The main question is - given the range of the vehicle (range
is based on a full charge) will that vehicle be able to go from any location to 
any other location with the maximum of K recharges.

Side note: the latter requirement may seem a little strange, but think recharge
takes considerable amount of time, so you want to limit the total travel time
by requiring the number of recharges to be limited.

Town layout is provided as a *connected* *undirected* graph, edge weight
corresponds to distance, *each* vertex of the graph has a recharging station 
(so recharging is possible in ALL vertices, but ONLY in vertices). 

The latter results in that shortest path is not necessarily the best path for
the electric car -- see examples below (R=350).

Problem: given the graph and number of recharges and the maximum range of a vehicle, will that vehicle
be able to reach any city from any other city? 


Input file format (in0):
4 2 4    // N (number of location) K (maximum recharges) M(number of edges)
0 1 100  // edge from 0 to 1 weight 100
1 2 200
2 3 300
3 0 400

Notes:
 - vehicle starts empty, so you have to recharge before departing

Example: K = 2, R = 250
                 200
             1-----------2
            /             \
        100/               \300
          /                 \
         0-------------------3
                400

Check if 0->3 is possible:
    recharge at location 0, now 1 recharge left and vehicle has 250 of
    electricity goto to 1, now 1 recharge left and vehicle has 150 of
    electricity not enough to reach 2, so recharge (150 remaining charge is
    wasted), now 0 recharge left and vehicle has 250 of electricity,
    goto 2: now 0 recharge left and vehicle has 50 of electricity not enough to
    reach 3. We should also check if 0-3 edge (cost 400) is possible - but
    since 400>250, we the vehicle does not have enough charge, so 0->3 is is
    impossible.  We may terminate and return false at this point (without
    checking the remaining pairs of vertices).
   



Example: K = 2, R = 350
                 200
             1-----------2
            /             \
        100/               \300
          /                 \
         0-------------------3
                400

Example: K = 2, R = 350
    recharge at 0, now 1 recharge left and vehicle has 350 of electricity
    goto to 1, now 1 recharge left and vehicle has 250 of electricity
    this is enough to reach 2, 
    goto to 2, now 1 recharge left and vehicle has 50 of electricity
    not enough to reach 3, so recharge (50 remaining charge is wasted), 
    now 0 recharge left and vehicle has 350 of electricity,
    goto 3: now 0 recharge left and vehicle has 50 of electricity.

    Notice that the shortest path from 0 to 3 (400) does not
    allow the car to reach the target.

    all locations reached from 0. Check the same for:
        0->1  is possible - yes, 0-1
        0->2  is possible - yes, 0-1-2
        1->2  is possible - yes, 1-2
        1->3  is possible - yes, 1-2-3
        2->3  is possible - yes, 2-3
        (note that since graph is undirected, checking 3->0 is unnecessary
    Since all succeed - return true.


Algorithm idea:
use modified Dijkstra algorithm that keeps track of a pair (k,r), where 
k - number of recharges left
r - amount of electricity left in the battery

To think: Dijkstra needs to be able to compare the above pairs, what is the logic of
(k1,r1) < (k2,r2)
?

Use in0 and in1 for debugging. in0 picture is above.
Use provided pdf-file "in1.dot.pdf" for in1.
