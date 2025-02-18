#include "hull-bruteforce.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <set>

bool Point::operator==( Point const& arg2 ) const 
{
    return ( (x==arg2.x) && (y==arg2.y) );
}

std::ostream& operator<< (std::ostream& os, Point const& p)
{
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) 
{
	os >> p.x >> p.y;
	return os;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// return value is (on left, on right)
std::pair<bool,bool> get_location(
		float const& p1x, //check which side of the line (p1x,p1y)-->(p2x,p2y) 
		float const& p1y, //point (qx,qy) is on
		float const& p2x,
		float const& p2y,
		float const& qx,
		float const& qy
		) 
{
	Point dir   = {p2x-p1x,p2y-p1y};
	Point norm  = {dir.y, -dir.x};
	Point point = {qx-p1x,qy-p1y};
	//scalar product is positive if on right side
	float scal_prod = norm.x*point.x + norm.y*point.y;
	return std::make_pair( (scal_prod<0), (scal_prod>0));
}

/// @brief Get the indicies of all the points that form the convex hull
/// @param points - A vector containing the points of the polygon
/// @return The set of points that make the convex hull 
std::set<int> hullBruteForce ( std::vector< Point > const& points ) 
{
	int num_points = points.size();
	if ( num_points < 3 ) throw "bad number of points";

	std::set<int> hull_indices;

	// This gets the first point of the line to check
	for (int i = 0; i < num_points - 1; i++)
	{
		// This forms the line
		for (int j = i + 1; j < num_points; j++)
		{
			bool left = true;
			bool right = true;

			// This checks every point except the two making the line to see if the points are part of the convex hull
			for (int k = 0; k < num_points; k++)
			{
				// Skip the points forming the line segment
				if (k == i || k == j)
				{
					continue;
				}

				// Get the location of the point in relation to the line, false, true means the point is on the right side of the line
				// true, false means the point is on the left side of the line.
				std::pair<bool, bool> result = get_location(points[i].x, points[i].y, points[j].x, points[j].y, points[k].x, points[k].y);	

				if (result.first == false)
				{
					left = false;
				}

				if (result.second == false)
				{
					right = false;
				}
				
				// If both results are false break 
				if (!left && !right)
				{
					break;
				}
				
			}

			// If either of these are true add the indicies to the set
			if (left || right)
			{
				hull_indices.insert(i);
				hull_indices.insert(j);
			}
		}
	}
	
	return hull_indices;
}

std::vector<int> hullBruteForce2 ( std::vector< Point > const& points ) 
{
	int num_points = points.size();
	if ( num_points < 3 ) throw "bad number of points";

	std::vector<int> hull_indices;

	// This variable will store the smallest x value
	float smallest_x = points[0].x;
	int start = 0;

	// Find the smallest x point
	for (int i = 1; i < num_points; i++)
	{
		if (points[i].x < smallest_x)
		{
			smallest_x = points[i].x;
			start = i;
		}
	}
	
	// Add the index of the smallest point to the vector
	hull_indices.push_back(start);

	// Process each point to form the hull
	for (int i = 0; i < num_points; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			// Check if J is inside the vector already
			if (j == hull_indices.back())
			{
				continue;
			}

			bool same_side = true;
			Point current = points[hull_indices.back()];

			// Check other points if they are on the same side
			for (int k = 0; k < num_points; k++)
			{
				if (j == k)
				{
					continue;
				}

				if (get_location(current.x, current.y, points[j].x, points[j].y, points[k].x, points[k].y).second == true)
				{
					same_side = false;
					break;
				}
			}

			if (same_side)
			{
				if (j == hull_indices.front())
				{
					return hull_indices;
				}

				hull_indices.push_back(j);
			}
		}
		
	}
	return hull_indices;
}
