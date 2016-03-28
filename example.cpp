#include <iostream>

#include <opencv2/opencv.hpp>

#include "opencvhdfs.h"

#include <stdio.h>

int main()
{
	// first delete test.hdf5
	remove( "opencvdfs_test.hdf5" );

	// generate test image
	cv::Mat test_hsv(255,180, CV_8UC3);
	for(int h=0; h<180; h++)
	{
		for(int s=0; s<255; s++)
		{
			test_hsv.at<cv::Vec3b>(cv::Point(h,s)) = cv::Vec3b(
				h,
				s,
				255
			);
		}
	}
	cv::Mat test_rgb;
	cv::cvtColor( test_hsv, test_rgb, cv::COLOR_HSV2BGR );

	// save the image the easy way
	CVHDFS::write( "opencvdfs_test.hdf5", "test_rgb", test_rgb);

	// read the image the easy way
	cv::Mat test_read;
	CVHDFS::read( "opencvdfs_test.hdf5", "test_rgb", test_read);


	cv::imshow("testzzzz", test_read);
	cv::waitKey(0);
}