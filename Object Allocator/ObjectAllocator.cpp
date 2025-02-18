///-------------------------------------------------------------------------
 // @file ObjectAllocator.cpp
 // @author Aidan Straker (aidan.straker@digipen.edu)
 // @brief 
 // @version 0.1
 // @date 2024-01-12
 //
 // @copyright Copyright (c) 2024
 //
 ///-------------------------------------------------------------------------

#include "ObjectAllocator.h"
#include <iostream>
#include <cstring> // memset
#include <cstdint> // intptr_r

// Size of a pointer.
constexpr size_t PTR_SIZE = sizeof(intptr_t);


    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the generic object.
    /// @param ptr - the header block.
    /// @return The address of the header block (char*).
    //--------------------------------------------------------------------------------------------
    unsigned char* ObjectAllocator::GetHeaderAddress(GenericObject* ptr) const {return reinterpret_cast<unsigned char*>(ptr) - config_.HBlockInfo_.size_ - config_.PadBytes_;}

    //--------------------------------------------------------------------------------------------
    /// @brief Create a specific sized block from a page.
    /// @param page      - The page to use to make the block.
    /// @param blockSize - The size of the block memory to create.
    /// @return The newly created block (GenericObject*).
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::InitBlocks(unsigned char* page)
    {
      // Starting point of the page.
      unsigned char* startAddress = page;
      // Staring point of the data on the page.
      unsigned char* dataStartAddress = page + m_FirstObjectOffset;


      for (; static_cast<unsigned>(abs(static_cast<int>(dataStartAddress - startAddress))) < stats_.PageSize_; dataStartAddress += m_DistanceBetweenObjects)
      {
        // Cast the address so it can be added to the free list
        GenericObject* dataAddress = reinterpret_cast<GenericObject*>(dataStartAddress);
        // Add the object to the list.
        dataAddress->Next = FreeList_;
        FreeList_ = dataAddress;

        if (config_.DebugOn_)
        {
          // Set the bytes of the object to the unallocated pattern
          memset(reinterpret_cast<unsigned char*>(dataAddress) + PTR_SIZE, UNALLOCATED_PATTERN, stats_.ObjectSize_ - PTR_SIZE);
          // Set the left pad bytes to the pad bytes pattern
          memset(reinterpret_cast<unsigned char*>(dataAddress) - config_.PadBytes_, PAD_PATTERN, config_.PadBytes_);
          // Set the right pad bytes to the pad bytes pattern.
          memset(reinterpret_cast<unsigned char*>(dataAddress) + stats_.ObjectSize_, PAD_PATTERN, config_.PadBytes_);
        }
        // Set the header bytes to 0.
        memset(GetHeaderAddress(dataAddress), 0, config_.HBlockInfo_.size_);
      }
      // Update the amount of free blocks.
      stats_.FreeObjects_ += config_.ObjectsPerPage_;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Check if any object is in use by the client.
    /// @param ptr - the object to check.
    /// @return Is the object in use by the client (bool).
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::CheckInUse(GenericObject *ptr) const
    {
      // Method for checking if an object is in use varies by header type.
      switch (config_.HBlockInfo_.type_)
      {
      case OAConfig::hbNone: // None
        {
          // Pointer for traversing the free list.
          GenericObject* object = FreeList_;
          // Traverse the free list.
          while (object)
          {
            // If the object is on the free list, 
            if (object == ptr)
            {
              return true;
            }
            // Move onto the next object.
            object = object->Next;
          }
          return false;
        }
        break;

        // Basic and Extended have the same method for checking.
        case OAConfig::hbBasic: // Basic

        case OAConfig::hbExtended: // Extended
        {
          // Get the address of the flag.
          unsigned char* flag = reinterpret_cast<unsigned char*>(ptr) - config_.PadBytes_ - 1;
          // Check if the flag has been set.
          return *(flag);
        }

        // Check if the address has been allocated
        case OAConfig::hbExternal: // External
        {
          return *(GetHeaderAddress(ptr));
        }
      
      default:
         return false;
        break;
      }
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Initialises the header blocks for this OA.
    /// @param ptr   - The block to initialise.
    /// @param type  - The type of header block
    /// @param label - The name of the header block.
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::InitHeader(GenericObject *ptr, OAConfig::HBLOCK_TYPE type, const char *label)
    {
      // Convert the header pointer to char.
      unsigned char* headerAddress = GetHeaderAddress(ptr);
      // if the pointer is valid
      if (ptr)
      {
        // A switch that determines the initialisation proccess based on header type
        switch (type)
        {
        case OAConfig::hbNone: // None
          break;

        // Initialisation of basic header blocks
        case OAConfig::hbBasic: // Basic
          {
            unsigned* num_alloc = reinterpret_cast<unsigned*>(headerAddress);
            *num_alloc = stats_.Allocations_;
            unsigned char* flag = reinterpret_cast<unsigned char*>(num_alloc + 1);
            *flag = true;
          }
          break;
        // Initialisation of extended header blocks
        case OAConfig::hbExtended: // Extended
          {
            unsigned short* add = reinterpret_cast<unsigned short*>(headerAddress + config_.HBlockInfo_.additional_);
            ++(*add);
            unsigned* num_alloc = reinterpret_cast<unsigned*>(add + 1);
            *num_alloc = stats_.Allocations_;
            unsigned char* flag = reinterpret_cast<unsigned char*>(num_alloc + 1);
            *flag = 1;
          }
          break;
        // Initialisation of external blocks.
        case OAConfig::hbExternal: // External
          {
            MemBlockInfo** mem_ptr = reinterpret_cast<MemBlockInfo**>(headerAddress);
            // Attempt to initialise a memblockinfo.
            try
            {
              (*mem_ptr) = new MemBlockInfo{true, nullptr, stats_.Allocations_};
              if (label)
              {
                // Attempt to initialise the label.
                try
                {
                  (*mem_ptr)->label = new char[std::strlen(label) + 1];
                }
                catch(const std::bad_alloc& e)
                {
                  throw OAException(OAException::E_NO_MEMORY, "InitHeader: Out of physical memory (operator new fails)");
                }

                // Copy data in label.
                strcpy((*mem_ptr)->label, label);
              }
            }
            catch(const std::bad_alloc& e)
            {
              throw OAException(OAException::E_NO_MEMORY, "InitHeader: Out of physical memory (operator new fails)");
            }
          }
          break;
        
        default:
          break;
        }
      }
    }
    
    //--------------------------------------------------------------------------------------------
    /// @brief Frees a header block
    /// @param ptr - the block to be freed
    /// @param type - the type of header block.
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::FreeHeader(GenericObject* ptr, OAConfig::HBLOCK_TYPE type)
    {
      // Convert pointer to char.
      unsigned char* headerAddress = GetHeaderAddress(ptr);
      // Different process to free the different header types.
      switch (type)
      {
      // Freeing none-type header
      case OAConfig::hbNone: // None
      {
        if (config_.DebugOn_)
        {
          // Get the last byte of the object.
          unsigned char* pointer = reinterpret_cast<unsigned char*>(ptr) + stats_.ObjectSize_ - 1;
          // Check for double free.
          if (*pointer == ObjectAllocator::FREED_PATTERN)
          {
            // Throw if double free detected.
            throw OAException(OAException::E_MULTIPLE_FREE, "FreeHeaderNone: Block has already been freed");
          }
        }
      }
      break;

      // Freeing  basic-type header.
      case OAConfig::hbBasic: // Basic
      {
        if (config_.DebugOn_)
        {
          // Check for double free
          if (*(headerAddress + sizeof(unsigned)) == 0)
          {
            // Throw if double free detected.
            throw OAException(OAException::E_MULTIPLE_FREE, "FreeHeaderBasic: Block has already been freed");
          }
        }
        // Set all bytes in the header to 0.
        memset(headerAddress, 0, OAConfig::BASIC_HEADER_SIZE);
      }
      break;
      
      case OAConfig::hbExtended: // Extended
      {
        if (config_.DebugOn_)
        {
          // Check for double free
          if (*(headerAddress + sizeof(unsigned) + config_.HBlockInfo_.additional_ + sizeof(unsigned short)) == 0)
          {
            // Throw if double free detected.
            throw OAException(OAException::E_MULTIPLE_FREE, "FreeHeaderExtended: Block has already been freed");
          }
        }
        memset(headerAddress  + config_.HBlockInfo_.additional_ + sizeof(unsigned short), 0, OAConfig::BASIC_HEADER_SIZE);
      }
      break;

      case OAConfig::hbExternal: //External
      {
        MemBlockInfo** memptr = reinterpret_cast<MemBlockInfo**>(headerAddress);
        if (!(*memptr) && config_.DebugOn_)
        {
          return;
        }
        if ((*memptr)->label)
        {
          delete[] (*memptr)->label;
        }
        delete *memptr;
        *memptr = nullptr;
      }
      break;

      default:
        break;
      }
    }
    //--------------------------------------------------------------------------------------------
    /// @brief Accounts for the inter and left alignment blocks when calculating the distance
    ///        between the start of the page and the first object & calculating the distance
    ///        between objects on the page.
    /// @param originalBlockSize - the total size of a block of memory on the page.
    /// @param alignment - the alignment from the config
    /// @return The alignment for the header blocks and data blocks (size_t).
    //--------------------------------------------------------------------------------------------
    size_t ObjectAllocator::Align(size_t originalBlockSize, size_t alignment)
    {
      // If there is no alignment then nothing needs to be done.
      if (!alignment)
      {
        return originalBlockSize;
      }

      // If the original size is perfectly divisible then the remainder is 1.
      size_t remainder = originalBlockSize % alignment == 0 ? 0ULL : 1ULL;
      return alignment * (originalBlockSize / alignment + remainder);
    }

    //--------------------------------------------------------------------------------------------
    /// @brief A helper function that creates pages.
    /// @param totalPageSize - the size of the page to be created.
    /// @return The created page (GenericObject*).
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::MakePage()
    {
      // Check if the maximum number of pages is in use.
      if (stats_.PagesInUse_ == config_.MaxPages_)
      {
        throw OAException(OAException::E_NO_PAGES, "MakePage: out of logical memory");
      }
      
      GenericObject* castPage;
      // The new page.
      unsigned char* newPage;
      // Allocate a page, using a try/catch block if allocation fails.
      try
      {
        newPage = new unsigned char[stats_.PageSize_];
      }
      catch(const std::bad_alloc& e)
      {
        // The Object Allocator exception to be thrown.
        throw OAException(OAException::E_NO_MEMORY, "MakePage: out of physical memory");
      }

      if (config_.DebugOn_)
      {
        memset(newPage, ALIGN_PATTERN, stats_.PageSize_);
      }
      
      // After the page has been allocated, initialise the blocks on the page.
      InitBlocks(newPage);
      // Cast the page, then add to free list.
      castPage = reinterpret_cast<GenericObject*>(newPage);
      castPage->Next = PageList_;
      // Update pages in use.
      stats_.PagesInUse_ ++;
      // Set the page as the first in the page list.
      PageList_ = castPage;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Creates the ObjectManager per the specified values
    /// @param ObjectSize - The size in bytes of the object.
    /// @param config     - Configuration options for the ObjectAllocator
    /// @return Throws an exception if the construction fails. (Memory allocation problem)
    //--------------------------------------------------------------------------------------------
    ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
      : config_(config)
    { 
      // Store the size of each object.
      stats_.ObjectSize_ = ObjectSize;
      // Calculate the page size.
      size_t leftHeaderSize, midBlockSize;
      // Distance between the first object and start of the page, not accounting for alignment.
      leftHeaderSize = PTR_SIZE + config_.HBlockInfo_.size_ + static_cast<size_t>(config_.PadBytes_);
      // Distance between objects on a page, not accounting for alignment.
      midBlockSize = stats_.ObjectSize_ + (config_.PadBytes_ * 2ULL) + config_.HBlockInfo_.size_;
      // The offset of the data from the front of the page, accounting for alignment.
      m_FirstObjectOffset = Align(leftHeaderSize, config_.Alignment_);
      // The distance between each object on the page, accounting for alignment.
      m_DistanceBetweenObjects = Align(midBlockSize, config_.Alignment_);

      // The size of alignment blocks between objects on the page.
      config_.InterAlignSize_ = static_cast<unsigned int>(m_DistanceBetweenObjects - midBlockSize);
      // The size of the initial alignment block on the page.
      config_.LeftAlignSize_ = static_cast<unsigned int>(m_FirstObjectOffset - leftHeaderSize);
      // The total page size.
      stats_.PageSize_ = PTR_SIZE + config_.LeftAlignSize_ + (config_.ObjectsPerPage_ * m_DistanceBetweenObjects) - config_.InterAlignSize_;
      
      // After all those calculations, make the page.
      MakePage();
    }

    //--------------------------------------------------------------------------
    /// @brief Destroys the ObjectAllocator (never throws).
    //--------------------------------------------------------------------------
    ObjectAllocator::~ObjectAllocator() 
    {
      // Temporary pointer for traversing the page list.
      GenericObject* page = PageList_;

      // Then deallocate memory used by the page list.
      while (page)
      {
        // Pointer to the next page.
        GenericObject* nextPage = page->Next;
        if (config_.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
          // Get the address of the first object on the the page.
          unsigned char* address = reinterpret_cast<unsigned char*>(page) + m_FirstObjectOffset;
          for (unsigned int i = 0; i < config_.ObjectsPerPage_; i++)
          {
            // Free the header that is part of this block of memory.
            FreeHeader(reinterpret_cast<GenericObject*>(address), config_.HBlockInfo_.type_);
          }
        }
        // Delete the rest of the page.
        delete[] reinterpret_cast<unsigned char*>(page);
        // Move onto the next page.
        page = nextPage;
      }
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Take an object from the free list and give it to the client (simulates new)
    /// @param label - The name of the newly allocated object.
    /// @return Throws an exception if the object can't be allocated. (Memory allocation problem)
    //--------------------------------------------------------------------------------------------
    void * ObjectAllocator::Allocate(const char *label) 
    { 
      // Creating a block using new.
      if(config_.UseCPPMemManager_)
      {
        try
        {
          GenericObject* newBlock = new GenericObject();
          // Update the allocator stats.
          stats_.ObjectsInUse_++;
          stats_.Allocations_++;
          stats_.FreeObjects_--;

          // Update most objects in use, if necessary.
          if (stats_.ObjectsInUse_ > stats_.MostObjects_)
          {
            stats_.MostObjects_ = stats_.ObjectsInUse_;
          }

          return newBlock;
        }
        catch(const std::bad_alloc& e)
        {
          throw OAException(OAException::E_NO_MEMORY, "Out of physical memory (operator new fails)");
        }
      }
      
      // If there are no free objects make a new page.
      if (stats_.FreeObjects_ == 0)
      {
        // This will throw if unable to make more pages.
        MakePage();
      }

      // If not using new/delete, get the object from the free list.
      GenericObject* object = FreeList_;
      FreeList_ = FreeList_->Next;

      if(config_.DebugOn_)
      {
        // Set the pattern indicating the block is in use by the client.
        memset(object, ALLOCATED_PATTERN, stats_.ObjectSize_);
      }

      // The stats have to be updated.
      stats_.ObjectsInUse_++;
      stats_.Allocations_++;
      stats_.FreeObjects_--;

      // Update most objects in use, if necessary.
      if (stats_.ObjectsInUse_ > stats_.MostObjects_)
      {
        stats_.MostObjects_ = stats_.ObjectsInUse_;
      }

      // Create the header for this block.
      InitHeader(object, config_.HBlockInfo_.type_, label);
  
      return object;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Returns an object to the free list (simulates delete).
    /// @param Object - Pointer tot he object to add to the free list.
    /// @return Throws an exception if the object can't be freed. (Invalid object)
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::Free(void *Object) 
    { 
      if(config_.DebugOn_)
      {
        // Check if the object is on the free list.
        if (IsFreed(Object))
        {
          throw OAException(OAException::E_MULTIPLE_FREE, "Free: block has already been freed");
        }

        // Check if the object is properly aligned.
        if (!IsAligned(Object))
        {
          throw OAException(OAException::E_BAD_BOUNDARY, "Free: block address is on a page, but not on any block-boundary");
        }

        // Check if the left pad bytes have been corrupted
        if (!CheckPadding((reinterpret_cast<unsigned char*>(Object) - config_.PadBytes_), config_.PadBytes_))
        {
          throw OAException(OAException::E_CORRUPTED_BLOCK, "Free: Block has been corrupted (left pad bytes have been overwritten)");
        }

        // Check if the right pad bytes have been corrupted.
        if (!CheckPadding((reinterpret_cast<unsigned char*>(Object) + stats_.ObjectSize_), config_.PadBytes_))
        {
          throw OAException(OAException::E_CORRUPTED_BLOCK, "Free: Block has been corrupted (right pad bytes have been overwritten)");
        }
      }

      // Cast the passed in Object to GenericObject.
      GenericObject* castedObject = reinterpret_cast<GenericObject*>(Object);
      // Free the header block.
      FreeHeader(castedObject, config_.HBlockInfo_.type_);

      // Set the freed pattern.
      unsigned char* charCastedObject = reinterpret_cast<unsigned char*>(Object);

      if (config_.DebugOn_)
      {
        memset(charCastedObject, FREED_PATTERN, stats_.ObjectSize_);
      }
      
      // Update the allocator stats.
      stats_.Deallocations_++;
      stats_.FreeObjects_++;
      stats_.ObjectsInUse_--;
      // Add the block to the free list.
      GenericObject* freedObject = reinterpret_cast<GenericObject*>(charCastedObject);
      freedObject->Next = FreeList_;
      FreeList_ = freedObject;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Calls the callback function for each block still in use.
    /// @param fn - function pointer.
    /// @return The number of objects in use.
    //--------------------------------------------------------------------------------------------
    unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
    { 
      // If the page does not exist
      if (!PageList_)
      {
        return 0;
      }
      
      // Keep track of how much data ahs been used.
      unsigned dataUsed = 0;

      // Pointer for traversing the list.
      GenericObject* current = PageList_;

      // Traverse the list
      while (current)
      {
        // Pointer to the start of the data inside the page.
        unsigned char* castedData = reinterpret_cast<unsigned char*>(current) + m_FirstObjectOffset;

        // Traverse through the page.
        for (size_t i = 0; i < config_.ObjectsPerPage_; i++)
        {
          GenericObject* ptr = reinterpret_cast<GenericObject*>(castedData + (i * m_DistanceBetweenObjects));
          if (CheckInUse(ptr))
          {
            // Use the callback.
            fn(ptr, stats_.ObjectSize_);
            dataUsed++;
          }
        }
    
        // Move to the next page.
        current = current->Next;
      }
      
      return dataUsed;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Calls the callback fn for each block that is potentially corrupted
    /// @param fn - function pointer.
    /// @return Returns the number of corrupted blocks.
    //--------------------------------------------------------------------------------------------
    unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
    { 
      // If allocator debugging is disabled or there are no pad bytes, do not do any validation.
      if (!config_.DebugOn_ || config_.PadBytes_ == 0)
      {
        return 0;
      }

      // Number of corrupted blocks.
      unsigned numCorrupted = 0;

      // Pointer for traversing the page list.
      GenericObject* page = PageList_;

      // Traverse the linked list.
      while (page)
      {
        // Get the address of the first object in the page.
        unsigned char* Object = reinterpret_cast<unsigned char*>(page) + m_FirstObjectOffset;

        // For every object on the current page.
        for (size_t i = 0; i < config_.ObjectsPerPage_; ++i)
        {
          GenericObject* castedObject = reinterpret_cast<GenericObject*>(Object + i * m_DistanceBetweenObjects);

          // Check if the pad bytes on either side are corrupted.
          if (!CheckPadding(GetLeftPadBytesAddress(castedObject), config_.PadBytes_) ||
              !CheckPadding(GetRightPadBytesAddress(castedObject), config_.PadBytes_))
          {
            fn(castedObject, stats_.ObjectSize_);
            ++numCorrupted;
          }
        }
        page = page->Next;
      }

      return numCorrupted;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Frees all empty pages.
    /// @return The number of pages that are freed.
    //--------------------------------------------------------------------------------------------
    unsigned ObjectAllocator::FreeEmptyPages() 
    { 
      // If there are no pages.
      if (!PageList_)
      {
        return 0;
      }

      // Number of pages freed.
      unsigned numPagesFreed = 0;

      // Pointer for traversing the page.
      GenericObject* current = PageList_;
      // Pointer to previous page in the list.
      GenericObject* prev = nullptr;
      
      // While the page is valid and the page is free.
      while (current && CheckPageFree(current))
      {
        // The next page becomes the head.
        PageList_ = current->Next;
        // Free the former head.
        FreePage(current);
        // Reassign the new head to current.
        current = PageList_;
        // Increment the number of pages freed.
        numPagesFreed++;
      }

      // Iterate through the rest of the page list.
      while (current)
      {
        // While the page is valid and the page is not free.
        while (current && !CheckPageFree(current))
        {
          // Store a pointer to this page.
          prev = current;
          // Move onto the next page.
          current = current->Next;
        }
        
        // If there are no more pages in the list.
        if (!current)
        {
          // Return the number of pages freed.
          return numPagesFreed;
        }

        // When a free page is found
        prev->Next = current->Next;
        // Delete the free page.
        FreePage(current);
        // Reassign current.
        current = prev->Next;
        ++numPagesFreed;
      }
      
      // Return number of pages freed.
      return numPagesFreed; 
    }

    
    //--------------------------------------------------------------------------------------------
    /// @brief  Has the free empty pages function been implemented?
    /// @return Yes or No (bool).
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::ImplementedExtraCredit() { return true; }

      // Testing/Debugging/Statistic methods
    void ObjectAllocator::SetDebugState(bool State) { debugState_ = State; } // true=enable, false=disable
    const void * ObjectAllocator::GetFreeList() const { return FreeList_; }  // returns a pointer to the internal free list
    const void * ObjectAllocator::GetPageList() const { return PageList_; }  // returns a pointer to the internal page list
    OAConfig ObjectAllocator::GetConfig() const { return config_; }          // returns the configuration parameters
    OAStats ObjectAllocator::GetStats() const{ return stats_; }              // returns the statistics for the allocator

    //--------------------------------------------------------------------------------------------
    /// @brief Has the specified object already been freed?
    /// @param Object - The  object to check.
    /// @return Has the object already been freed (bool)?
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::IsFreed(void* Object)
    {
      // Cast to generic object.
      GenericObject* object = reinterpret_cast<GenericObject*>(Object);
      // Pointer for traversing the freelist.
      GenericObject* ObjectOnFreeList = FreeList_;

      // Traverse the list.
      while (ObjectOnFreeList)
      {
        // If the object is on the freed list then it has been freed already.
        if (ObjectOnFreeList == object)
        {
          return true;
        }
        // Move onto to the next object in the free list.
        ObjectOnFreeList = ObjectOnFreeList->Next;
      }

      return false;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Check if a specified object is aligned with any of the pages.
    /// @param Object - The object to check.
    /// @return Is the object aligned (bool)?
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::IsAligned(void* Object)
    {
      unsigned char* address = reinterpret_cast<unsigned char*>(Object);
      // Traversing the page list.
      for (GenericObject* page = PageList_; page != nullptr; page = page->Next)
      {
        // Cast to char to examine each object in the page.
        unsigned char* start = reinterpret_cast<unsigned char*>(page);

        // End of the page.
        unsigned char* end = start + stats_.PageSize_;

        // Check if the object is on the page.
        if (start > address || address >= end)
        {
          continue;
        }
        
        if (static_cast<unsigned>(address - start) < config_.HBlockInfo_.size_)
        {
          throw OAException(OAException::E_BAD_BOUNDARY, "Block address is on a page, but not on any block-boundary");
        }

        // The address of the first object on the page.
        start += m_FirstObjectOffset;
        // The difference between the address of the parameter and address of the first object.
        long displacement = address - start;

        // If the displacement is perfectly divisible then the object is aligned.
        return static_cast<size_t>(displacement) % m_DistanceBetweenObjects == 0;
      }
      // The object is not on any page.
      return false;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Check if padbytes have been corrupted
    /// @param ptr - address of the padebytes
    /// @param size - the size of the padbytes
    /// @return If the padbytes are not corrupted (bool).
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::CheckPadding(unsigned char* padByteAddress, size_t padSize) const
    {
      for (size_t i = 0; i < padSize; ++i)
      {
        if (*(padByteAddress + i) != ObjectAllocator::PAD_PATTERN)
        {
          return false;
        }
      }
      return true;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the left pad bytes.
    /// @param ptr - The block of memory to find the pad byts in.
    /// @return The left padbytes address (unsigned char*).
    //--------------------------------------------------------------------------------------------
    unsigned char* ObjectAllocator::GetLeftPadBytesAddress(GenericObject* ptr) const { return reinterpret_cast<unsigned char*>(ptr) - config_.PadBytes_; }

    //--------------------------------------------------------------------------------------------
    /// @brief Get the address of the right pad bytes.
    /// @param ptr - The block of memory to find the padbytes next to.
    /// @return The right padbytes address (unsigned char*).
    //-------------------------------------------------------------------------------------------
    unsigned char* ObjectAllocator::GetRightPadBytesAddress(GenericObject* ptr) const { return reinterpret_cast<unsigned char*>(ptr) + stats_.ObjectSize_; }

    //--------------------------------------------------------------------------------------------
    /// @brief This functions frees a page
    /// @param page - the page to be freed.
    //--------------------------------------------------------------------------------------------
    void ObjectAllocator::FreePage(GenericObject* page)
    {
      GenericObject* prev = nullptr;
      GenericObject* current = FreeList_;

      // if the page is valid, check if the current object is on the page.
      while (current && CheckOnPage(page, reinterpret_cast<unsigned char*>(current)))
      {
        // Update the free list.
        FreeList_ = current->Next;
        current = FreeList_;
        stats_.FreeObjects_--;
      }

      // Iterate through the rest of the page.
      while (current)
      {
        // While the object is valid, and not on the page.
        while (current && !CheckOnPage(page, reinterpret_cast<unsigned char*>(current)))
        {
          // Move onto the next one keeping track of the revious object.
          prev = current;
          current = current->Next;
        }

        // End of the free list reached.
        if (!current)
        {
          break;
        }

        // Update the free list.
        prev->Next = current->Next;
        // Decrement the amount of free objects.
        --stats_.FreeObjects_;
        // Reassign current.
        current = prev->Next;
      }
      
      // Delete the page.
      delete[] reinterpret_cast<unsigned char*>(page);
      // Decrement the amount of pages in use.
      --stats_.PagesInUse_;
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Check if an object is on a page.
    /// @param page    - the page to search.
    /// @param address - the address of the object to find.
    /// @return Is the object on the page (bool)?
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::CheckOnPage(GenericObject* page, unsigned char* address) const
    {
      // Address of the beginning of the page.
      unsigned char* castedPage = reinterpret_cast<unsigned char*>(page);
      // Address of the end of the page.
      unsigned char* pageEnd = castedPage + stats_.PageSize_;

      // Whether or not the object is on the page.
      return (address < castedPage && address >= pageEnd);
    }

    //--------------------------------------------------------------------------------------------
    /// @brief Check if the page is free.
    /// @param page - The page to check.
    /// @return Whether or not the page is free (bool)?
    //--------------------------------------------------------------------------------------------
    bool ObjectAllocator::CheckPageFree(GenericObject* page) const
    {
      if (page)
      {
        return false;
      }

      GenericObject* current = FreeList_;
      unsigned numObjects = 0;

      while (current)
      {
        if (CheckOnPage(page, reinterpret_cast<unsigned char*>(current)))
        {
          if (++numObjects > config_.ObjectsPerPage_ - 1)
          {
            return true;
          }
          
        }
        current = current->Next;
      }
      
      return false;
    }
