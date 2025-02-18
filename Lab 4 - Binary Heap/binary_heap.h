#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <tuple>
#include <iostream> // debugging only
#include <algorithm>
#include <utility>

// key   - binary heap
// value - actual data 

// forward declaration for 1-1 operator<< 
template < typename T_key, typename T_value >
class BinaryHeap;

template < typename T_key, typename T_value >
std::ostream& operator<< (std::ostream& os, BinaryHeap<T_key, T_value> const & bh);


template < typename T_key, typename T_value >
class BinaryHeap
{
    private:
        // indexing starts at 0 - i.e. minimum element is at index 0
        // modify left/right/parent implementations to support that
        std::vector< std::tuple<T_key,T_value> > data = {};
    public:
        BinaryHeap() = default;
        BinaryHeap( std::vector< std::tuple<T_key,T_value> > && data2 ) : data( std::move( data2 ) ) 
        {
            init_heap();
        }
        BinaryHeap( BinaryHeap const& ) = default; // vector copy is OK
        BinaryHeap( BinaryHeap && ) = default; // vector move is OK
        BinaryHeap& operator=( BinaryHeap const& ) = default; // vector assignment is OK
        BinaryHeap& operator=( BinaryHeap && ) = default; // vector move assignment is OK

        friend
        std::ostream& operator<< (std::ostream& os, BinaryHeap<T_key, T_value> const & bh)
        {
            for ( std::tuple< T_key,T_value> const& el : bh.data ) 
            {
                os << std::get<0>( el ) << " "; // does not print values
            }
            return os;
        }

        /// @brief Add a new key/value pair to the heap
        /// @param t - The key/value pair to add
        void push(std::tuple<T_key,T_value> t)
        {
           // Insert key at the end
            data.push_back(t);
            int index = data.size() - 1;

            // Fix violated heap
            heapifyUp(index);
        }

        /// @brief Move a new key/value pair to the heap
        /// @param t - The key/value pair to add
        void push(std::tuple<T_key,T_value> && t)
        {
            // Insert key at the end
            data.push_back(t);
            int index = data.size() - 1;

            // Fix violated heap
            heapifyUp(index);
        }

        /// @brief Remove the first element in the list
        void pop() 
        {
            
            int index = size() - 1;
            std::swap(data[0], data[index]);
            data.pop_back();
            heapifydown(0);
        }

        /// @brief  The amount of elements in the heap
        /// @return The amount of elements in the heap
        int size() const { return data.size(); }

        /// @brief Get the key/value pair at the top of the heap
        /// @return The key/value pair at the top of the tree
        std::tuple<T_key,T_value> const& top() const 
        {
            // If the heap is not empty, return the first item in the heap
            return data[0];
        }

        /// @brief Decrease the key of a specified element
        /// @param index   - The index of the element
        /// @param new_key - The new key value
        void decrease_key(int index, T_key new_key) // bubble up
        {
            // Insert the new key
            std::get<0>(data[index]) = new_key;
            // Maintain the heap property
            heapifyUp(index);
        }

    private:

        /// @brief Get the index of the left child of the specified node
        /// @param i - the index of the node
        /// @return The index of the left child
        size_t left  (size_t i) const { return 2 * i + 1;   }

        /// @brief Get the index of the right child of the specified node
        /// @param i - The index of the specified node
        /// @return The index of the left child
        size_t right (size_t i) const { return 2 * i + 2;   }

        /// @brief Get the index of the parent of the specified node
        /// @param i - The index of the specified node
        /// @return The index of the parent
        size_t parent(size_t i) const { return (i - 1) / 2; }


        /// @brief Ensures that a newly created heap maintains the heap property
        void init_heap() 
        {
            // Iterate through the vector
            for (int i = data.size() / 2 - 1; i >= 0; --i)
            {
                // Ensure all elements follow the heap property
                heapifydown(i);
            }
        }

        /// @brief After adding or removing an element enforce the heap rule
        /// @param index - The index to enforce from
        void heapifydown(size_t index)
        {
            // Get the index of the left child
            size_t leftChild = left(index);
            // Get the index of the right child
            size_t rightChild = right(index);

            // Let the passed index serve as the "smallest"
            size_t smallest = index;

            // Check if the left child is smaller than the "smallest"
            if (leftChild < data.size() && std::get<0>(data[leftChild]) < std::get<0>(data[smallest]))
            {
                smallest = leftChild;
            }

            // Check if the right child is smaller than the "smallest"
            if (rightChild < data.size() && std::get<0>(data[rightChild]) < std::get<0>(data[smallest]))
            {
                smallest = rightChild;
            }

            if (smallest != index)
            {
                std::swap(data[smallest], data[index]);
                heapifydown(smallest);
            }
        }

        /// @brief Maintain the heap property by sorting a value upwards
        /// @param index - The position in the heap we are heapifying
        void heapifyUp(size_t index)
        {
            // Enforce heap property from specified position in the heap
            while (index != 0 && std::get<0>(data[parent(index)]) > std::get<0>(data[index]))
            {
                std::swap(data[parent(index)], data[index]);
                index = parent(index);
            }
        }
};

#endif
