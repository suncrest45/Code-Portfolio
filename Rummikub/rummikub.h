/// --------------------------------------------------------------------------
/// @file rummikub.h
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-02-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------

#ifndef RUMMIKUB_H
#define RUMMIKUB_H
#include <fstream>
#include <vector>
#include <iostream>

enum Color { Red, Green, Blue, Yellow };


struct Tile
{
    int     denomination;
    Color   color;
};

using RUN = std::vector<Tile>;
using GROUP = std::vector<Tile>;

std::ostream& operator<<(std::ostream& os, Tile const& t);
class RummiKub 
{
    
    public:
        RummiKub(); // empty hand
        void Add( Tile const& tile); // add a tile to hand

        void Solve(); // solve

        // get solution - groups
        std::vector< std::vector< Tile > > GetGroups() const;
        // get solution - runs
        std::vector< std::vector< Tile > > GetRuns() const;
        // if both vectors are empty - no solution possible

    private:
        
        std::vector<Tile> m_Hand; // The player's hand.
        std::vector<GROUP> m_SolvedGroups;
        std::vector<RUN> m_SolvedRuns;
        int count;

        // Check if a set of tiles are a run 
        bool isRun(const RUN& tiles) const;
        // Check if a set of tiles are a group
        bool isGroup(const GROUP& tiles) const;
        // Check if all groups and runs are valid.
        bool CheckTiles() const;
        // Attempt to add a tile to a run in the solved runs vector.
        bool AddRuns(Tile tile, int index);
        // Attempt to add a tile to a group in the solved groups vector.
        bool AddGroups(Tile tile, int index);
        // If a run cannot be solved undo, the latest action performed on the run.
        void UndoRun(int index);
        // If a group cannot be solved undo, the latest action performed on the group.
        void UndoGroup(int index);
        // Attempt to solve the specified hand.
        bool Solve(std::vector<Tile> hand);
};

#endif
