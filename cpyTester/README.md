This is a curious effect I noticed in one of my projects. A sparse linalg heavy algorithm needed approximately 105 seconds to complete. It ran a total of 49 (sparse) matmults, which needed in total about 80 seconds. Much to my surprise, some 130 copy calls took in total over 20 seconds to complete. I soon found out why: in my haste to get this mess ready, I blindely copied some code over. The BCSR matrix had a vector<vector> allocator for its data. Nice, no need to manually clean up, and no overhead over raw pointers. Except for copy. Here we used simple assignment via operator. It works, of course, but is rather slow. So, here's some timing data on randomly generated sparse matrices. Obviously a single contiguous block of memory will get copied faster than anything presented here, and the presented timings are for a single call only. The algo in question performed well over a hundred of these operations.






