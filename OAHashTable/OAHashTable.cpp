///---------------------------------------------------------------------------------------------------------------------
/// @file OAHashTable.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)

/// @brief Develop an open-addressing-based hash table class that uses 
///        both linear probing (with ascending indexes) and double hashing 
///        to resolve collisions.

/// @version 0.1
/// @date 2024-03-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------------------------------------------------

#include "OAHashTable.h"

///--------------------------------OAHTSlot Function Definitions--------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// @brief Slot Default Constructor
/// @tparam T - The data type of the value in the pair
///---------------------------------------------------------------------------------------------------------------------
template<typename T>
OAHashTable<T>::OAHTSlot::OAHTSlot()
    : Key()
    , Data()
    , State(UNOCCUPIED)
    , probes(0)
{}

///---------------------------------------------------------------------------------------------------------------------
/// @brief OAHTSlot Non-Default Constructor
/// @tparam T   - The data type of the value in the pair
/// @param Key  - The Key for the slot
/// @param data - The client data associated with the key
///---------------------------------------------------------------------------------------------------------------------
template<typename T>
OAHashTable<T>::OAHTSlot::OAHTSlot(const char *Key, T data)
    : State(OCCUPIED)
    , probes(0)
    , Key()
    , Data(data)
{
    std::strncpy(this->Key, Key, MAX_KEYLEN);
}

///--------------------------------OAHashTable Function Definitions-----------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// @brief Non-Default Constructor
/// @tparam T     - Data type of the data in the pair
/// @param Config - Reference to another instance of OAHTConfig
///---------------------------------------------------------------------------------------------------------------------
template<typename T>
OAHashTable<T>::OAHashTable(const OAHashTable::OAHTConfig &Config)
        : m_table_config(Config)
        , m_table_stats()
        , m_Table(Config.m_initial_table_size)
{

    // Update the pointers to the primary and secondary hashing functions.
    m_table_stats.PrimaryHashFunc_ = m_table_config.m_primary_hash_func;
    m_table_stats.SecondaryHashFunc_ = m_table_config.m_secondary_hash_func;

    // Update the size of the table.
    m_table_stats.TableSize_ += m_table_config.m_initial_table_size;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Destructor
/// @tparam T - The data type of the data in the key/data pair
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
OAHashTable<T>::~OAHashTable()
{
    clear();
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Insert a new key/data pair into the Hash Table
/// @tparam T   - Data type of the data in the key/data pair
/// @param Key  - The key
/// @param Data - Client data associated with the key
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void OAHashTable<T>::insert(const char *Key, const T &Data)
{
    // If adding the key/data pair makes the load factor exceed the maximum
    if (LoadFactor(m_table_stats.Count_ + 1) > m_table_config.m_max_load_factor)
    {
        // Grow the table.
        GrowTable();
    }

    // Index of the first unoccupied or deleted slot in the table
    int emptyIndex = 0;
    // Get the index of the key
    int index = IndexOf(Key, emptyIndex);

    // If the index is not -1, this key already exists in the table
    if(index != -1)
    {
        // Duplicate found
        throw OAHashTableException(OAHashTableException::E_DUPLICATE, "Insert: Duplicate");
    }

    // Add the key/data pair to the table
    std::strncpy(m_Table[emptyIndex].Key, Key, MAX_KEYLEN - 1);
    m_Table[emptyIndex].Data = Data;
    m_Table[emptyIndex].State = OAHTSlot::OCCUPIED;
    m_table_stats.Count_++;
}

//----------------------------------------------------------------------------------------------------------------
/// @brief Removes an item by key. Throws an exception if the key doesn't exist.
///        Compacts the table by moving key/data pairs, if necessary
/// @param Key - The key of the pair to remove
//----------------------------------------------------------------------------------------------------------------
template<typename T>
void OAHashTable<T>::remove(const char *Key)
{
    int emptyIndex = 0;
    // Search for the key
    int index = IndexOf(Key, emptyIndex);

    // Index not found
    if(index == -1)
    {
        throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Key not in table.");
        return;
    }

    m_table_stats.Count_--;
    // If the deletion policy is marking just mark the slot as deleted
    if(m_table_config.m_oaht_deletion_policy == OAHTDeletionPolicy::MARK)
    {
        m_Table[index].State = OAHTSlot::DELETED;
        return;
    }

    // If the deletion policy is pack, we remove the slot and update the table
    std::vector<OAHTSlot> backup;
    m_Table[index].State = OAHTSlot::UNOCCUPIED;
    // Backup every slot in the table after the one to be deleted
    for (int i = index + 1; m_Table[i].State == OAHTSlot::OCCUPIED; i = (i + 1) % m_table_stats.TableSize_)
    {
         backup.push_back(m_Table[i]);
         m_Table[i].State = OAHTSlot::UNOCCUPIED;
         m_table_stats.Count_--;
    }

    // Add the backed up slots to the table
    for (OAHTSlot& slot : backup)
    {
        insert(slot.Key, slot.Data);
    }
}

//----------------------------------------------------------------------------------------------------------------
/// @brief Find and return the data in the table by key
/// @param Key - The key to find
/// @return The data or an exception if the key is not found (const T&)
//----------------------------------------------------------------------------------------------------------------
template<typename T>
const T &OAHashTable<T>::find(const char *Key) const
{
    int emptyIndex = 0;
    int index = IndexOf(Key, emptyIndex);

    if(index == -1)
    {
        throw OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found in table.");
    }
    else
    {
        return m_Table[index].Data;
    }
}

//----------------------------------------------------------------------------------------------------------------
/// @brief Removes all items from the table, but does not deallocate it
//----------------------------------------------------------------------------------------------------------------
template<typename T>
void OAHashTable<T>::clear()
{
    // Set every slot in the table to unoccupied
    for(OAHTSlot& slot : m_Table)
    {
        // Call the client-provided free function, if it exists
        if(slot.State == OAHTSlot::OCCUPIED && m_table_config.m_free_proc)
        {
            // Free the data associated with the key
            m_table_config.m_free_proc(slot.Data);
        }
        slot.State = OAHTSlot::UNOCCUPIED;
        slot.probes = 0;
    }
    // After clearing set count to zero
    m_table_stats.Count_ = 0;
}

//----------------------------------------------------------------------------------------------------------------
/// @brief Allow the client to peer into the table.
/// @return The statistical data of an OAHashTable (OAHSTStats)
//----------------------------------------------------------------------------------------------------------------
template<typename T>
OAHTStats OAHashTable<T>::GetStats() const { return m_table_stats; }

//----------------------------------------------------------------------------------------------------------------
/// @brief  Allow the client to see a slot in the table
/// @return The data of a slot in the table (OAHTSlot*)
//----------------------------------------------------------------------------------------------------------------
template<typename T>
const typename OAHashTable<T>::OAHTSlot *OAHashTable<T>::GetTable() const { return m_Table.data(); }

//----------------------------------------------------------------------------------------------------------------------
/// @brief Calculates the load factor of this hash table.
/// @tparam T - The data type of the data in the key/data pair.
/// @return The load factor of this hash table (double).
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
double OAHashTable<T>::LoadFactor(double num_elements) { return num_elements / m_table_stats.TableSize_; }


//----------------------------------------------------------------------------------------------------------------
/// @brief Expands the table when the load factor reaches a certain point
///        (greater than MaxLoadFactor) Grows the table by GrowthFactor,
///        making sure the new size is prime by calling GetClosestPrime
//----------------------------------------------------------------------------------------------------------------
template<typename T>
void OAHashTable<T>::GrowTable()
{
    double factor = std::ceil(m_table_stats.TableSize_ * m_table_config.m_growth_factor);

    // Get new prime size.
    unsigned new_size = GetClosestPrime(static_cast<unsigned>(factor));

    std::vector<OAHTSlot> localCopy = std::move(m_Table);
    m_Table.clear();

    // Resize the vector
    m_Table.resize(new_size);
    m_table_stats.Count_ = 0;

    // Update the table stats.
    m_table_stats.TableSize_ = static_cast<unsigned int>(new_size);
    m_table_stats.Expansions_++;

    for(OAHTSlot& slot : localCopy)
    {
        if(slot.State == OAHTSlot::OCCUPIED)
        {
            insert(slot.Key, slot.Data);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------
/// @brief Workhorse method to locate an item (if it exists)
/// @param Key  - The key to find
/// @param Slot - Pointer to address of the slot in the table of the key
/// @return Index if it exists, -1 if not (int)
//----------------------------------------------------------------------------------------------------------------
template<typename T>
int OAHashTable<T>::IndexOf(const char *Key, int& emptyIndex) const
{

    // Compute the hash value which will serve as the index
    unsigned hashValue = m_table_config.m_primary_hash_func(Key, m_table_stats.TableSize_);;

    int index = static_cast<int>(hashValue);

    // Stride size for linear probing or double hashing
    unsigned stride = 1;

    int loopCount = 0;

    // Only calculate stride if there is a collision.
    if(
        (
            m_Table[index].State == OAHTSlot::DELETED ||
            (
                m_Table[index].State == OAHTSlot::OCCUPIED &&
                std::strncmp(m_Table[index].Key, Key, MAX_KEYLEN) != 0
            )
        )
        && m_table_config.m_secondary_hash_func
    )
    {
        stride = 1 + m_table_config.m_secondary_hash_func(Key, m_table_stats.TableSize_ - 1);
    }

    emptyIndex = -1;

    while (true)
    {
        if(loopCount > static_cast<int>(m_table_stats.Count_))
        {
            return -1;
        }
        m_Table[index].probes++;
        m_table_stats.Probes_++;
        if (m_Table[index].State == OAHTSlot::UNOCCUPIED)
        {
            if (emptyIndex == -1)
            {
                emptyIndex = index;
            }
            // Key not found
            return -1;
        }
        else if (m_Table[index].State == OAHTSlot::DELETED)
        {
            if (emptyIndex == -1)
            {
                emptyIndex = index;
            }
        }
        else if (m_Table[index].State == OAHTSlot::OCCUPIED && std::strcmp(m_Table[index].Key, Key) == 0)
        {
            // Key found, return index

            return index;
        }

        index = (index + stride) % m_table_stats.TableSize_;
        loopCount++;
    }
}
