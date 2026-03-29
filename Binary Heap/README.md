Implement (min) binary heap as discussed in class.

1) data is kept in std::vector (allows use of compiler generated copy and
   assignment -- see partial implementation)
2) indexing starts at 0 (in class I used 1-based indexing). Modify implementation of left, right, parent
3) binary heap stores a tuple <key,value>, binary heap property is based on
   keys, values are arbitrary - similar to std::map. 
4) test2,7,8 have tighter deadline to make sure that your implementation is efficient.
   As with all tight deadlines - if you timeout during submission, try recompiling (on pontus).
   Double check your init_heap is linear. 
