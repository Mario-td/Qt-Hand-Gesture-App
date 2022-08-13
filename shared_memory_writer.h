#ifndef SHARED_MEMORY_WRITER_H
#define SHARED_MEMORY_WRITER_H

#include <cstdlib>

#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "boost/interprocess/sync/named_semaphore.hpp"

#include "utilities.h"

class SharedMemoryWriter
{

public:
    SharedMemoryWriter();
    void writeFrameToMemory(const cv::Mat &frame, int index);

private:
    static constexpr size_t landmark_coordinates_bytes =
        Utilities::NUM_KEYPOINTS * 2 * sizeof(float); // Size for all the landmarks x's and y's
    static constexpr size_t image_size_bytes = Utilities::FRAME_WIDTH * Utilities::FRAME_HEIGHT * 3;

    // Remove shared memory on construction and destruction
    struct shm_remove {
        shm_remove()
        {
            boost::interprocess::shared_memory_object::remove("MySharedMemory");
        }
        ~shm_remove()
        {
            boost::interprocess::shared_memory_object::remove("MySharedMemory");
        }
    } remover;

    // Create a shared memory object.
    boost::interprocess::shared_memory_object shm = boost::interprocess::shared_memory_object(
                                                        boost::interprocess::create_only, "MySharedMemory", boost::interprocess::read_write);
    boost::interprocess::mapped_region *region;
    boost::interprocess::named_semaphore *semaphore;

    void allocateSharedMemory();
    void createNamedSemaphore();
};

#endif // SHARED_MEMORY_WRITER_H
