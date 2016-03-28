#ifndef CVHDFS_H
#define CVHDFS_H

#include <opencv2/opencv.hpp>

#include "H5Cpp.h"

namespace CVHDFS
{

	
	void write( H5::H5File file, std::string key, cv::Mat matrix );
	void write( std::string filename, std::string key, cv::Mat matrix );

	void read( H5::H5File file, std::string key, cv::Mat &matrix );
	void read( std::string filename, std::string key, cv::Mat &matrix );
}

#endif // CVHDFS_H