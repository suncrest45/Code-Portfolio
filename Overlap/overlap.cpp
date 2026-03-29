///---------------------------------------------------------------------------
/// @file overlap-skeleton.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-11-30
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "overlap.h"

#include <vector>   // std::vector
#include <iostream>
#include <fstream>  // std::ifstream
#include <algorithm>

int MOD = 7001; // for modulo counting

// A struct representing a rectangle
struct R 
{
    int x1; // starting x coordinate of the rectangle
    int y1; // starting y coordinate of the rectangle
    int x2; // ending x coordinate of the rectangle
    int y2; // ending y coordinate of the rectangle
};

struct Line 
{
    int x1;
    int x2;
    int y;
    bool start;     // true - start. false - end
    bool operator< (Line const& other) const { return y<other.y; }
};

////////////////////////////////////////////////////////////////////////////////
//   START BRUTE FORCE
////////////////////////////////////////////////////////////////////////////////
struct BruteForce 
{
    std::vector< std::vector<int> > field;
    std::map<int,int> freq;

    BruteForce( int dim ) : field( dim, std::vector<int>( dim,0 ) ), freq() { }

    void AddRectangle( int x1, int y1, int x2, int y2 ) {
        for ( int i = x1; i<=x2; ++i ) {    // note "<=" !!!
            for( int j = y1; j<=y2; ++j ) { // note "<=" !!!
                ++field[i][j];
            }
        }
    }

    void Process() {
        for ( unsigned i = 0; i<field.size(); ++i ) {
            for( unsigned j = 0; j<field.size(); ++j ) {
                ++freq[ field[i][j] ];
                freq[ field[i][j] ] %= MOD; // modulo so no overflow
            }
        }
    }
};

std::map<int,int> brute_force( char const * filename )
{
    std::ifstream in( filename ); // closed automatically
    if ( in.fail() ) throw "Cannot open input file";

    std::vector<R> rects;
    int dim, N;
    in >> dim >> N;
    for ( int r=0; r<N; ++r ) {
        int x1,y1,x2,y2;
        in >> x1 >> y1 >> x2 >> y2;
        rects.push_back( R{x1,y1,x2,y2} );
    }

    BruteForce brf( dim );

    for( auto const & r : rects ) {
        brf.AddRectangle( r.x1, r.y1, r.x2, r.y2 );
    }

    brf.Process();

    return brf.freq;
}
////////////////////////////////////////////////////////////////////////////////
//   END BRUTE FORCE
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//   YOUR IMPLEMENTATION 
////////////////////////////////////////////////////////////////////////////////

/// @brief Read in all the rectangle making up the grid
/// @param filename  - The name of the file to read from
/// @param grid_size - A reference to the size of the grid
/// @return A vector representing all rectangle son the grid
std::vector<R>read_rect(const char* filename, int& grid_size)
{
    // Create an input stream to read from the file
    std::ifstream FILE(filename);

    // if the file was not opened successfully
    if (!FILE.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }

    // Read in the size of the grid 
    FILE >> grid_size;

    // Number of rectangles
    int num_rectangle = 0;
    FILE >> num_rectangle;

    // Create a vector to hold all the rectangles
    std::vector<R> rectangles;

    for (int i = 0; i < num_rectangle; i++)
    {
        // The start and end coordinates of the rectangle
        int x1, y1, x2, y2;
        FILE >> x1 >> y1 >> x2 >> y2;

        // Create and a rectangle to the vector
        rectangles.push_back({x1, y1, x2, y2});
    }
    
    return rectangles;
}

/// @brief Create events from the rectangles using their start and end coordinates
/// @param rectangles - The vector of rectangles to create the events from
/// @return A container of all events created from the rectangles
std::vector<Line> create_events(const std::vector<R>& rectangles)
{
    // The container for all events
    std::vector<Line> events;

    // For each rectangle create a start and end event
    for (const R& rect : rectangles)
    {
        // Create the start event
        Line start_event;
        start_event.x1 = rect.x1;
        start_event.x2 = rect.x2;
        start_event.start = true;
        start_event.y = rect.y1;
        events.push_back(start_event);

        // Create the end event
        Line end_event;
        end_event.x1 = rect.x1;
        end_event.x2 = rect.x2;
        end_event.start = false;
        end_event.y = rect.y2 + 1;
        events.push_back(end_event);
    }
    
    return events;
}

/// @brief 
/// @param y - The starting y
/// @param last_y - The ending y
/// @param scan_line - A "line" of coordinates corresponding to the map
/// @param overlap 
void add_area(int y, int last_y, std::vector<int>& scan_line, std::map<int, int>& overlap)
{
    int distance = y - last_y;

    for (int level : scan_line)
    {
        overlap[level] += distance;
        overlap[level] %= MOD;
    }
}


void update_scan_line(Line next_event, std::vector<int>& scan_line)
{
    // given this event, increment/decremt the value of event.x1 through event.x2 in the scan line
    int dir = next_event.start ? 1 : -1;

    // adjust the scan line values
    for (int i = next_event.x1; i <= next_event.x2; i++) { scan_line[i] += dir; }
}



////////////////////////////////////////////////////////////////////////////////
std::map<int,int> overlap( char const * filename ) 
{
    // Size of the grid
    int grid_size = 0;

    // Create a vector rectangles and populate grid size
    std::vector<R> rectangles = read_rect(filename, grid_size);
    // Create a vector of events from the rectangles
    std::vector<Line> events = create_events(rectangles);

    // Sort the events
    std::sort(events.begin(), events.end());

    // Initialise the scan line and set all positions to zero
    std::vector<int> scan_line(grid_size, 0);

    int last_y = 0;

    // Create the overlap map
    std::map<int, int> overlap_map = std::map<int, int>();

    // For each event, ddetermine the overlap
    for (Line event : events)
    {
        add_area(event.y, last_y, scan_line, overlap_map);
        update_scan_line(event, scan_line);
        last_y = event.y;
    }
    
    return overlap_map;
}


