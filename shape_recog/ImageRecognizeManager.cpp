#include "StdAfx.h"
#include "ImageRecognizeManager.h"

#include "ScanImgManager.h"
#include "ShapeRecognizer.h"
#include "FileHelper.h"
#include "BarCodeRecognizer.h"
#include <opencv2/opencv.hpp>
#include "windows.h"
using namespace std;
using namespace cv;
ImageRecognizeManager::ImageRecognizeManager(void)
{
	mUDP.InitConnection("192.168.1.134",7777);
	//mUDP.InitConnection("192.168.1.163",7777);

}


ImageRecognizeManager::~ImageRecognizeManager(void)
{
	mUDP.CloseConnection();
}

void ImageRecognizeManager::startProcessing(std::string strDbFold,std::string strScanFold)
{


	ScanImgManager newImgScanner;
	newImgScanner.settFoldToScan(strScanFold);

	ShapeRecognizer shapeRecog;
	vector<string>dbImgs;
	//string strDbFold = "images\\database1";
	loadDatabaseImages(strDbFold,dbImgs);
	if(dbImgs.size()==0)
	{
		printf("sample image fold is empty");
		return;
	}
	shapeRecog.extractShapesFromDatabase(dbImgs);
	
	if(shapeRecog.mShapeDb.size()==0)
	{
		printf("fail to extract sample image shapes\n");
		return;
	}

	BarCodeRecognizer barRecog;

	while(true)
	{
		vector<string> newFiles;
		newImgScanner.scanNewFilesToProcess(newFiles);

		for(int i = 0; i < newFiles.size();i++)
		{
			string strImgPath = strScanFold + string("/") + newFiles[i];
			Sleep(1500);
			Mat img = imread(strImgPath);
			//namedWindow("original", WINDOW_NORMAL);
			//imshow("original", img);

			printf("process image:%s ,",strImgPath.c_str());
			shapeRecog.rotateImageToVetricalPos(img);
			//namedWindow("Rotate", WINDOW_NORMAL);
			//imshow("Rotate", img);
			string strBarCode = "";	
			strBarCode = barRecog.recognizeBarCode(img);
			if (strBarCode == "") {
				cout << "change the vertical position of the image" << endl;
				flip(img, img, -1);
				strBarCode = barRecog.recognizeBarCode(img);
			}
			cout << strBarCode << endl;
			string model_name = strBarCode.substr(0, 2);
			cout << "model_name_first = " << model_name << endl;
			string strClass = shapeRecog.recogImageClass(img, model_name);

			//strBarCode = barRecog.recognizeBarCode(img);

			string strResult = strClass;

			if(strClass != "")
			{
				string strRunFold = FileHelper::GetRunningDirectory();
				/*
				add code change the absolute path of the strRunFold
				*/
				//strRunFold = "C:\\Users\\Administrator\\Desktop\\image";
				strRunFold = "F:\\Image";
				/*
					strRunFold
				*/
				cout << "strRunFold = " << strRunFold << endl;
				int pos = strRunFold.find_first_of('\\');
				while(pos!=-1)
				{
					strRunFold.at(pos) = '/';
					pos = strRunFold.find_first_of('\\');
				}
				string strCroppedPng =strRunFold +string("/")+newFiles[i]+string(".png");
				strCroppedPng = strRunFold + string ("/") + newFiles[i] + string(".png");
				/*
				 strCroppedPng
				*/
				cout << "strCroppedPng = " << strCroppedPng << endl;
				imwrite(strCroppedPng,shapeRecog.mCroppedPng);
				strCroppedPng = newFiles[i] + string(".png");
				//strResult = strResult + string(";")+strCroppedPng + string(";");
				strResult = string(";") + strCroppedPng + string(";");
				/*
				change strResult value
				*/
				//strResult = strBarCode.substr(0, 2) + string(";") + strCroppedPng + string(";");

				printf("recognized image class as %s\n",strClass.c_str());
			}
			else
			{
				strResult=" ; ;";
				printf("recognize image class fail\n");
				//if it can not recognize the image, do not send any text to unity
				//return;
			}
			if(strBarCode.length()!=0 && strBarCode.length() > 10 && strClass != "")
			{
				printf("found bar code:%s\n",strBarCode.c_str());
				cout << "strBarCode = " << strBarCode << endl;
				cout << strResult << endl;
				cout << strBarCode.substr(2, strBarCode.length() - 2) << endl;
				strResult =strBarCode.substr(0,2) + strResult + strBarCode.substr(2, strBarCode.length() - 3);
				cout << "strResult = " << strResult << endl;
				if (strResult.size()>7)
					sendStrToUnity(strResult);
			}else{
				cout << "Not found bar code" << endl;
			}
			newImgScanner.updateProcessedFile(newFiles[i]);
			waitKey(0);
		}

	}
	

}



void ImageRecognizeManager::loadDatabaseImages(std::string strDbFold,std::vector<std::string>&strModelImgs)
{

	vector<string> strImgs;
	vector<string> strImgExts;
	strImgExts.push_back("png");
	strImgExts.push_back("jpg");
	FileHelper::GetFiles(strDbFold,strModelImgs,true,strImgExts);
}

void ImageRecognizeManager::sendResultToUnity(std::string strClass,std::string strCroppedImg)
{
	string strData = strClass + string(";") + strCroppedImg;
	mUDP.Send(strData);

}

void ImageRecognizeManager::sendStrToUnity(std::string strCotent)
{	
	mUDP.Send(strCotent);
}