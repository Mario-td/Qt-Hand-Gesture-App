#ifndef SHARED_MEMORY_READER_H
#define SHARED_MEMORY_READER_H

#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"

#include "utilities.h"

class SharedMemoryReader
{
public:
    SharedMemoryReader();
    void readCoordinatesFromMemory();
private:
    // Create a shared memory object.
    boost::interprocess::shared_memory_object shm = boost::interprocess::shared_memory_object(
                                                        boost::interprocess::open_only, "MySharedMemory", boost::interprocess::read_write);
    boost::interprocess::mapped_region *region;
    void mapSharedMemory();
    float *coordinates_buff;
};

#endif // SHARED_MEMORY_READER_H
