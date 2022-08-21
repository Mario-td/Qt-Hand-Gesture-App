#include "shared_memory_reader.h"

SharedMemoryReader::SharedMemoryReader()
{
    mapSharedMemory();
}

float *SharedMemoryReader::getCoordinatesMemoryLocation()
{
    return static_cast<float *>(region->get_address());
}

void SharedMemoryReader::mapSharedMemory()
{
    region = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write);
}
