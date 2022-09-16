#include <cstdlib>
#include <iostream>

#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "boost/interprocess/sync/named_semaphore.hpp"

#include "handlandmarksCPU.h"

int main(int argc, char** argv) {

	using namespace boost::interprocess;

	size_t landmark_coordinates_bytes = NUM_KEYPOINTS * 2 * sizeof(float); // Size for all the landmarks x's and y's

	// Child process

	// Open already created shared memory object.
	shared_memory_object shm(open_only, "MySharedMemory", read_write);

	// Map the whole shared memory in this process
	mapped_region region(shm, read_write);

	void* region_address = region.get_address();

	uchar* image_buff = static_cast<uchar*>(region_address);
	size_t image_size_bytes = FRAME_WIDTH * FRAME_HEIGHT * 3;

	float* coordinates_buff = static_cast<float*>(region_address);

	named_semaphore semaphore(open_only_t(), "Semaphore");
	
	HandlandmarksDetectorCPU handlandmarksDetector("hand_tracking_desktop_live.pbtxt");
	cv::Mat output_image;
	
	for (int i = 0; i < FRAMES_PER_SEQUENCE; ++i)
	{
		semaphore.wait();
		cv::Mat image(cv::Size(FRAME_WIDTH, FRAME_HEIGHT), CV_8UC3, image_buff + image_size_bytes * i, cv::Mat::AUTO_STEP);
  		cv::cvtColor(image, output_image, cv::COLOR_BGR2RGBA);

		// Detect the landmarks in the image
		output_image = handlandmarksDetector.DetectLandmarks(output_image);

		// Write the landmark coordinates
		memcpy(&coordinates_buff[i * NUM_KEYPOINTS * 2], handlandmarksDetector.coordinates, landmark_coordinates_bytes);

		/*
		for (int j = 0; j < NUM_KEYPOINTS * 2; j+=2)
		{
			std::cout << "\nWriter Landmark " << j / 2 << ":" << std::endl;
			std::cout << "\tx:" << handlandmarksDetector.coordinates[j] << std::endl;
			std::cout << "\ty:" << handlandmarksDetector.coordinates[j + 1] << std::endl;
				std::cout << "\tx:" << coordinates_buff[i * NUM_LANDMARKS * 2 + j] << std::endl;
				std::cout << "\ty:" << coordinates_buff[i * NUM_LANDMARKS * 2 + j + 1] << std::endl;
		}
		*/

		handlandmarksDetector.resetCoordinates();

		//cv::imshow("Display window", output_image);
		//cv::waitKey(0); 
	}
	return 0;
}
