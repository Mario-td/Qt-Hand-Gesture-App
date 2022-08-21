#include "shared_memory_writer.h"

SharedMemoryWriter::SharedMemoryWriter()
{
    createNamedSemaphore();
    allocateSharedMemory();
}

void SharedMemoryWriter::writeFrameToMemory(const cv::Mat &frame, int index)
{
    uchar *image_buff = static_cast<uchar *>(region->get_address());
    memcpy(&image_buff[index * image_size_bytes], frame.data, image_size_bytes);
    semaphore->post();
}

void SharedMemoryWriter::allocateSharedMemory()
{
    shm.truncate(image_size_bytes * FRAMES_PER_SEQUENCE);
    region = new boost::interprocess::mapped_region(shm, boost::interprocess::read_write);
    std::memset(region->get_address(), 0, region->get_size());
}

void SharedMemoryWriter::createNamedSemaphore()
{
    using namespace boost::interprocess;
    named_semaphore::remove("Semaphore");
    semaphore = new named_semaphore(create_only_t(), "Semaphore", 0);
}
