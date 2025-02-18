///-----------------------------------------------------------------------------//
/// @file   lariat.cpp
/// @brief  Implementation of linked lists of arrays.
///
/// Class 280A
///
/// @author Aidan Straker (aidan.straker)
/// @date   February 2024
///
/// @copyright (c) 2024 DigiPen (USA) Corporation.
///-----------------------------------------------------------------------------//

#include <iostream>
#include <iomanip>
#include "lariat.h"

#if 1

///-----------------------------------------------------------------------------//
/// @brief Add an element to the back of a node.
/// @param value
///-----------------------------------------------------------------------------//
template <typename T, int Size>
void Lariat<T, Size>::LNode::push_back(const T &value)
{
  // If adding the element would exceed array size throw.
  if (this->count + 1 > Size)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  // Add the value to the array.
  this->values[this->count++] = value;
}

///-----------------------------------------------------------------------------//
/// @brief Remove an element from the back of a node.
/// @return The removed value (T).
///-----------------------------------------------------------------------------//
template <typename T, int Size>
T Lariat<T, Size>::LNode::pop_back()
{
  // If the node's array is empty.
  if (this->count <= 0)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  return this->values[--this->count];
}

//----------------------------------------------------------------------------
/// @brief Shift all the elements in the given node down
/// @param node - The node to shift.
/// @return The overflow from shifting the elements.
//----------------------------------------------------------------------------
template <typename T, int Size>
const T Lariat<T, Size>::shiftDown(LNode *node, int index)
{
  // If the node is not null.
  if (node)
  {
    // Shift all elements in the array to the left.
    for (int i = index; i < node->count - 1; i++)
    {
      std::swap(node->values[i], node->values[i + 1]);
    }

    node->count--;
  }

  return node->values[index];
}

//----------------------------------------------------------------------------
/// @brief Inserts a value at the end of the list
/// @param value - Value to insert at the end of the list.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::push_back(T const &value)
{
  if (!m_Tail)
  {
    push_front(value);
    return;
  }
  m_Size++;
  // If the list is full, split the node.
  if (m_Tail->count == m_ASize)
  {

    split(m_Tail);
    // Update the node.
    m_Tail = m_Tail->next;
    m_Tail->push_back(value);
  }
  else
  {
    insertNode(m_Tail, m_Tail->count, value);
  }
}

//----------------------------------------------------------------------------
/// @brief Inserts a value at the front of the list.
/// @param value - The value to insert at the end of the list.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::push_front(T const &value)
{
  // If the list is empty.
  if (!m_Head)
  {
    m_Head = new LNode();
    m_NodeCount++;
    m_Tail = m_Head;
  }

  // Insert the node.
  insertNode(m_Head, 0, value);

 
  if (!m_Tail)
  {
    m_Tail = m_Head;
  }
  

  // Update the tail.
  while (m_Tail->next != nullptr)
  {
    m_Tail = m_Tail->next;
  }
  m_Size++;
}

//----------------------------------------------------------------------------
/// @brief Removes an element from the list.
/// @param index - the global index of the element.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::erase(int index)
{
  // If the index is out of range, throw an exception.
  if (index < 0 || index >= m_Size)
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");

  // If the index is the size, pop the value off the back
  if (index == m_Size)
  {
    pop_back();
    return;
  }
  // If the index is 0, pop the value off the front
  if (index == 0)
  {
    pop_front();
    return;
  }

  // Find the element at the given index
  int localIndex = 0;
  LNode *node;
  std::tie(node, localIndex) = findElement(index);
  shiftDown(node, localIndex);
  m_Size--;
}

//----------------------------------------------------------------------------
/// @brief Removes an element from the back of the list.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::pop_back()
{
  // Make sure the tail is valid.
  if (!m_Tail)
  {
    return;
  }

  // Update statistics.
  m_Tail->count--;
  this->m_Size--;

  // If the tail becomes empty delete it.
  if (m_Tail->count <= 0)
  {
    // If there is more thaan one node left.
    if (m_NodeCount > 1)
    {
      m_Tail = m_Tail->prev;
      deleteNode(m_Tail->next);
    }
    else
    {
      deleteNode(m_Tail);
      m_Tail = nullptr;
      m_Head = nullptr;
    }
  }
}

//----------------------------------------------------------------------------
/// @brief Removes the first element in the list.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::pop_front()
{
  // Check if the head is valid.
  if (!m_Head)
  {
    return;
  }

  // Shift all elements in the head's array down.
  shiftDown(m_Head, 0);
  m_Size--;

  // Check if the head is empty.
  if (m_Head->count <= 0)
  {
    LNode *temp = m_Head;
    if (m_Head->next)
    {
      m_Head = m_Head->next;
      m_Head->prev = nullptr;
    }
    deleteNode(temp);
  }
}

//----------------------------------------------------------------------------
/// @brief Clears all the data in the list.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::clear()
{
  LNode *node = m_Head;
  // Traverse the list
  while (node != nullptr)
  {
    // Clear all the data in the node
    LNode *next = node->next;
    delete node;
    node = next;
  }
  // Reset the head and tail
  m_Head = nullptr;
  m_Tail = nullptr;
  // Reset the list statistics
  m_Size = 0;
  m_NodeCount = 0;
  m_ASize = Size;
}

//----------------------------------------------------------------------------
/// @brief Uses as few nodes as possible to store the same amount of data.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::compact()
{

  // If the list does not exist.
  if (!m_Head)
  {
    return;
  }

  // Temporary pointer to the head of the list.
  LNode *current = m_Head;
  // This will be used to point to the next node n the list.
  LNode *next = m_Head;
  // Count of the elements in the next node.
  int nextNodeCount = 0;

  // Traverse the list.
  while (true)
  {
    // Pointer to the next node.
    next = next->next;

    // If there is no next pointer that is the end of the list.
    if (next == nullptr)
    {
      break;
    }
    // Save the amount of elements in the node.
    nextNodeCount = next->count;
    next->count = 0;

    // If the current node is full move on.
    if (current->count == m_ASize)
    {
      current = current->next;
    }

    // Move the values from the next node to the current.
    for (int i = 0; i < nextNodeCount; i++)
    {
      current->push_back(next->values[i]);
      // Check if the count is full after each.
      if (current->count == m_ASize)
      {
        current = current->next;
      }
    }
  }

  // For the purpose of cleaning up the list.
  LNode *cleanup = m_Head;
  // Traverse the list.
  while (cleanup)
  {
    // If the node has no elements.
    if (cleanup->count == 0)
    {
      LNode *temp = cleanup;
      cleanup = cleanup->next;
      deleteNode(temp);
      continue;
    }
    cleanup = cleanup->next;
  }

  // Update the tail.
  m_Tail = m_Head;
  while (m_Tail->next != nullptr)
  {
    m_Tail = m_Tail->next;
  }
}

//----------------------------------------------------------------------------
/// @brief Splits a full node in two
/// @param node  - The node to split.
/// @param index - Where to split the node.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::split(LNode *node) const
{
  // New node to split the original into.
  LNode *split_node = new LNode();

  // Swap the next pointers
  std::swap(split_node->next, node->next);

  // Update list.
  node->next = split_node;
  split_node->prev = node;

  // Add one to the node, to account for the new value being added.
  // Add one again to force it to round up.
  int index = (node->count + 1 + 1) / 2;

  // Move the values at and after the index to the new node.
  for (int i = index; i < m_ASize; i++)
  {
    split_node->values[i - index] = node->values[i];
    ++split_node->count;
  }

  // Update the original node's count;
  node->count = index;

  // Update previous pointer of next node.
  if (split_node->next != nullptr)
  {
    split_node->next->prev = split_node;
  }
  m_NodeCount++;
}

//----------------------------------------------------------------------------
/// @brief Finds the node and local index of an element in the list.
/// @param globalIndex - the global index of the element.
/// @return The node and local index of an element in the list (std::tuple).
//----------------------------------------------------------------------------
template <typename T, int Size>
std::tuple<typename Lariat<T, Size>::LNode *, int> Lariat<T, Size>::findElement(int globalIndex) const
{
  LNode *current = nullptr;

  if (m_Head)
  {
    current = m_Head;
  }

  int count = 0;

  // Traverse the lariat, keeping a tally of all elements until global index is reached
  while (current->count + count <= globalIndex && current != m_Tail)
  {
    count += current->count;
    current = current->next;
  }

  // The difference between the global index and the tally
  int difference = globalIndex - count;

  // If the difference is within the bounds of the lariat.
  if (difference >= 0)
  {
    return std::make_tuple(current, difference);
  }

  return std::make_tuple(nullptr, -1);
}

//----------------------------------------------------------------------------
/// @brief Shfits all the elements in the list up.
/// @param node  - the index of the element inside the node.
/// @param index - the local index of the element.
//----------------------------------------------------------------------------
template <typename T, int Size>
const T *Lariat<T, Size>::shiftUp(LNode *node, int index)
{
  // Index out of bounds.
  if (index < 0 || index > m_ASize)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  if (node)
  {
    // Swap the values in the array in ascending order.
    for (int i = index; i < m_ASize; ++i)
    {
      std::swap(node->values[index], node->values[i]);
    }
  }

  return &(node)->values[index];
}

//----------------------------------------------------------------------------
/// @brief Swaps two values
/// @param a - the value to swap.
/// @param b - the value to swap.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::swap(T &a, T &b)
{
  T temp = a;
  a = b;
  b = temp;
}

//----------------------------------------------------------------------------
/// @brief Deletes a specified node.
/// @param node - the node to be deleted.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::deleteNode(LNode *node)
{
  // Prevent double free.
  if (!node)
  {
    return;
  }

  // If there is a previous node.
  if (node->next != nullptr)
  {
    node->next->prev = node->prev;
  }

  // If there is a next node.
  if (node->prev != nullptr)
  {
    node->prev->next = node->next;
  }

  // Delete the node.
  delete node;
  node = nullptr;
  // Update the amount of nodes in the list.
  this->m_NodeCount--;
}

//----------------------------------------------------------------------------
/// @brief Inserts a node in the list at a given position
/// @param ptr        - the node to insert.
/// @param list_index - the position to insert the node at.
/// @param value      - the value to insert int he node.
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::insertNode(LNode *ptr, int list_index, T const &value)
{
  // Check Lariat boundaries.
  if (list_index < 0 || list_index > Size)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  // If the node is full.
  if (ptr->count == m_ASize)
  {

    T copy;
    // Shift all elements to the right, saving the overflow.
    const T *overflow = shiftUp(ptr, list_index);

    if (overflow)
    {
      copy = *overflow;
    }

    // Insert the value.
    ptr->values[list_index] = value;
    // Split the node.
    split(ptr);

    // If there is overflow.
    if (overflow)
    {
      // Add the overflow to the new node.
      ptr->next->push_back(copy);
    }

    // Update tail
    if (ptr == m_Tail)
    {
      m_Tail = ptr->next;
    }
  }
  else
  {
    // Shift the elements to right.
    shiftUp(ptr, list_index);
    // Add the element to the node's array.
    ptr->values[list_index] = value;
    ptr->count++;
  }
}

//----------------------------------------------------------------------------
/// @brief Iterates through the lariat and gets the index of a node.
/// @param ptr - the node to find.
/// @return The index of the node (unsigned).
//----------------------------------------------------------------------------
template <typename T, int Size>
int Lariat<T, Size>::GetNodeIndex(LNode *ptr)
{
  int index = 0;
  LNode *current = m_Head;

  // Traverse the list.
  while (current)
  {
    // If the nod is found
    if (current == ptr)
    {
      break;
    }
    index++;
    current = current->next;
  }
  return index;
}

//----------------------------------------------------------------------------
/// @brief [] Overload for the Lariat class
/// @param index - index of the value to find
/// @return The element at the index.
//----------------------------------------------------------------------------
template <typename T, int Size>
T &Lariat<T, Size>::operator[](int index)
{
  // Check for bad index.
  if (index < 0 || index > m_Size)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  // Find the node the element is in.
  int localIndex = 0;
  LNode *node;
  // You can use tie to seperate the data from a tuple.
  std::tie(node, localIndex) = findElement(index);

  return node->values[localIndex];
}

//----------------------------------------------------------------------------
/// @brief [] Overload for the Lariat class (const version).
/// @param index - index of the value to find
/// @return The element at the index (T&).
//----------------------------------------------------------------------------
template <typename T, int Size>
T const &Lariat<T, Size>::operator[](int index) const
{
  // Check for bad index.
  if (index < 0 || index > m_Size)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  // Find the node the element is in.
  int localIndex = 0;
  LNode *node;
  // You can use tie to seperate the data from a tuple.
  std::tie(node, localIndex) = findElement(index);

  return node->values[localIndex];
}

//----------------------------------------------------------------------------
/// @brief  Get the first element in the list
/// @return The first element in the list
//----------------------------------------------------------------------------
template <typename T, int Size>
T &Lariat<T, Size>::first() { return m_Head->values[0]; }

//----------------------------------------------------------------------------
/// @brief  Get the first element in the list (const version)
/// @return The first element in the list (const version)
//----------------------------------------------------------------------------
template <typename T, int Size>
T const &Lariat<T, Size>::first() const { return m_Head->values[0]; }

//----------------------------------------------------------------------------
/// @brief  Get the last element in the list
/// @return The last element in the list
//----------------------------------------------------------------------------
template <typename T, int Size>
T &Lariat<T, Size>::last() { return m_Tail->values[m_ASize]; }

//----------------------------------------------------------------------------
/// @brief  Get the last element in the list (const version)
/// @return The last element in the list (const version)
//----------------------------------------------------------------------------
template <typename T, int Size>
T const &Lariat<T, Size>::last() const { return m_Tail->values[m_ASize]; }

//----------------------------------------------------------------------------
/// @brief Assignment Operator Overload.
/// @param rhs - Reference to a lariat instance.
/// @return A new lariat insatnce.
//----------------------------------------------------------------------------
template <typename T, int Size>
Lariat<T, Size> &Lariat<T, Size>::operator=(Lariat<T, Size> const &rhs)
{
  //  Check for self assignment.
  if (this == &rhs)
  {
    return *this;
  }

  // Clear all data in the list
  clear();

  // Copy all elements in the other lariat.
  for (int i = 0; i < rhs.m_Size; i++)
  {
    push_back(rhs[i]);
  }

  return *this;
}

//----------------------------------------------------------------------------
/// @brief Assignment operator overload.
/// @tparam T     - The type of data being stored in the data structure.
/// @tparam Size  - Logical size of the arrays within each node.
/// @tparam T2    - The type of data being stored in the other Lariat.
/// @tparam Size2 - Logical size of the arrays within the nodes of the other Lariat.
/// @param rhs    - Reference to another Lariat of a different type and size.
/// @return a reference to a Lariat<T, Size>.
//----------------------------------------------------------------------------
template <typename T, int Size>
template <typename T2, int Size2>
Lariat<T, Size> &Lariat<T, Size>::operator=(Lariat<T2, Size2> const &rhs)
{

  // Clear all data in the list
  clear();

  // Copy all elements in the other lariat.
  for (int i = 0; i < rhs.m_Size; i++)
  {
    push_back(static_cast<T>(rhs[i]));
  }

  return *this;
}

//----------------------------------------------------------------------------
/// @brief Finds the index of an element in the list
/// @return The index of an element in the list or the amount of elements if not found
//----------------------------------------------------------------------------
template <typename T, int Size>
unsigned Lariat<T, Size>::find(const T &value) const
{
  LNode *node = m_Head;
  int index = 0;
  // Traverse the list.
  while (node != m_Tail->next)
  {
    // Search the node's array
    for (int i = 0; i < node->count; i++)
    {
      // If the value is found, return the index
      if (node->values[i] == value)
      {
        return index;
      }
      index++;
    }
    node = node->next;
  }
  // If not found return the amount elements in the lariat.
  return m_Size;
}

//----------------------------------------------------------------------------
/// @brief Copy Constructor for the Lariat class (own type)
/// @tparam T    - The type of data being stored in the data structure.
/// @tparam Size - Logical size of the arrays within each node.
/// @param rhs   - A reference to an instance of the Lariat class.
//----------------------------------------------------------------------------
template <typename T, int Size>
Lariat<T, Size>::Lariat(Lariat const &rhs)
    : m_Head(nullptr), m_Tail(nullptr), m_Size(0), m_NodeCount(0), m_ASize(Size)
{
  // Copy all elements in the other lariat.
  for (int i = 0; i < rhs.m_Size; i++)
  {
    push_back(rhs[i]);
  }
}

//-------------------------------------------------------------------------------------
/// @brief A copy constructor for Lariats of differen types and sizes.
/// @tparam T      - The type of data being stored in the data structure.
/// @tparam U      - The type of data being stored in the reference data structure.
/// @tparam Size   - Logical size of the arrays within each node.
/// @tparam Length - Logical size of the arrays within each node of the reference.
/// @param rhs     - Reference to an instance of the Lariat class.
//-------------------------------------------------------------------------------------
template <typename T, int Size>
template <typename T2, int Size2>
Lariat<T, Size>::Lariat(Lariat<T2, Size2> const &rhs)
    : m_Head(nullptr), m_Tail(nullptr), m_Size(0), m_NodeCount(0), m_ASize(Size)
{
  // Copy all elements from the other lariat.
  for (int i = 0; i < rhs.m_Size; i++)
  {
    push_back(static_cast<T>(rhs[i]));
  }
}

//----------------------------------------------------------------------------
/// @brief Destructor for the lariat class
/// @tparam T    - The type of data being stored in the data structure.
/// @tparam Size - Logical size of the arrays within each node.
//----------------------------------------------------------------------------
template <typename T, int Size>
Lariat<T, Size>::~Lariat()
{
  // Delete all the data used in the linked list.
  clear();
}

//----------------------------------------------------------------------------
/// @brief Insert a value into the list at a specific index
/// @param index - The global index of the element
/// @param value - The value to insert
//----------------------------------------------------------------------------
template <typename T, int Size>
void Lariat<T, Size>::insert(int globalIndex, T const &value)
{
  // If the index is out of range, throw an exception.
  if (globalIndex < 0 || globalIndex > m_Size)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  // If the index is 0, push the value to the front
  if (globalIndex == 0)
  {
    push_front(value);
    return;
  }

  // If the index is the size, push the value to the back
  if (globalIndex == m_Size)
  {
    push_back(value);
    return;
  }

  // Index for the element inside the node.
  int NodeIndex = 0;
  // Node containing element.
  LNode *node;
  // Get the node and local index to insert at.
  std::tie(node, NodeIndex) = findElement(globalIndex);

  // Insert a new node containing this value.
  insertNode(node, NodeIndex, value);
  // Increment number of elements in the list.
  m_Size++;
}

//----------------------------------------------------------------------------
/// @brief Constructor for the Lariat class
/// @tparam T    - The type of data being stored in the data structure.
/// @tparam Size - Logical size of the arrays within each node.
//----------------------------------------------------------------------------
template <typename T, int Size>
Lariat<T, Size>::Lariat()
    : m_Head(nullptr), m_Tail(nullptr), m_Size(0), m_NodeCount(0), m_ASize(Size)
{
}

template <typename T, int Size>
std::ostream &operator<<(std::ostream &os, Lariat<T, Size> const &list)
{
  typename Lariat<T, Size>::LNode *current = list.m_Head;
  int index = 0;
  while (current)
  {
    os << "Node starting (count " << current->count << ")\n";
    for (int local_index = 0; local_index < current->count; ++local_index)
    {
      os << index << " -> " << current->values[local_index] << std::endl;
      ++index;
    }
    os << "-----------\n";
    current = current->next;
  }
  return os;
}

#else // fancier
#endif
