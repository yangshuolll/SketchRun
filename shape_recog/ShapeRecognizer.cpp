#include "StdAfx.h"
#include "ShapeRecognizer.h"
#include <opencv2/shape.hpp>
#include "FileHelper.h"
#include "sstream"
using namespace cv;
using namespace std;
ShapeRecognizer::ShapeRecognizer(void)
{
	mProcessImgWidth = 320;
	mRows = 2343;
	mCols = 1650;
}


ShapeRecognizer::~ShapeRecognizer(void)
{
}

void ShapeRecognizer::computeImageGradient(cv::Mat img, cv::Mat &gradImg)
{
	Canny(img,gradImg,50,80);
	return;

	Mat src;
	cvtColor(img,src,COLOR_BGR2GRAY);
	Mat dst_x, dst_y, dst;
	Sobel(src, dst_x, src.depth(), 1, 0);
	Sobel(src, dst_y, src.depth(), 0, 1);
	convertScaleAbs(dst_x, dst_x);
	convertScaleAbs(dst_y, dst_y);
	addWeighted( dst_x, 0.5, dst_y, 0.5, 0, dst);	
	gradImg = dst.clone();
}

std::string ShapeRecognizer::recogImageClass(cv::Mat img, string model_name)
{
	//***before change***
	std::string strClass = "";
	cout << "model_name = " << model_name << endl;
	int model_num;
	std::stringstream ss;
	ss << model_name;
	ss >> model_num;
	cout << "model_num = " << model_num << endl;
	if (model_num < 10 || model_num >24) {
		return "";
	}
	
	
	
	
	//***after change***
	resize(img, img, Size(mCols, mRows));
	int useWidth = mProcessImgWidth;
	cout << "img.rows = " << img.rows << endl;
	cout << "img.cols = " << img.cols << endl;

	int useHeight = img.rows*useWidth/img.cols;

	resize(img,img,Size(useWidth,useHeight));
	//imshow("12", img);
	//***before change***
	//vector<Point> srcCs,shuffleCs;
	
	
	//detectContours(img,srcCs,shuffleCs);

	//if(shuffleCs.size()==0)
	//	return strClass;

	extractIPImage(img, mShapeDb[model_name], mCroppedPng);
	//***after change***

	//***before change***
	//cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();

	//float minMatchingDist = 1e8;
	//int minMatchDsitIndex = -1;
	//for(int i = 0; i < mShapeNames.size();i++)
	//{
	//	string strImgName = mShapeNames[i];
	//	vector<Point> &dbShape = mShapeDb[strImgName];
	//	float dis = mysc->computeDistance( shuffleCs, dbShape );
	//	/*int temp = 20;
	//	if (temp < shuffleCs.size()) {
	//		for(int i = 0; i < temp; i++){
	//			cout << "shuffleCs[" << i << "] = " << shuffleCs[i] << "   " << "dbShape[" << i << "] = " << dbShape[i] << endl;
	//		}
	//	}
	//	*/
	//	if(dis<minMatchingDist)
	//	{
	//		minMatchingDist = dis;
	//		minMatchDsitIndex = i;
	//	}
	//}


	//if(minMatchingDist< 0.2)
	//{
	//	strClass = mShapeNames[minMatchDsitIndex];
	//	printf("matched image name is :%s,dist=%.2f\n",mShapeNames[minMatchDsitIndex].c_str(),minMatchingDist);
	//}
	//printf("-----------------------------press any key to continue\n");
	//imshow("img",img);
	//***after change***
	
	return model_name;
	
	//return 1;
}

void ShapeRecognizer::extractIPImage(cv::Mat img,std::vector<cv::Point> &srcContours ,cv::Mat &png)
{
	//get bounding box
	

	cv::Rect rct = getBoundingBox(srcContours);
	cout << rct << endl;
	cout << img.cols << "  " << img.rows << endl;
	rct = resizeBoundingBox(rct,img.cols,img.rows);
	cout << rct << endl;
	rectangle(img, rct, Scalar(0, 0, 255), 3);
	//namedWindow("Tom", WINDOW_NORMAL);
	//imshow("Tom", img);
	
	Mat subImg = img(rct).clone();

	std::vector<cv::Point> subContours;
	for(int i = 0; i < srcContours.size(); i++)
	{
		cv::Point pt = srcContours[i];
		pt.x -= rct.x;
		pt.y -= rct.y;
		subContours.push_back(pt);
	}
	Mat mask = Mat::zeros(Size(rct.width,rct.height),CV_8UC1);
	std::vector<std::vector<cv::Point>> subCCs;
	subCCs.push_back(subContours);
	drawContours(mask,subCCs,0,Scalar(25,255,255),-1);

	png = Mat::zeros(subImg.size(),CV_8UC4);
	for(int r = 0; r < subImg.rows;r++)
	{
		uchar *ptrImgRow = subImg.ptr<uchar>(r);
		uchar *ptrAlphaRow = mask.ptr<uchar>(r);
		uchar *ptrPngRow = png.ptr<uchar>(r);
		for(int c = 0; c < subImg.cols;c++)
		{
			ptrPngRow[4*c+0] = ptrImgRow[3*c+0];
			ptrPngRow[4*c+1] = ptrImgRow[3*c+1];
			ptrPngRow[4*c+2] = ptrImgRow[3*c+2];
			ptrPngRow[4*c+3] = ptrAlphaRow[c]==0?0:255;
		}
	}
	int debug =0;
	//imshow("png",png);
	//imshow("mask",mask);


}
void ShapeRecognizer::detectContours(cv::Mat img,std::vector<cv::Point> &srcContours,std::vector<cv::Point> &shuffleContours )
{
	shuffleContours.clear();
	srcContours.clear();

	Mat grad;
	computeImageGradient(img,grad);
	
	threshold(grad,grad,40,255,cv::THRESH_BINARY);

	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));
	//dilate(grad,grad,element);
	dilate(grad, grad, element);
	erode(grad,grad,element);

	//imshow("grad",grad);


	vector<vector<Point> > _contoursQuery;
	
	findContours(grad, _contoursQuery, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	int numMaxCs = 0;
	int maxCsIdx = 0;
	for(int i = 0; i < _contoursQuery.size(); i++)
	{
		if(_contoursQuery[i].size() > numMaxCs)
		{
			numMaxCs = _contoursQuery[i].size();
			maxCsIdx = i;
		}
	}

	 int n = 60;
	std::vector<Point> tempCS;
	 approxPolyDP(Mat( _contoursQuery[maxCsIdx]), tempCS, arcLength(Mat(_contoursQuery[maxCsIdx]), true)*0.002, true);
	 
	 if(tempCS.size()<n)
	 {
		 approxPolyDP(Mat( _contoursQuery[maxCsIdx]), tempCS, arcLength(Mat(_contoursQuery[maxCsIdx]), true)*0.001, true);		
	 }


	 srcContours = tempCS;

	 if(tempCS.size()<n/2)
	 {
		 printf("contour point is not enough\n");
		 return;
	 }

	 vector<vector<Point>>ccs;
	 ccs.push_back(tempCS);
	 //drawContours(img,ccs,0,Scalar(0,0,255),2);
	 //imshow("img",img);

	 int dummy=0;
	
	if(tempCS.size()< n)
	{
		std::vector<Point> tempCSWithDummy;
		tempCSWithDummy.resize(n);
		int srcLen = tempCS.size();
		for(int i = 0; i < n;i++)
			tempCSWithDummy[i] = tempCS[i%srcLen];
		tempCS.clear();
		tempCS = tempCSWithDummy;
	}
	
	

	 // Uniformly sampling
	 random_shuffle(tempCS.begin(), tempCS.end());
	 vector<Point> cont;
	 for (int i=0; i<n; i++)
	 {
		 shuffleContours.push_back(tempCS[i]);
	 }
	
}

void ShapeRecognizer::extractShapesFromDatabase(std::vector<std::string> &strImgs)
{
	mShapeDb.clear();
	mShapeNames.clear();

	//int sumWidth = 0;
	// sumHeight = 0;
	for(int i = 0; i < strImgs.size(); i++)
	{
		string strImg = strImgs[i];
		Mat img = imread(strImg,0);
		resize(img, img, Size(mCols, mRows));
		int useWidth = mProcessImgWidth;
		int useHeight = img.rows*useWidth/img.cols;
		//int useWidth = mCols;
		//int useHeight = mRows;
		//sumWidth += img.rows;
		//sumHeight += img.cols;
		resize(img,img,Size(useWidth,useHeight));
		//imshow("test", img);
		int pos = strImg.find_last_of('\\');
		//extract image name from image path
		string strImgName = strImg.substr(pos+1,strImg.length()-pos-1);
		pos = strImgName.find_last_of('.');
		if(pos != -1)
			strImgName = strImgName.substr(0,pos);

		vector<cv::Point> shuffleCs;
		vector<Point> &srcCs = mShapeDb[strImgName];
		detectContours(img,srcCs,shuffleCs);
		/*
			getting the correct the contour from the database
		*/
		/*	
		vector<vector<Point>> temp;
		temp.clear();
		temp.push_back(srcCs);
		drawContours(img, temp, 0, Scalar(0, 0, 255), 3);
		imshow(strImgName, img);
		*/
		if(shuffleCs.size()>0)
		{
			mShapeNames.push_back(strImgName);
			printf("data base %s setup done\n",strImgName.c_str());
		}
		else
		{
			printf("data base %s setup failed\n",strImgName.c_str());
		}
		//pos = strImgName.find_last_of('.');
		//strImgName = strImg.substr(0,pos);
		
		/*
		Mat tempImg = imread(strImg);
		resize(tempImg,tempImg,Size(useWidth,useHeight));
		vector<vector<Point>>ccs;
		ccs.push_back(cs);
		drawContours(tempImg,ccs,0,Scalar(0,0,255),2);
		imshow("img",tempImg);
		waitKey(0);*/
	}	
	//cout << "sum_width = " << sumWidth << endl;
	//cout << "sum_height = " << sumHeight << endl;
	//cout << "sumSize = " << strImgs.size() << endl;
}

cv::Rect ShapeRecognizer::getBoundingBox(const std::vector<cv::Point> &contours)
{
	int leftx=10000,topy=10000,rightx=0,bottomy=0;
	for(int i = 0; i < contours.size();i++)
	{
		cv::Point pt = contours[i];
		//cout << "Point = " << pt << endl;
		if(pt.x < leftx)
			leftx = pt.x;
		if(pt.x > rightx)
			rightx = pt.x;

		if(pt.y < topy)
			topy = pt.y;
		if(pt.y > bottomy)
			bottomy = pt.y;
	}

	cv::Rect rct(leftx,topy,rightx-leftx+1,bottomy-topy+1);
	return rct;
}

cv::Rect ShapeRecognizer::resizeBoundingBox(cv::Rect rct,int imgWidth,int imgHeight)
{
	int centerX = rct.x + rct.width/2;
	int centerY = rct.y + rct.height/2;

	int maxSize = rct.width>rct.height?rct.width:rct.height;
	maxSize+=10; //expan 10 pixels

	if(centerX+maxSize/2>=imgWidth)
		maxSize = 2*(imgWidth-centerX-1);
	if(centerY+maxSize/2>=imgHeight)
		maxSize = 2*(imgHeight-centerY-1);

	if(centerX-maxSize/2<0)
		maxSize=centerX*2;
	if(centerY-maxSize/2<0)
		maxSize=centerY*2;

	Rect newRct;
	newRct.x = centerX-maxSize/2;
	newRct.y = centerY-maxSize/2;
	newRct.width = maxSize;
	newRct.height = maxSize;

	return newRct;



}

void ShapeRecognizer::rotateImageToVetricalPos(cv::Mat &img)
{
	// rotate counter clockwise 90 degrees
	if (img.rows < img.cols) {
		transpose(img, img);
		flip(img, img, 0);
		cout << "rotate contour clockwise 90 degrees" << endl;
	}
}


