#pragma once

#include  "UDPConnection.h"
#include <string>
#include<vector>
class ImageRecognizeManager
{
public:
	ImageRecognizeManager(void);
	~ImageRecognizeManager(void);

	void startProcessing(std::string strDbFold,std::string strScanFold);
	void loadDatabaseImages(std::string strDbFold,std::vector<std::string>&strModelImgs);

	void sendResultToUnity(std::string strClass,std::string strCroppedImg);
	void sendStrToUnity(std::string strCotent);

	CUDPConnection mUDP;
	
};

