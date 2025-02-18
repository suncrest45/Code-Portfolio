#include "tsp.h"
#include <fstream>  // std::ifstream
#include <limits>   // std::numeric_limits
#include <limits.h> // INT_MAX




/// @brief Read in all the necessary information for TSP from the specified file
/// @param filename    - The name of the file to read from
/// @param map         - The adjacency matrix shoing distances from one city to another
/// @param totalCities - The total number of cities the salesperson must visit
void Read(char const* filename, MAP& map, int& totalCities)
{
    // Empty the map so we start with fresh routes
    map.clear();

    // Open the file for reading
    std::ifstream input(filename, std::ifstream::in);

    // If the file could not be opened
    if (!input.is_open())
    {
        return;
    }

    // Read in the total amount of cities
    input >> totalCities; 

    // Create and initialise the rows of the map
    // By default the distance between each city is infinity
    for( int i = 0; i < totalCities; ++i ) 
    {
        std::vector<int> row;
        for( int j = 0; j < totalCities; ++j )
        {
            row.push_back( std::numeric_limits<int>::max() );
        }
        map.push_back(row);
    }

    // Read in the values for the distances between the cities
    // If a path between a city is infinity it means there is no path 
    for( int i = 0; i < totalCities; ++i ) 
    {
        for( int j = i + 1; j < totalCities; ++j )
        {
            if( !input.good())
            {
                return;
            }
            input >> map[i][j];
            map[j][i] = map[i][j];
        }
    }
    
}

/// @brief Cop the current path into the final path
/// @param current_path - The determined best route
/// @param final_path   - The best route
/// @param totalCities  - The number of cities to travel to
void CopyToFinal(std::vector<int>& current_path, std::vector<int>& final_path, int totalCities)
{
    for (int i = 0; i < totalCities; i++)
    {
        final_path[i] = current_path[i];
    }
    
    final_path[totalCities] = current_path[0];
    return;
}

/// @brief Find the first minimum edge cost
/// @param map         - the vector of vectors holding the costs of all possible routes
/// @param i           - The specified row in the vector to search
/// @param totalCities - The number of cities to travel to
/// @return The minimum edge cost
int first_minimum(MAP& map, int i, int totalCities)
{
    int min = INT_MAX;

    for (int j = 0; j < totalCities; j++)
    {
        if (map[i][j] < min && i != j)
        {
            min = map[i][j];
        }
    }
    return min;
}

/// @brief Find the second minimum edge cost
/// @param map         - The vector of vectors holding the costs of all possible routes
/// @param i           - The specific route we are examining 
/// @param totalCities - The number of cities to travel to
/// @return The second minimum edge
int second_minimum(MAP& map, int i, int totalCities)
{
    int first = INT_MAX, second = INT_MAX;

    for (int j = 0; j < totalCities; j++)
    {
        if (i == j)
        {
            continue;
        }

        if (map[i][j] <= first)
        {
            second = first;
            first = map[i][j];
        }
        else if (map[i][j] <= second && map[i][j] != first)
        {
            second = map[i][j];
        }
    }
    
    return second;
}

/// @brief Calculate the shortest path to visit all cities once
/// @param map - The vector of vectors of all possible routes
/// @param current_bound  - Lower bound of the starting city
/// @param current_weight - The weight of the path so far
/// @param level          - Current city the salesperson is at on their journery
/// @param current_path   - A vector holding the current path
/// @param Visited        - A vector keeping track of which cities have been visited
/// @param totalCities    - The number of cities to visit
/// @param final_path     - The optimal path the salesperson must take
/// @param final_result   - Stores the final minimum weight oof the shortest tour
void TSPRecursive(MAP& map, int current_bound, int current_weight, 
int level, std::vector<int>& current_path, std::vector<bool>& Visited, 
int totalCities, std::vector<int>& final_path, int& final_result)
{
    // Base case is when we have travelled to all cities once
    if (level == totalCities)
    {
        // Check if there is a path from the last city to the first city
        if (map[current_path[level - 1]][current_path[0]] != 0)
        {
            int current_result = current_weight + map[current_path[level - 1]][current_path[0]];

            // If this path is shorter
            if (current_result < final_result)
            {
                CopyToFinal(current_path, final_path, totalCities);
                final_result = current_result;
            }
        }
        return;
    }

    // For every level, iterate through all the routes to find the best
    // routes
    for (int i = 0; i < totalCities; i++)
    {
        if (map[current_path[level - 1]][i] != 0 && Visited[i] == false)
        {
            int temp = current_bound;
            current_weight += map[current_path[level - 1]][i];

            if (level == 1)
            {
                current_bound -= ((first_minimum(map, current_path[level - 1], totalCities) + 
                                   first_minimum(map, i, totalCities)) / 2);
            }
            else
            {
                current_bound -= ((second_minimum(map, current_path[level - 1], totalCities) + 
                                   first_minimum(map, i, totalCities)) / 2);
            }
            
            // current bound and current weight is the actual lower bound
            // If this bound is less than the final result, we need to explore this route further
            if (current_bound + current_weight < final_result)
            {
                current_path[level] = i;
                Visited[i] = true;

                // Call this function again for the next city
            TSPRecursive(map, current_bound, current_weight, level + 1, current_path, Visited, totalCities, final_path, final_result);
            }
            
            // Else we have to remove the current city by resetting current bound and weight
            current_weight -= map[current_path[level - 1]][i];
            current_bound = temp;

            Visited.clear();

            for (int j = 0; j <= level - 1; j++)
            {
                Visited[current_path[j]] = true;
            }
        }
    }
}


/// @brief Given a map determine the best route a salesperson should take
/// @param filename - The name of the file the map is stored in
/// @return The best route that covers all cities only once
std::vector<int> SolveTSP(char const *filename)
{
    // The number of cities to visit
    int totalCities = 0;
    // The distances between each city from different cities
    MAP map;
    // Stores the final minimum weight oof the shortest tour
    int final_result = INT_MAX;

    // Get all the necessary information from the file
    Read(filename, map, totalCities);

    // A bit mask indicating which cities have been visited
    int VISITED_ALL = (1 << totalCities) - 1;

    // The path of the salesperson
    std::vector<int> current_path(totalCities + 1, -1);
    // The final path of the salesperson
    std::vector<int> final_path(totalCities + 1, -1);
    // Keeps track of the already visited cities on a path
    std::vector<bool> Visited(totalCities, false);
    int current_bound = 0;

    // Compute initial bound
    for (int i = 0; i < totalCities; i++)
    {
        current_bound += (first_minimum(map, i, totalCities) + second_minimum(map, i, totalCities));
    }

    

    // We start at the first city
    Visited[0] = true;
    current_path[0] = 0;
    
    TSPRecursive(map, current_bound, 0, 1, current_path, Visited, totalCities, final_path, final_result);

    return final_path;
}
