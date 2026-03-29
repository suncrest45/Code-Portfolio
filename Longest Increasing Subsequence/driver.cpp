#include "lis.h"
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cstdio> //sscanf

void print_subsequence( std::vector<unsigned> const& answer, std::vector<int> const& sequence ) {
    for ( unsigned i=0; i<answer.size(); ++i ) {
        std::cout << sequence[ answer[i] ] << " ";
    }
    std::cout << std::endl;
}

void test0 () {
    //4 1 3 1 5 0
    std::vector<int> sequence;
    sequence.push_back( 4 ); sequence.push_back( 1 ); sequence.push_back( 3 );
    sequence.push_back( 1 ); sequence.push_back( 5 ); sequence.push_back( 0 );

    std::vector<unsigned> answer = longest_increasing_subsequence( sequence );
    std::cout << "Subsequence size " << answer.size() << std::endl;
    print_subsequence( answer, sequence );
}

#include <ctime>   //std::time
#include <cstdlib> //std::srand, std::rand

//create a sequence of total_size with LIS of length size
void create_lis_of_size( int size, int total_size, std::vector<int>& sequence ) {
    int up   = -1; //longest inscresing sequence is going to be 0 1 2 .. size-1
    int down =  size; //all other numbers in decreasing order starting with size  
                      //spliced with the above sequence
    int total_size_curr = 0;
    sequence.reserve( total_size );
    std::srand ( std::time( NULL ) );
    for ( int i=0; i<size; ++i ) {
        int range_size = 2* ( std::rand()%(total_size/size) );
        if ( total_size_curr + range_size + size - i > total_size ) {
            range_size = total_size - total_size_curr - size + i;
        }
        total_size_curr += range_size;
        for ( int r=0; r<range_size; ++r ) {
            sequence.push_back( --down );
        }
        sequence.push_back( ++up );
        ++total_size_curr;
    }
    for ( ; total_size_curr < total_size; ++total_size_curr ) {
        sequence.push_back( --down );
    }
}

void test1 () {
    std::vector<int> sequence;
    create_lis_of_size( 50, 700, sequence );
    //std::cout << sequence.size() << ": ";
    //std::copy( sequence.begin(), sequence.end(), std::ostream_iterator<int>( std::cout, " " ) );
    //std::cout << "=============================\n";
    std::vector<unsigned> answer = longest_increasing_subsequence( sequence );
    std::cout << "Subsequence size " << answer.size() << std::endl;
    print_subsequence( answer, sequence );
}
    
void test2 () {
    std::vector<int> sequence;
    create_lis_of_size( 1500, 7000, sequence );
    std::vector<unsigned> answer = longest_increasing_subsequence( sequence );
    std::cout << "Subsequence size " << answer.size() << std::endl;
    print_subsequence( answer, sequence );
}

void test3 () {
    std::vector<int> sequence;
    create_lis_of_size( 5000, 35000, sequence );
    std::vector<unsigned> answer = longest_increasing_subsequence( sequence );
    std::cout << "Subsequence size " << answer.size() << std::endl;
    print_subsequence( answer, sequence );
}

void (*pTests[])(void) = { test0, test1, test2, test3 };

int main( int argc, char ** argv) {
	if (argc!=2) return 1;
	else {
		int test = 0;
		std::sscanf(argv[1],"%i",&test);
		pTests[ test ]();
	}
	return 0;
}
