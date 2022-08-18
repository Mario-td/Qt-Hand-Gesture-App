#include "shared_memory_reader.h"

SharedMemoryReader::SharedMemoryReader()
{
    mapSharedMemory();
}

void SharedMemoryReader::readCoordinatesFromMemory()
{
    float *coordinates_buff = static_cast<float *>(region->get_address());
    for (int i = 0; i < Utilities::FRAMES_PER_SEQUENCE; ++i) {

        for (int j = 0; j < Utilities::NUM_KEYPOINTS * 2; j += 2) {
            std::cout << "\n Reader Frame " << i << " Landmark " << j / 2 << ":" << std::endl;
            std::cout << "\tx:" << coordinates_buff[i * Utilities::NUM_KEYPOINTS * 2 + j] << std::endl;
            std::cout << "\ty:" << coordinates_buff[i * Utilities::NUM_KEYPOINTS * 2 + j + 1] << std::endl;
        }
    }
}
float *SharedMemoryReader::getCoordinatesMemoryLocation()
{
    return static_cast<float *>(region->get_address());
}

void SharedMemoryReader::mapSharedMemory()
{
    region = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write);
}
