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
    SharedMemoryWriter()
    {
        createNamedSemaphore();
        allocateSharedMemory();
    }

    void writeFrameToMemory(const cv::Mat &frame, int index)
    {
        uchar *image_buff = static_cast<uchar *>(region->get_address());
        memcpy(&image_buff[index * image_size_bytes], frame.data, image_size_bytes);
        //cv::imshow("Child display window", frame);
        //cv::waitKey(0);
        semaphore->post();
    }

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

    void allocateSharedMemory()
    {
        shm.truncate(image_size_bytes * Utilities::FRAMES_PER_SEQUENCE);
        region = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write);
        //void *region_address = region->get_address();
        std::memset(region->get_address(), 0, region->get_size());
    }

    void createNamedSemaphore()
    {
        using namespace boost::interprocess;
        named_semaphore::remove("Semaphore");
        semaphore = new named_semaphore(create_only_t(), "Semaphore", 0);
    }
};

#endif // SHARED_MEMORY_WRITER_H
