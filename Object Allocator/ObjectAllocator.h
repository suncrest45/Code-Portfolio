///-------------------------------------------------------------------------
 // @file ObjectAllocator.h
 // @author Aidan Straker (aidan.straker@digipen.edu)
 // @brief 
 // @version 0.1
 // @date 2024-01-12
 //
 // @copyright Copyright (c) 2024
 //
 ///-------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH
//---------------------------------------------------------------------------

#include <string>

// If the client doesn't specify these:
static const int DEFAULT_OBJECTS_PER_PAGE = 4;  
static const int DEFAULT_MAX_PAGES = 3;

// Exception Class
class OAException
{
  public:
    // Possible exception codes
    enum OA_EXCEPTION 
    {
      E_NO_MEMORY,      //!< out of physical memory (operator new fails)
      E_NO_PAGES,       //!< out of logical memory (max pages has been reached)
      E_BAD_BOUNDARY,   //!< block address is on a page, but not on any block-boundary
      E_MULTIPLE_FREE,  //!< block has already been freed
      E_CORRUPTED_BLOCK //!< block has been corrupted (pad bytes have been overwritten)
    };

    //--------------------------------------------------------------------------
    /// @brief Constructor for the Exception Class
    /// @param ErrCode - The error code
    /// @param Message - The appropriate message to print.
    //-------------------------------------------------------------------------- 
    OAException(OA_EXCEPTION ErrCode, const std::string& Message) : error_code_(ErrCode), message_(Message) {};

    //--------------------------------------------------------------------------
    /// @brief Destructor for the Exception Class
    //--------------------------------------------------------------------------
    virtual ~OAException() {}

    //--------------------------------------------------------------------------
    /// @brief  Retrieves the error code.
    /// @return One of the 5 error codes.
    //-------------------------------------------------------------------------- 
    OA_EXCEPTION code() const {return error_code_;}

    //--------------------------------------------------------------------------
    /// @brief  Retrieves a human-readable string regarding the error.
    /// @return Returns the NULL terminated string representing the error.
    //--------------------------------------------------------------------------
    virtual const char *what() const {return message_.c_str();}

  private:  
    OA_EXCEPTION error_code_; //!< The error code (one of the 5)
    std::string message_;     //!< The formatted string for the user.
};


/// @brief Object Allocator Configuration Parameters
struct OAConfig
{
  static const size_t BASIC_HEADER_SIZE = sizeof(unsigned) + 1; //!< allocation number + flags
  static const size_t EXTERNAL_HEADER_SIZE = sizeof(void*);     //!< just a pointer

  /// @brief The different types of header blocks.
  enum HBLOCK_TYPE{hbNone, hbBasic, hbExtended, hbExternal};

  /// @brief POD that stores the information related to the header blocks.
  struct HeaderBlockInfo
  {
    HBLOCK_TYPE type_;  //!< Which of the 4 header types to use?
    size_t size_;       //!< The size of this header
    size_t additional_; //!< How many user-defined additional bytes

    //--------------------------------------------------------------------------
    /// @brief Constructor
    /// @param type       - The kind of header blocks in use.
    /// @param additional - The number of user-defined additional bytes required.
    //--------------------------------------------------------------------------
    HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) : type_(type), size_(0), additional_(additional)
    {
      if (type_ == hbBasic)
        size_ = BASIC_HEADER_SIZE;
      else if (type_ == hbExtended) // alloc # + use counter + flag byte + user-defined
        size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + additional_;
      else if (type_ == hbExternal)
        size_ = EXTERNAL_HEADER_SIZE;
    };
  };

  //--------------------------------------------------------------------------
  /// @brief OAConfig Constructor
  /// @param UseCPPMemManager - Determines whether or not to by-pass the OA.
  /// @param ObjectsPerPage   - Number of objects for each page of memory.
  /// @param MaxPages         - Maximum number of pages before throwing an exception. A value
  ///                           of 0 means unlimited.
  /// @param DebugOn          - Is debugging code on or off?
  /// @param PadBytes         - The number of bytes to the left and right of a block to pad with.
  /// @param HBInfo           - Information about the header blocks used.
  /// @param Alignment        - The number of bytes to align on.
  //--------------------------------------------------------------------------
  OAConfig(bool UseCPPMemManager = false,
           unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE, 
           unsigned MaxPages = DEFAULT_MAX_PAGES, 
           bool DebugOn = false, 
           unsigned PadBytes = 0,
           const HeaderBlockInfo &HBInfo = HeaderBlockInfo(),
           unsigned Alignment = 0) : UseCPPMemManager_(UseCPPMemManager),
                                     ObjectsPerPage_(ObjectsPerPage), 
                                     MaxPages_(MaxPages), 
                                     DebugOn_(DebugOn), 
                                     PadBytes_(PadBytes),
                                     HBlockInfo_(HBInfo),
                                     Alignment_(Alignment)
  {
    HBlockInfo_ = HBInfo;
    LeftAlignSize_ = 0;  
    InterAlignSize_ = 0;
  }

  bool UseCPPMemManager_;      //!< by-pass the functionality of the OA and use new/delete
  unsigned ObjectsPerPage_;    //!< number of objects on each page
  unsigned MaxPages_;          //!< maximum number of pages the OA can allocate (0=unlimited)
  bool DebugOn_;               //!< enable/disable debugging code (signatures, checks, etc.)
  unsigned PadBytes_;          //!< size of the left/right padding for each block
  HeaderBlockInfo HBlockInfo_; //!< size of the header for each block (0=no headers)
  unsigned Alignment_;         //!< address alignment of each block
  unsigned LeftAlignSize_;     //!< number of alignment bytes required to align first block
  unsigned InterAlignSize_;    //!< number of alignment bytes required between remaining blocks
};


/// @brief POD that holds the Object Allocator statistical info.
struct OAStats
{
  /// @brief Constructor.
  OAStats() : ObjectSize_(0), PageSize_(0), FreeObjects_(0), ObjectsInUse_(0), PagesInUse_(0),
                  MostObjects_(0), Allocations_(0), Deallocations_(0) {};

  size_t ObjectSize_;      //!< size of each object
  size_t PageSize_;        //!< size of a page including all headers, padding, etc.
  unsigned FreeObjects_;   //!< number of objects on the free list
  unsigned ObjectsInUse_;  //!< number of objects in use by client
  unsigned PagesInUse_;    //!< number of pages allocated
  unsigned MostObjects_;   //!< most objects in use by client at one time
  unsigned Allocations_;   //!< total requests to allocate memory
  unsigned Deallocations_; //!< total requests to free memory
};

/// @brief This allows us to easily treat raw objects as nodes in a linked list.
struct GenericObject
{
  GenericObject *Next; //!< The next object in the list
};

/// @brief This is used with external headers.
struct MemBlockInfo
{
  bool in_use;        //!< Is the block free or in use?
  char *label;        //!< A dynamically allocated NUL-terminated string
  unsigned alloc_num; //!< The allocation number (count) of this block
};

/// @brief This class represents a custom memory manager.
class ObjectAllocator
{
  public:
    // Defined by the client (pointer to a block, size of block)
    typedef void (*DUMPCALLBACK)(const void *, size_t);     //!< Callback function when dumping memory leaks
    typedef void (*VALIDATECALLBACK)(const void *, size_t); //!< Callback function when validating blocks

      // Predefined values for memory signatures
    static const unsigned char UNALLOCATED_PATTERN = 0xAA; //!< New memory never given to the client
    static const unsigned char ALLOCATED_PATTERN =   0xBB; //!< Memory owned by the client
    static const unsigned char FREED_PATTERN =       0xCC; //!< Memory returned by the client
    static const unsigned char PAD_PATTERN =         0xDD; //!< Pad signature to detect buffer over/under flow
    static const unsigned char ALIGN_PATTERN =       0xEE; //!< For the alignment bytes

    //--------------------------------------------------------------------------------------------
    /// @brief Creates the ObjectManager per the specified values
    /// @param ObjectSize - The size in bytes of the object.
    /// @param config     - Configuration options for the ObjectAllocator
    /// @return Throws an exception if the construction fails. (Memory allocation problem)
    //--------------------------------------------------------------------------------------------
    ObjectAllocator(size_t ObjectSize, const OAConfig& config);

    //--------------------------------------------------------------------------
    /// @brief Destroys the ObjectAllocator (never throws).
    //--------------------------------------------------------------------------
    ~ObjectAllocator();

    //--------------------------------------------------------------------------------------------
    /// @brief Take an object from the free list and give it to the client (simulates new)
    /// @param label - The name of the newly allocated object.
    /// @return Throws an exception if the object can't be allocated. (Memory allocation problem)
    //--------------------------------------------------------------------------------------------
    void *Allocate(const char *label = 0);

    //--------------------------------------------------------------------------------------------
    /// @brief Returns an object to the free list (simulates delete).
    /// @param Object - Pointer tot he object to add to the free list.
    /// @return Throws an exception if the object can't be freed. (Invalid object)
    //--------------------------------------------------------------------------------------------
    void Free(void *Object);

    //--------------------------------------------------------------------------------------------
    /// @brief Calls the callback function for each block still in use.
    /// @param fn - function pointer.
    /// @return The number of objects in use.
    //--------------------------------------------------------------------------------------------
    unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

    /// @brief Calls the callback fn for each block that is potentially corrupted
    /// @param fn - function pointer.
    /// @return Returns the number of corrupted blocks.
    unsigned ValidatePages(VALIDATECALLBACK fn) const;

    /// @brief Frees all empty pages.
    /// @return The number of pages that are freed.
    unsigned FreeEmptyPages();

      // Returns true if FreeEmptyPages and alignments are implemented
    static bool ImplementedExtraCredit();

      // Testing/Debugging/Statistic methods
    void SetDebugState(bool State);   // true=enable, false=disable
    const void *GetFreeList() const;  // returns a pointer to the internal free list
    const void *GetPageList() const;  // returns a pointer to the internal page list
    OAConfig GetConfig() const;       // returns the configuration parameters
    OAStats GetStats() const;         // returns the statistics for the allocator

      // Prevent copy construction and assignment
    ObjectAllocator(const ObjectAllocator &oa) = delete;            //!< Do not implement!
    ObjectAllocator &operator=(const ObjectAllocator &oa) = delete; //!< Do not implement!
  private:
      // Some "suggested" members (only a suggestion!)
    GenericObject *PageList_; //!< the beginning of the list of pages
    GenericObject *FreeList_; //!< the beginning of the list of objects
    OAConfig config_;         //!< Configuration of the ObjectAllocator instance.
    OAStats stats_;           //!< Statistics of this ObjectAllocator instance.
    bool debugState_;         //!< Turn on/off the allocators debugging.
    size_t m_FirstObjectOffset;
    size_t m_DistanceBetweenObjects;      

    
    // Lots of other private stuff... 
    
    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the generic object.
    /// @param ptr - the header block.
    /// @return The address of the header block (char*).
    //--------------------------------------------------------------------------------------------
    unsigned char* GetHeaderAddress(GenericObject* ptr) const;

    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the left pad bytes.
    /// @param ptr - The block of memory to find the padbytes next to.
    /// @return The left padbytes address (char*).
    //--------------------------------------------------------------------------------------------
    unsigned char* GetLeftPadBytesAddress(GenericObject* ptr) const;

    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the right pad bytes.
    /// @param ptr - The block of memory to find the padbytes next to.
    /// @return The right padbytes address (char*).
    //--------------------------------------------------------------------------------------------
    unsigned char* GetRightPadBytesAddress(GenericObject* ptr) const;

    //--------------------------------------------------------------------------------------------
    /// @brief Initialises the header blocks for this OA.
    /// @param ptr   - The block to initialise.
    /// @param type  - The type of header block
    /// @param label - The name of the header block.
    //--------------------------------------------------------------------------------------------
    void InitHeader(GenericObject* ptr, OAConfig::HBLOCK_TYPE type, const char* label);

    //--------------------------------------------------------------------------------------------
    /// @brief Frees a header block
    /// @param ptr - the block to be freed
    /// @param type - the type of header block.
    //--------------------------------------------------------------------------------------------
    void FreeHeader(GenericObject* ptr, OAConfig::HBLOCK_TYPE type);

    //--------------------------------------------------------------------------------------------
    /// @brief Calculates the alignment of header blocks and data blocks on the page.
    /// @param blockSize - the total size of a block of memory on the page.
    /// @param alignment - the alignment from the config
    /// @return The alignment for the header blocks and data blocks (size_t).
    //--------------------------------------------------------------------------------------------
    size_t Align(size_t blockSize, size_t alignment);

    //--------------------------------------------------------------------------------------------
    /// @brief A helper function that creates pages.
    /// @param totalPageSize - The size of the page to be created.
    /// @return The created page (GenericObject*).
    //--------------------------------------------------------------------------------------------
    void MakePage();

    //--------------------------------------------------------------------------------------------
    /// @brief Create a specific sized block from a page.
    /// @param page      - The page to use to make the block.
    /// @param blockSize - The size of the block memory to create.
    /// @return The newly created block (GenericObject*).
    //--------------------------------------------------------------------------------------------
    void InitBlocks(unsigned char* page);

    //--------------------------------------------------------------------------------------------
    /// @brief Has the parameter already been freed?
    /// @param Object - The  parameter in question.
    /// @return Is the object on the free list (bool)?
    //--------------------------------------------------------------------------------------------
    bool IsFreed(void* Object);

    //--------------------------------------------------------------------------------------------
    /// @brief Determine if an object fits perfectly within a page.
    /// @param Object - The object to check.
    /// @return Whether or not the object is part of the page (bool).
    //--------------------------------------------------------------------------------------------
    bool IsAligned(void* Object);

    //--------------------------------------------------------------------------------------------
    /// @brief Check if any object is in use by the client.
    /// @param ptr - the object to check.
    /// @return Is the object in use by the client (bool).
    //--------------------------------------------------------------------------------------------
    bool CheckInUse(GenericObject* ptr) const;

    //--------------------------------------------------------------------------------------------
    /// @brief Check if padbytes have been corrupted
    /// @param ptr - address of the padebytes
    /// @param size - the size of the padbytes
    /// @return Whether or not the padbytes were corrupted (bool).
    //--------------------------------------------------------------------------------------------
    bool CheckPadding(unsigned char* ptr, size_t size) const;

    //--------------------------------------------------------------------------------------------
    /// @brief This functions frees a page
    /// @param page - the page to be freed.
    //--------------------------------------------------------------------------------------------
    void FreePage(GenericObject* page);

    bool CheckOnPage(GenericObject* page, unsigned char* address) const;

    
    bool CheckPageFree(GenericObject* page) const;
};

#endif
