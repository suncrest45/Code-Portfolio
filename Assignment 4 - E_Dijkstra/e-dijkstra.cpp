///---------------------------------------------------------------------------
/// @file e-dijkstra.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-11-13
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------

#include "e-dijkstra.h"
#include <iostream>
#include <vector>  //std::vector
#include <queue>   // std::priority_queue
#include <fstream> // std::ifstream
#include <sstream>
#include <limits> // std::numeric_limits

// The longest possible distance
const int INFINITE_DISTANCE = std::numeric_limits<int>::max();
// The maximum range of the car, this will be set by the dijkstra call
int MAX_RANGE = 0;

struct Edge
{
    int target; // Target node of the edge
    int weight; // Weight of the edge or distance between nodes
};

struct State
{
    int node;        // Current node
    int r_recharges; // Number of recharges left
    int r_range;     // Remaining battery range

    /// @brief Comparator for the priority queue
    /// @param other - The other state to compare
    /// @return If the current state is less than the other
    bool operator<(const State& other) const
    {
        int current_state_viability = r_recharges * MAX_RANGE + r_range;
        int other_state_viability = other.r_recharges * MAX_RANGE + other.r_range;

        return current_state_viability < other_state_viability;
    }
};

/// @brief Parse the graph data from the file and create an adjacency list
/// @param filename      - The name of the file storing the info
/// @param max_recharges - The number of recharges the car is allowed
/// @return The adjacency list
std::vector<std::vector<Edge>> graph_data(const char* filename, int& max_recharges)
{
    // Open the file for reading
    std::ifstream FILE(filename);

    // Throw an error if the file could not be opened
    if (!FILE.is_open())
    {
        throw std::runtime_error("Could not open file");
    }

    // Number of nodes that could be visisted
    int num_nodes = 0;
    // The number of edges a node has
    int edges = 0;

    // Read in the values from the file
    FILE >> num_nodes >> max_recharges >> edges;

    // Create the list
    std::vector<std::vector<Edge>> graph(num_nodes);

    int u = 0;
    int v = 0;
    // The cost to use this edge
    int weight = 0;

    for (int i = 0; i < edges; i++)
    {
        // Read in the coordinates and weight of the node
        FILE >> u >> v >> weight;
        // Construct edges using this info
        graph[u].push_back({v, weight});
        graph[v].push_back({u, weight});
    }
    
    FILE.close();
    return graph;
}


/// @brief A helper function for starting at a given node
/// @param graph - The graph of nodes we could traverse
/// @param start - The node to start from
/// @param max_recharges - The maximum amount of recharges that can be made
/// @param vehicle_range - The range of the vehicle
/// @return Can all nodes be reached from this position
bool sub_dijkstra(const std::vector<std::vector<Edge>>& graph, int start, int max_recharges, int vehicle_range)
{
    // Size of the graph
    int n = graph.size();

    // Minimum remaining range to reach each node with given recharges left
    std::vector<std::vector<int>> min_distance(n, std::vector<int>(max_recharges + 1, -1));

    min_distance[start][max_recharges] = vehicle_range;

    // The open list
    std::priority_queue<State> open_list;

    // Start with no range left
    open_list.push({start, max_recharges, 0});

    while (!open_list.empty())
    {
        // Get the first node in the list
        State currrent = open_list.top();
        open_list.pop();

        for (const Edge& edge : graph[currrent.node])
        {
            int next_node = edge.target;
            int distance_to_next_node = edge.weight;
            int remaining_range = currrent.r_range - distance_to_next_node;

            // Check if there is charge in the battery
            if (remaining_range >= 0)
            {
                // Check if we can travel to the next node without a recharge
                if (remaining_range > min_distance[next_node][currrent.r_recharges])
                {
                    min_distance[next_node][currrent.r_recharges] = remaining_range;
                    // Add that node to the open list
                    open_list.push({next_node, currrent.r_recharges, remaining_range});
                }
            }

            if (currrent.r_recharges > 0 && vehicle_range - distance_to_next_node >= 0)
            {
                if (vehicle_range - distance_to_next_node > min_distance[next_node][currrent.r_recharges - 1])
                {
                    min_distance[next_node][currrent.r_recharges - 1] = vehicle_range - distance_to_next_node;
                    // Add the node to the open list
                    open_list.push({next_node, currrent.r_recharges - 1, vehicle_range - distance_to_next_node});
                }
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        bool is_reachable = false;

        for (int k = 0; k <= max_recharges; k++)
        {
            if (min_distance[i][k] >= 0)
            {
                is_reachable = true;
                break;
            }
        }

        if (!is_reachable)
        {
            return false;
        }
    }
    
    return true;
}

/// @brief Checks if a vehicle is capable of reaching every node
/// @param filename      - The file to create the grid from
/// @param vehicle_range - The range of the vehicle we are testing
/// @return CAn this vehicle reach all nodes
bool e_dijkstra(char const *filename, int vehicle_range)
{
    // Set the range of the vehicle
    MAX_RANGE = vehicle_range;
    int max_recharges; 

    auto graph = graph_data(filename, max_recharges);

    for (unsigned long start = 0; start < graph.size(); start++)
    {
        if (!sub_dijkstra(graph, start, max_recharges, vehicle_range))
        {
            MAX_RANGE = 0;
            return false;
        }
        
    }
    
    MAX_RANGE = 0;
    return true;
}
