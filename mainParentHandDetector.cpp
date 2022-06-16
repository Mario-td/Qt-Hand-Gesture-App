#include <opencv2/opencv.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <thread>
#include <filesystem>
#include <fstream>

#define NUM_FRAMES 32
#define NUM_LANDMARKS 21
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480 

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Enter gesture number\n";
		return 0;
	}
	using namespace boost::interprocess;
	size_t landmark_coordinates_bytes = NUM_LANDMARKS * 2 * sizeof(float); // Size for all the landmarks x's and y's

	// Parent process
	cv::Mat image;

	// Remove shared memory on construction and destruction
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
	} remover;

	// Create a shared memory object.
	shared_memory_object shm(create_only, "MySharedMemory", read_write);

	// Set size
	size_t image_size_bytes = FRAME_WIDTH * FRAME_HEIGHT * 3;
	shm.truncate(image_size_bytes * NUM_FRAMES);

	// Map the whole shared memory in this process
	mapped_region region(shm, read_write);
	
	named_semaphore::remove("Semaphore");
	named_semaphore semaphore(create_only_t(), "Semaphore", 0);
	
	void* region_address = region.get_address();
	std::memset(region_address, 0, region.get_size());	
	
	std::fstream data_file, label_file;
	data_file.open("X.txt", std::ios::app);
	label_file.open("Y.txt", std::ios::app);

	const std::filesystem::path images_path{argv[1]};
	for (const auto & dir : std::filesystem::directory_iterator{images_path})
    {
		std::string label = dir.path().string();
		std::cout << dir << std::endl;
		if (label[label.size() - 1] >= '0' && label[label.size() - 1] <= '9')
		{

			// Launch child process
			std::thread thrd([](){
						std::string s("~/mediapipe/bazel-bin/hand_tracking/hand_tracking_gpu hand_tracking_desktop_live_gpu.pbtxt");
						std::system(s.c_str());
						});
			
			// Write the image data
			uchar* image_buff = static_cast<uchar*>(region_address);
			float* coordinates_buff = static_cast<float*>(region_address);
		
			for (int i = 0; i < NUM_FRAMES; ++i)
			{
				image = cv::imread(dir.path().string() + "/Sample"+ std::string(std::to_string(i + 1) + ".png"), cv::IMREAD_COLOR);
				cv::resize(image, image, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), cv::INTER_LINEAR);
				memcpy(&image_buff[i * image_size_bytes], image.data, image_size_bytes);
				//cv::imshow("Child display window", image);
				//cv::waitKey(0);
				semaphore.post();
			}
			thrd.join();
			
			for (int i = 0; i < NUM_FRAMES; ++i)
			{
				for (int j = 0; j < NUM_LANDMARKS * 2; j+=2)
				{
					if (j != 0)
						data_file << ",";
						data_file << coordinates_buff[i * NUM_LANDMARKS * 2 + j];
	
					data_file << ",";
	
						data_file << coordinates_buff[i * NUM_LANDMARKS * 2 + j + 1];
	
					/*std::cout << "\nLandmark " << j / 2 << ":" << std::endl;
					std::cout << "\tx:" << coordinates_buff[i * NUM_LANDMARKS * 2 + j] << std::endl;
					std::cout << "\ty:" << coordinates_buff[i * NUM_LANDMARKS * 2 + j + 1] << std::endl;
					*/
				}
				data_file << "\n";
				data_file.flush();
			}
			label_file << argv[1] << "\n"; 
			label_file.flush(); 
		}		
	}
	return 0;
}
