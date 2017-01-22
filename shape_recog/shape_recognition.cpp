// shape_recognition.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ImageRecognizeManager.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	
	//std::string strDbFold,strScanFold;
	//std::cout<<"输入样本图片文件夹，例如database1所在的路径"<<std::endl;
	//std::cin>>strDbFold;

	//std::cout<<"输入要监测的扫描图片存放文件夹，例如testimage1所在的路径"<<std::endl;
	//std::cin>>strScanFold;

	std::string strDbFold = "images\\database1";
	std::string strScanFold = "images\\test_ip_bar";

	ImageRecognizeManager recogMgr;
	recogMgr.startProcessing(strDbFold,strScanFold);

	/*
	string strFold("images\\database1");
	vector<string> strImgs;
	vector<string> strImgExts;
	strImgExts.push_back("png");
	strImgExts.push_back("jpg");
	FileHelper::GetFiles(strFold,strImgs,true,strImgExts);

	ShapeRecognizer recog;
	recog.extractShapesFromDatabase(strImgs);

	string strQueryFold("images\\test images1");
	vector<string> strQueryImgs;
	FileHelper::GetFiles(strQueryFold,strQueryImgs,true,strImgExts);

	while(true)
	{

	}*/

	
	return 0;
}

