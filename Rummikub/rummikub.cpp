/// --------------------------------------------------------------------------
/// @file rummikub.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-02-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "rummikub.h"
#include <algorithm>

///---------------------------------------------------------------------------
/// @brief Default Constructor 
///---------------------------------------------------------------------------
RummiKub::RummiKub()
    : m_Hand()
    , m_SolvedGroups()
    , m_SolvedRuns()
{}

///---------------------------------------------------------------------------
/// @brief Add a tile to the player's hand
/// @param - The tile to be added.
///---------------------------------------------------------------------------
void RummiKub::Add(Tile const & tile) { m_Hand.push_back(tile); }

///---------------------------------------------------------------------------
/// @brief Solves the runs and groups.
///---------------------------------------------------------------------------
void RummiKub::Solve()
{
    // Sort the hand based on the greater denomination.
    std::sort(m_Hand.begin(), m_Hand.end(), [](Tile const& tile, Tile const& other){ return (tile.denomination > other.denomination); });
    Solve(m_Hand);

}

///---------------------------------------------------------------------------
/// @brief  Gets a vector of all solved groups.
/// @return A vector of all solved groups.
///---------------------------------------------------------------------------
std::vector<std::vector<Tile>> RummiKub::GetGroups() const { return m_SolvedGroups; }

///---------------------------------------------------------------------------
/// @brief  Get a vector of all solved runs.
/// @return A vector of all solved runs.
///---------------------------------------------------------------------------
std::vector<std::vector<Tile>> RummiKub::GetRuns() const { return m_SolvedRuns; }

///---------------------------------------------------------------------------
/// @brief Check if a set of tiles is a run
/// @param tiles - the set of tiles to check.
/// @return Whether this set is a run or not (bool).
///---------------------------------------------------------------------------
bool RummiKub::isRun(const RUN &tiles) const 
{ 
    if (tiles.size() < 3)
    {
        return false;
    }
    return true; 
}

///---------------------------------------------------------------------------
/// @brief Check if the set of tiles is a group
/// @param tiles - The set of tile to check
/// @return Is the set a group (bool).
///---------------------------------------------------------------------------
bool RummiKub::isGroup(const GROUP &tiles) const 
{ 
    if (tiles.size() < 3)
    {
        return false;
    }
    return true;
}

///---------------------------------------------------------------------------
/// @brief Checks if all groups and runs are the correct size.
/// @return True if all groups and runs are the right size, false if not (bool).
///---------------------------------------------------------------------------
bool RummiKub::CheckTiles() const
{
  // If both groups and runs are empty.
  if (m_SolvedGroups.empty() && m_SolvedRuns.empty())
  {
    return true;
  }
  
  // Check if all runs are the correct size.
  for (const RUN& run : m_SolvedRuns )
  {
    if (isRun(run) == false)
    {
        return false;
    } 
  }
  
  // Check if all the groups are the correct size.
  for (const GROUP& group : m_SolvedGroups)
  {
    if (isGroup(group) == false)
    {
        return false;
    }
  }

  return true;
}

///---------------------------------------------------------------------------
/// @brief Add a tile to the run at a given index.
/// @param tile - The tile to add to the run.
/// @param index - The position of the tile in the run.
/// @return If this tile is added, is the run still legal (bool)?
///---------------------------------------------------------------------------
bool RummiKub::AddRuns(Tile tile, int index)
{
    // If the set of runs is empty or the index is the last position in the set.
    if (m_SolvedRuns.empty() || index == static_cast<int>(m_SolvedRuns.size()))
    {
        // Constructs a vector of tiles using the parameter
        // As the first item in the newly created vector.
        // That newly created vector is then placed into the solved runs vector.
        m_SolvedRuns.push_back({tile});
        return true;
    }

    // Prevent overwriting
    // As long as the colours are the same.
    // As long as the last tile has the same denomination.
    if (static_cast<int>(m_SolvedRuns.size()) > index
        && tile.color == m_SolvedRuns[index][0].color
        && m_SolvedRuns[index][0].denomination + static_cast<int>(m_SolvedRuns[index].size()) == tile.denomination)
    {
        // Add the tile after all checks.
        m_SolvedRuns[index].push_back(tile);
        return true;
    }

    return false;
}

///---------------------------------------------------------------------------
/// @brief Add a tile to the solved groups at a specified index.
/// @param tile  - The tile to add to the group.
/// @param index - The index to add the tile at.
/// @return If this tile is added, is the group still legal (bool)?
///---------------------------------------------------------------------------
bool RummiKub::AddGroups(Tile tile, int index)
{
    // If the set of runs is empty or the index is the last position in the set.
    if (m_SolvedGroups.empty() || index == static_cast<int>(m_SolvedGroups.size()))
    {
        // Constructs a vector of tiles using the parameter
        // As the first item in the newly created vector.
        // That newly created vector is then placed into the solved groups vector.
        m_SolvedGroups.push_back({tile});
        return true;
    }

    // Prevents overwriting
    // If the group is not full
    // As long as the denominations are the same
    // As long as the colours differ
    // The tile can be added
    if (static_cast<int>(m_SolvedGroups.size()) > index
        && static_cast<int>(m_SolvedGroups[index].size()) < 4
        && tile.denomination == m_SolvedGroups[index][0].denomination)
    {
        for (size_t i = 0; i < m_SolvedGroups[index].size(); i++)
        {
            if (tile.color == m_SolvedGroups[index][i].color)
            {
                return false;
            }
            
        }
        
        // Add the tile after all checks.
        m_SolvedGroups[index].push_back(tile);
        return true;
    }
    return false;
}

///---------------------------------------------------------------------------
/// @brief Undo the latest action performed on a specified run.
/// @param index - The position of the run in the solved runs vector.
///---------------------------------------------------------------------------
void RummiKub::UndoRun(int index)
{
    // If the index is out of bounds.
    if (static_cast<int>(m_SolvedRuns.size()) < index)
    {
        return;
    }

    // Remove the tile from the end of the run.
    m_SolvedRuns[index].pop_back();
    // If the run is empty, remove from the solved runs vector.
    if (m_SolvedRuns[index].empty())
    {
        m_SolvedRuns.erase(m_SolvedRuns.begin() + index);
    }
}

///---------------------------------------------------------------------------
/// @brief Undo the latest action perform on a specified group
/// @param index - The position of the group in the solved groups vector.
///---------------------------------------------------------------------------
void RummiKub::UndoGroup(int index)
{
    // If the index is out of bounds.
    if (static_cast<int>(m_SolvedGroups.size()) > index)
    {
        // Remove the tile from the end of the group.
        m_SolvedGroups[index].pop_back();
        // If the group is empty, from the solved groups vector.
        if (m_SolvedGroups[index].empty())
        {
            m_SolvedGroups.erase(m_SolvedGroups.begin() + index);
        }
    }
}

///---------------------------------------------------------------------------
/// @brief Attempts to solve the passed in hnad.
/// @param hand - The hand to be solved.
///---------------------------------------------------------------------------
bool RummiKub::Solve(std::vector<Tile> hand)
{
    if (hand.empty())
    {
        return CheckTiles();
    }
    
    // The last tile in the hand.
    Tile tile = hand.back();
    // Remove the tile from the list.
    hand.pop_back();

    // Traverse the solved runs, seeing if the tile can be added.
    for (int i = 0; i <= static_cast<int>(m_SolvedRuns.size()); i++)
    {
        // Attempt to add the tile to a run.
        if (AddRuns(tile, i))
        {
            // If successfully added, attempt to solve.
            if (Solve(hand))
            {
                return true;
            }
            else
            {
                UndoRun(i);
            }
        }
    }

    // Traverse the solved groups, seeing if the tile can be added.
    for (int i = 0; i <= static_cast<int>(m_SolvedGroups.size()); i++)
    {
        // Attempt to add the tile to the groups.
        if (AddGroups(tile, i))
        {
            // If successfully added attempt to solve.
            if (Solve(hand))
            {
                return true;
            }
            else
            {
                UndoGroup(i);
            }
        }   
    }
    
    return false;
}

///---------------------------------------------------------------------------
/// @brief output stream overload.
/// @param os - The output stream to overload.
/// @param t  - The tile to print.
/// @return The overloaded output stream printing a tile.
///---------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, Tile const &t)
{
    os << "{ "<< t.denomination << ",";
    switch ( t.color ) {
        case Red:      os << "R"; break;
        case Green:    os << "G"; break;
        case Blue:     os << "B"; break;
        case Yellow:   os << "Y"; break;
    }
    os << " }";
    return os;
}
