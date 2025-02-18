/// --------------------------------------------------------------------------
/// @file OAHashTable.h
/// @author Aidan Straker (aidan.straker@digipen.edu)

/// @brief Develop an open-addressing-based hash table class that uses 
///        both linear probing (with ascending indexes) and double hashing 
///        to resolve collisions.

/// @version 0.1
/// @date 2024-03-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef OAHASHTABLEH
#define OAHASHTABLEH
//---------------------------------------------------------------------------
#include <string>    // std::string
#include <utility>   // std::move
#include <cstring>   // std::strncpy
#include <vector>    // std::vector
#include "Support.h" // GetClosestPrime
#include <cmath>     // std::ceil

//---------------------------------------------------------------------------
/// @brief Client-provided function
/// @param const char* - key
/// @param unsigned - table size
/// @return An index in the table (unsigned).
//---------------------------------------------------------------------------
typedef unsigned (*HASHFUNC)(const char *, unsigned);

/// @brief Maximum length of the the string keys
const unsigned MAX_KEYLEN = 32;

/// @brief The exception class for our Hash Table
class OAHashTableException
{
  private:
    /// @brief Code for the exception
    int error_code_;
    /// @brief Readable string describing the exception
    std::string message_;
  public:

    //---------------------------------------------------------------------------
    /// @brief Non-default Constructor
    /// @param ErrCode - The error code
    /// @param Message - Message associated with the error code
    //---------------------------------------------------------------------------
    OAHashTableException(int ErrCode, std::string Message)
    : error_code_(ErrCode)
    , message_(std::move(Message))
    {};

    /// @brief Destructor
    virtual ~OAHashTableException() = default;

    /// @brief Retrieves an exception code
    /// @return One of: E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY
    virtual int code() const { return error_code_; }


    /// @brief Retrieve human-readable string describing the exception
    /// @return The description of the exception (const char*)
    virtual const char *what() const { return message_.c_str(); }

    /// @brief Possible exception conditions
    enum OAHASHTABLE_EXCEPTION {E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY};
};

/// @brief The policy used during a deletion
enum OAHTDeletionPolicy {MARK, PACK};

/// @brief OAHashTable statistical info
struct OAHTStats
{
  /// @brief Default Constructor
  OAHTStats() : Count_(0), TableSize_(0), Probes_(0), Expansions_(0), PrimaryHashFunc_(nullptr), SecondaryHashFunc_(nullptr) {};

  /// @brief Number of elements in the table
  unsigned Count_;
  /// @brief Size of the (total slots)
  unsigned TableSize_;
  /// @brief Number of probes performed
  mutable unsigned Probes_;
  /// @brief Number of times the table grew
  unsigned Expansions_;
  /// @brief Pointer to the primary hash function
  HASHFUNC PrimaryHashFunc_;
  /// @brief Pointer to the secondary hash function
  HASHFUNC SecondaryHashFunc_;
};

/// @brief Hash table definition (open-addressing)
/// @tparam T - data type
template <typename T>
class OAHashTable
{
  public:

    /// @brief Client-provided free proc (we own the data)
    typedef void (*FREEPROC)(T);

    /// @brief Configuration for the hash table.
    struct OAHTConfig
    {
      //----------------------------------------------------------------------------------------------------------------
      /// @brief Non-default Constructor
      /// @param InitialTableSize  - The starting table size
      /// @param PrimaryHashFunc   - First hash function
      /// @param SecondaryHashFunc - Hash function resolve collisions
      /// @param MaxLoadFactor     - Maximum LF before growing
      /// @param GrowthFactor      - The amount to grow the table
      /// @param Policy            - MARK or PACK
      /// @param FreeProc          - Client-provided free function
      //----------------------------------------------------------------------------------------------------------------
      OAHTConfig(unsigned InitialTableSize, HASHFUNC PrimaryHashFunc, HASHFUNC SecondaryHashFunc = nullptr,
                 double MaxLoadFactor = 0.5, double GrowthFactor = 2.0, OAHTDeletionPolicy Policy = PACK,
                 FREEPROC FreeProc = 0) :

              m_initial_table_size(InitialTableSize), m_primary_hash_func(PrimaryHashFunc),
              m_secondary_hash_func(SecondaryHashFunc), m_max_load_factor(MaxLoadFactor),
              m_growth_factor(GrowthFactor), m_oaht_deletion_policy(Policy), m_free_proc(FreeProc) {}

      /// @brief The starting size of the table
      unsigned m_initial_table_size;
      /// @brief The hash function used in all cases
      HASHFUNC m_primary_hash_func;
      /// @brief The hush function used in double hashing, if null then linear probing will be used
      HASHFUNC m_secondary_hash_func;
      /// @brief Maximum LF before growing
      double m_max_load_factor;
      /// @brief The amount to grow the table
      double m_growth_factor;
      /// @brief MARK or PACK
      OAHTDeletionPolicy m_oaht_deletion_policy;
      /// @brief Client-provided free function
      FREEPROC m_free_proc;
    };
      
    /// @brief Slots that will hold the key/data pairs
    struct OAHTSlot
    {
      /// @brief The 3 possible states the slot can be in
      enum OAHTSlot_State {OCCUPIED, UNOCCUPIED, DELETED};

      /// @brief Key is a string
      char Key[MAX_KEYLEN];
      /// @brief Client data
      T Data;
      /// @brief The state of the slot
      OAHTSlot_State State;
      /// @brief For testing
      mutable int probes;

      //----------------------------------------------------------------------------------------------------------------
      /// @brief Default Constructor
      //----------------------------------------------------------------------------------------------------------------
      OAHTSlot();

      //----------------------------------------------------------------------------------------------------------------
      /// @brief Non-default Constructor
      /// @param Key  - The key
      /// @param data - The data
      //----------------------------------------------------------------------------------------------------------------
      OAHTSlot(const char* Key, T data);
    };

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Non-Default Constructor
    /// @param Config - Reference to an instance of OAHTConfig
    //----------------------------------------------------------------------------------------------------------------
    explicit OAHashTable(const OAHTConfig& Config);

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Destructor
    //----------------------------------------------------------------------------------------------------------------
    ~OAHashTable();

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Insert a key/data pair into table. Throws an exception if the insertion is unsuccessful.
    /// @param Key  - The string key
    /// @param Data - The data to add to the table
    //----------------------------------------------------------------------------------------------------------------
    void insert(const char *Key, const T& Data);

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Removes an item by key. Throws an exception if the key doesn't exist.
    ///        Compacts the table by moving key/data pairs, if necessary
    /// @param Key - The key of the pair to remove
    //----------------------------------------------------------------------------------------------------------------
    void remove(const char *Key);

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Find and return the data in the table by key
    /// @param Key - The key to find
    /// @return The data or an exception if the key is not found (const T&)
    //----------------------------------------------------------------------------------------------------------------
    const T& find(const char *Key) const;

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Removes all items from the table, but does not deallocate it
    //----------------------------------------------------------------------------------------------------------------
    void clear();

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Allow the client to peer into the table.
    /// @return The statistical data of an OAHashTable (OAHSTStats)
    //----------------------------------------------------------------------------------------------------------------
    OAHTStats GetStats() const;

    //----------------------------------------------------------------------------------------------------------------
    /// @brief  Allow the client to see a slot in the table
    /// @return The data of a slot in the table (OAHTSlot*)
    //----------------------------------------------------------------------------------------------------------------
    const OAHTSlot *GetTable() const;

  private: // Some suggestions (You don't have to use any of this.)

    //----------------------------------------------------------------------------------------------------------------
    /// @brief Expands the table when the load factor reaches a certain point
    ///        (greater than MaxLoadFactor) Grows the table by GrowthFactor,
    ///        making sure the new size is prime by calling GetClosestPrime
    //----------------------------------------------------------------------------------------------------------------
    void GrowTable();


    //----------------------------------------------------------------------------------------------------------------
    /// @brief Workhorse method to locate an item (if it exists)
    /// @param Key - The key to find
    /// @param Slot - Pointer to address of the slot in the table of the key
    /// @return Index if it exists, -1 if not (int)
    //----------------------------------------------------------------------------------------------------------------
    int IndexOf(const char *Key, int& emptyIndex) const;

    //----------------------------------------------------------------------------------------------------------------
    /// @brief  Calculate the load factor of the hash table.
    /// @param tableCount - The amount of slots in use.
    /// @return The load factor of the hash table
    //----------------------------------------------------------------------------------------------------------------
    double LoadFactor(double num_elements);

    /// @brief The configuration of this table
    OAHTConfig m_table_config;

    /// @brief The stats of this table
    OAHTStats m_table_stats;

    /// @brief The table
    std::vector<OAHTSlot> m_Table;

    /// @brief First available slot in the list
    OAHTSlot m_available_slot;
};

#include "OAHashTable.cpp"

#endif
