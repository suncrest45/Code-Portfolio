#include "closestpair.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <iostream>
#include <utility>

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

///---------------------------------------------------------------------------
/// @brief Find the distance between two points
/// @param p1 - point 1
/// @param p2 - point 2
/// @return The distance between the two points (float)
///---------------------------------------------------------------------------
float distance(const Point& p1, const Point& p2)
{
	return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

///---------------------------------------------------------------------------
/// @brief Compare two y coordinates
/// @param p1 - The first point
/// @param p2 - The second point
/// @return Is the first y coordinate less than the second 
///---------------------------------------------------------------------------
bool compareY(const Point& p1, const Point& p2) { return p1.y < p2.y; }


///---------------------------------------------------------------------------
/// @brief A brute force method for finding the smallest distance
/// @param points - The set of points
/// @param left   - The start of the subset
/// @param right  - The end of the subset
/// @return The smallest distance between two points in the subset
///---------------------------------------------------------------------------
float bruteForce(std::vector<Point> const& points, int left, int right) 
{
    float minDist = std::numeric_limits<float>::max();
    for (int i = left; i < right; ++i) 
	{
        for (int j = i + 1; j < right; ++j) 
		{
            minDist = std::min(minDist, distance(points[i], points[j]));
        }
    }
    return minDist;
}

///---------------------------------------------------------------------------
/// @brief Finds the closest distance in a narrow vertical band/strip
/// @param strip - The vertical band
/// @param d 	 - The current minimum distance
/// @return The minimum distance between the points in a vertical band (float)
///---------------------------------------------------------------------------
float stripClosest(std::vector<Point>& strip, float d)
{
	// The current minimum distance
	float min_dist = d;

	// Sort the y values
	std::sort(strip.begin(), strip.end(), compareY);
	// Size of the band
	int size = strip.size();

	// Traverse the strip 
	for (int i = 0; i < size; i++)
	{
		// Get the next point in the strip
		// Since the x values are the same compare the y values
		for (int j = i + 1; j < size && (strip[j].y - strip[i].y) < min_dist; j++)
		{
			// Determine if the current minimum distance is smaller than
			// The distance between points i and j
			min_dist = std::min(min_dist, distance(strip[i], strip[j]));
		}
	}
	
	return min_dist;
}

/// @brief Find the smallest distance between any two points in the vector
/// @param points - The vector holding the points
/// @return The smallest distance between two points (float)
float closestPair_aux (std::vector<Point> const& points, int left, int right) 
{
	// If you have a small subset, use brute force
	if (right - left <= 3)
	{
		return bruteForce(points, left, right);
	}
	
	// The index of the mid point of this subset
	int mid = left + (right - left) / 2;
	

	// The mid point
	Point midPoint = points[mid];

	

	// Recursively find the smallest distance for the left half
	float dl = closestPair_aux(points, left, mid);
	// Recursively find the smallest distance for the right half
	float dr = closestPair_aux(points, mid, right);
	// The smallest between the two halves
	float d = std::min(dl, dr);

	// Build strip to hold points closest to the dividing line
	std::vector<Point> strip;

	for (int i = left; i < right; i++)
	{
		if (std::abs(points[i].x - midPoint.x) < d)
		{
			strip.push_back(points[i]);
		}
		
	}
	
	
	// Find the closest distance in the strip
	return std::min(d, stripClosest(strip, d));
}


float closestPair (std::vector<Point> const& points) 
{
	int size = points.size();

	if (size==0) throw "zero size subset";
	if (size==1) return std::numeric_limits<float>::max();

	// Sort points by x and y coordinates
    std::vector<Point> pointsX = points;
    sort(pointsX.begin(), pointsX.end(), [](Point a, Point b) { return a.x < b.x; });


	return closestPair_aux(pointsX, 0, points.size());
}