// shape_recognition.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ImageRecognizeManager.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	
	//std::string strDbFold,strScanFold;
	//std::cout<<"��������ͼƬ�ļ��У�����database1���ڵ�·��"<<std::endl;
	//std::cin>>strDbFold;

	//std::cout<<"����Ҫ����ɨ��ͼƬ����ļ��У�����testimage1���ڵ�·��"<<std::endl;
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

