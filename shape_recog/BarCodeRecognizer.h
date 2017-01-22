#pragma once
#include <opencv2/opencv.hpp>
#include <string>

#include "zbar.h"    
#include "ComponentConnector.h"

class BarCodeRecognizer
{
public:
	BarCodeRecognizer(void);
	~BarCodeRecognizer(void);

	std::string recognizeBarCode(cv::Mat img);

	void locateBarCode(cv::Mat img,cv::Mat &barImg);

	zbar::ImageScanner mBarScanner;
	ComponentConnector mConnector;
	std::string mstrCodeType;
};

