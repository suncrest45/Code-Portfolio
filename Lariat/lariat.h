///-----------------------------------------------------------------------------//
/// @file   lariat.h
/// @brief  Definition of linked lists of arrays.
/// 
/// @author Aidan Straker (aidan.straker)
/// @date   February 2024
///
/// @copyright (c) 2024 DigiPen (USA) Corporation.
///-----------------------------------------------------------------------------//

#ifndef LARIAT_H
#define LARIAT_H

#include <string>  // error strings
#include <utility> // error strings
#include <cstring> // memcpy
#include <iostream>

class LariatException : public std::exception
{
private:
  int m_ErrCode;
  std::string m_Description;

public:
  LariatException(int errCode, const std::string& description) : m_ErrCode(errCode), m_Description(description) {}

  virtual int code(void) const { return m_ErrCode; }

  virtual const char* what() const throw() override { return m_Description.c_str(); }

  virtual ~LariatException() throw() override {}

  enum LARIAT_EXCEPTION{E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR};
};


template<typename T, int Size>
class Lariat;

template<typename T, int Size>
std::ostream& operator<<(std::ostream& os, Lariat<T, Size> const& rhs);

/// @brief Linked List of Arrays Class
/// @tparam T - the data type of the lariat.
/// @tparam Size - the logical size of the each array in a node.
template<typename T, int Size>
class Lariat
{
public:

  template<typename T3, int Size3>
  friend class Lariat;


  // Default constructor
  Lariat();

  // Copy constructor
  Lariat(Lariat const& rhs);
  // template copy constructor
  template<typename T2, int Size2>
  Lariat(Lariat<T2, Size2> const& rhs);

  // Destructor
  ~Lariat();

  // More constructors and assignments

  // Inserts
  void insert(int index, T const& value);
  void push_back(T const& value);
  void push_front(T const& value);
 
  // Erases
  void erase(int index);
  void pop_back();
  void pop_front();

  // Access
  T& operator[](int index);
  T const& operator[](int index) const;
  T& first();
  T const& first() const;
  T& last();
  T const& last() const;

  // Assignment
  Lariat& operator=(Lariat const& rhs);
  
  // template assignment operator.
  template<typename T2, int Size2>
  Lariat& operator=(Lariat<T2, Size2> const& rhs);

  unsigned find(const T& value) const;

  friend std::ostream& operator<< <>(std::ostream& os, Lariat const& list);

  size_t size() const { return m_Size; }
  void clear();
  void compact();
  

private:
  struct LNode
  {
    LNode* prev;
    LNode* next;
    int count = 0;
    T values[Size];

    void push_back(const T& value);
    T pop_back();
  };



  LNode* m_Head = nullptr;     // Points to the first node
  LNode* m_Tail = nullptr;     // Points to the last node
  int m_Size = 0;              // Number of items (not nodes) in the list
  mutable int m_NodeCount = 0; // Number of nodes int he list
  int m_ASize = Size;          // The Size of the array within the nodes.

  // Splits a node into two. 
  void split(LNode* node) const; 
  // Finds the node and local index of an element.
  std::tuple<LNode*, int> findElement(int globalIndex) const; 
  // Shifts an element up the list.
  const T* shiftUp(LNode* node, int index);
  // Shfits the entire array down.
  const T shiftDown(LNode* node, int index);
  // Swaps two elements.
  void swap(T& a, T& b); 
  // Deletes a node.
  void deleteNode(LNode* node);
  // Insert node at a position in the list.
  void insertNode(LNode* ptr, int list_index, T const& value);
  // Iterates through the lariat and gets the index of a node.
  int GetNodeIndex(LNode* ptr);
};



#include "lariat.cpp"
#endif //LARIAT_H
