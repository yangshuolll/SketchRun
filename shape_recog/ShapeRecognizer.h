#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <string>

class ShapeRecognizer
{
public:
	ShapeRecognizer(void);
	~ShapeRecognizer(void);

	

	void computeImageGradient(cv::Mat img, cv::Mat &gradImg);

	std::string recogImageClass(cv::Mat img, std::string model_name);

	void detectContours(cv::Mat img,std::vector<cv::Point> &srcContours,std::vector<cv::Point> &shuffleContours );

	void extractShapesFromDatabase(std::vector<std::string> &strImgs);

	void extractIPImage(cv::Mat img,std::vector<cv::Point> &srcContours ,cv::Mat &png);

	cv::Rect getBoundingBox(const std::vector<cv::Point> &contours);
	cv::Rect resizeBoundingBox(cv::Rect rct,int imgWidth,int imgHeight);

	cv::Mat mCroppedPng;
	std::map<std::string,std::vector<cv::Point>> mShapeDb;
	std::vector<std::string> mShapeNames;
	int mProcessImgWidth;
	int mRows;
	int mCols;
	void rotateImageToVetricalPos(cv::Mat &img);
};

