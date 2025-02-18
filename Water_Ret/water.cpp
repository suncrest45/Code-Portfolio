/// --------------------------------------------------------------------------
/// @file water.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief  Implemented a water flow simulation using a priority queue
/// @version 0.1
/// @date 2024-04-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------




///---------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------
#include "water.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <climits>



/// @brief container that holds the water with a 2D space position
struct Cell 
{
    int height; // height of the cell
    int pos[2]; // position of the cell (x and y)

    ///---------------------------------------------------------------------------
    /// @brief Overloaded less than operator
    /// @param c - the other cell we are comparing against.
    /// @return If the current cell is less than the passed in one (bool)
    ///---------------------------------------------------------------------------
    bool operator<(const Cell& c) const { return height > c.height; }
};



///---------------------------------------------------------------------------
/// @brief Helper function to read in the data from a file
///        and create a data structure for water_ret

/// @param filename - The name of the file to read the data from
/// @param row      - Number of rows in the file.
/// @param col      - Number of columns in the file.
/// @return The data structure for water_ret (std::vector<std::vector<int>>)
///---------------------------------------------------------------------------

std::vector<std::vector<int>> readfile(const char* filename, int& row, int& col)
{
    // Open the file
    std::ifstream file(filename); 

    // Check if file is open
    if (!file.is_open()) 
    { 
        // Print error message
        std::cerr << "Unable to open file\n"; 
        // Exit the program
        exit(-1); 
    }

    // Read in the row and column detentions from the file
    file >> row >> col; 

    // create a 2D vector to store the heights
    std::vector<std::vector<int>> heights(row, std::vector<int>(col));

    // Loop through the rows
    for (int i = 0; i < row; ++i) 
    { 
        // Loop through the columns
        for (int j = 0; j < col; ++j) 
        { 
            // Read in the heights from the file
            file >> heights[i][j]; 
        }
    }

    // Close the file 
    file.close(); 

    // return the 2D vector
    return heights; 
}


///---------------------------------------------------------------------------
/// @brief Determines how much water is retained based on given inputs
/// @param filename - The file to read the inputs from.
/// @return The amount of water retained (long int)
///---------------------------------------------------------------------------
long int waterret(const char* filename)
{
    
    int rows;   // rows and columns
    int cols;

    // read in the file and store the 2D vector
    auto heights = readfile(filename, rows, cols);

    // create a 2D vector to store the water
    std::vector<std::vector<int>> water(rows, std::vector<int>(cols, 0));

    // create a 2D vector to store the cutoffs (where water will stop)
    std::vector<std::vector<int>> cutoff(rows, std::vector<int>(cols, INT_MAX));

    // create a priority queue
    std::priority_queue<Cell, std::vector<Cell>> cell_queue;

    // Add all perimeter cells to the priority queue
    // loop through the rows
    for (int i = 0; i < rows; i++) 
    {
        // loop through the columns   
        for (int j = 0; j < cols; j++) 
        {
            // Check if the cell is a perimeter cell    
            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1) 
            { 
                // add the cell to the priority queue
                cell_queue.push({heights[i][j], {i, j}}); 
                // set the cutoff to the height of the cell         
                cutoff[i][j] = heights[i][j];                         
            }
        }
    }

    // create a 2D array to store the directions
    std::vector<std::pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    // tick through the simulation
    while (!cell_queue.empty()) 
    {
        // Get the top cell
        Cell cell = cell_queue.top(); 
        // Remove the top cell
        cell_queue.pop();      
        // Get the height of the current cell
        int currHeight = cell.height; 
        // Get the x position of the cell
        int x = cell.pos[0];
        // Get the y position of the cell.
        int y = cell.pos[1]; 

        // loop through the directions
        for (auto& item : directions) 
        {
            // new x position
            int newX = x + item.first; 
            // new y position
            int newY = y + item.second; 

            // Boundaries check
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) 
            { 
                // calculate the new height
                int new_height = std::max(currHeight, heights[newX][newY]); 

                // check if updating is necessary
                if (new_height < cutoff[newX][newY]) 
                { 
                    // update the cutoff
                    cutoff[newX][newY] = new_height; 
                    // add the new cell to the queue
                    cell_queue.push({new_height, {newX, newY}}); 

                    // Calculate trapped water if any
                    if (cutoff[newX][newY] > heights[newX][newY]) 
                    {
                        water[newX][newY] = cutoff[newX][newY] - heights[newX][newY];
                    }
                }
            }
        }
    }

    // look through the cutoffs and calculate the water
    long int waterRetained = 0;

    // loop through the rows
    for (int i = 0; i < rows; i++) 
    {
        // loop through the columns     
        for (int j = 0; j < cols; j++) 
        { 
            // add the water to the total
            waterRetained += water[i][j];       
        }
    }

    // return the water retained
    return waterRetained; 
}