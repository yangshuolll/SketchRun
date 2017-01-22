#include "BarCodeRecognizer.h"

#pragma comment(lib,"lib/libzbar-0.lib")

using namespace std;    
using namespace zbar;  
using namespace cv;

BarCodeRecognizer::BarCodeRecognizer(void)
{
	mBarScanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);  
}


BarCodeRecognizer::~BarCodeRecognizer(void)
{
}


std::string  BarCodeRecognizer::recognizeBarCode(cv::Mat img)
{
	std::string strCodeValue;
	/*
	Mat imageGray = img.clone();	
	cvtColor(imageGray,imageGray,COLOR_BGR2GRAY);    

	Mat barImg;
	locateBarCode(imageGray,barImg);

	if(barImg.data==0)
		return strCodeValue;
	
	imshow("barImg",barImg);

	int width = barImg.cols;    
	int height = barImg.rows;    
	uchar *raw = (uchar *)barImg.data;       
	Image imageZbar(width, height, "Y800", raw, width * height);      
	mBarScanner.scan(imageZbar);

	

	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if(imageZbar.symbol_begin()==imageZbar.symbol_end())
	{
		//scan fail
		return strCodeValue;
	}

	
	for(;symbol != imageZbar.symbol_end();++symbol)  
	{    
		mstrCodeType = symbol->get_type_name();
		strCodeValue = symbol->get_data();
		break;//only scan one bar code
	}    
	*/
	/*
	get the bottom right corner of the barcode;
	*/
	int _height = img.rows;
	int _width = img.cols;
	int ceil_height = _height / 6;
	int ceil_width = _width / 3;
	Rect rect(ceil_width * 2, ceil_height * 5, ceil_width, ceil_height);
	img(rect).copyTo(img);
	//imshow("original", img);
	/*
		initialize the barcode scanner
	*/
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	/*
	Mat image = imread("IP\\licuilian.jpg");
	if (!image.data)
	{
		cout << "请确认图片" << endl;
		system("pause");
		return 0;
	}
	*/
	Mat imageGray;
	cvtColor(img, imageGray, CV_RGB2GRAY);
	int width = imageGray.cols;
	int height = imageGray.rows;
	uchar *raw = (uchar *)imageGray.data;
	Image imageZbar(width, height, "Y800", raw, width * height);
	scanner.scan(imageZbar); //扫描条码      
	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		cout << "查询条码失败，请检查图片！" << endl;
		strCodeValue = "";
		imageZbar.set_data(NULL, 0);
		return strCodeValue;
	}
	int sum = 0;
	symbol = imageZbar.symbol_begin();
	for (; symbol != imageZbar.symbol_end(); ++symbol)
	{
		//cout << "sum = " << sum++ << endl;
		cout << "类型1：" << endl << symbol->get_type_name() << endl << endl;
		cout << "条码1：" << endl << symbol->get_data() << endl << endl;
		strCodeValue = symbol->get_data();
		break;
	}
	
	//namedWindow("Source Image", WINDOW_NORMAL);
	//imshow("Source Image", img);
	waitKey();
	
	imageZbar.set_data(NULL, 0);
	//return 0;

	return strCodeValue;
}

stBoundingBox mergeBox(const stBoundingBox &a, const stBoundingBox &b)
{
	stBoundingBox newBox;
	newBox.R0 = std::min(a.R0, b.R0);
	newBox.C0 = std::min(a.C0, b.C0);

	newBox.R1 = std::max(a.R1, b.R1);
	newBox.C1 = std::max(a.C1, b.C1);


	newBox.numPixel = a.numPixel + b.numPixel;
	newBox.intensitySum = a.intensitySum + b.intensitySum;
	return newBox;
}
stBoundingBox clusterBox(vector<stBoundingBox> &boxes)
{
	if(boxes.size()==1)
		return boxes[0];

	vector<vector<stBoundingBox>>clusteredBoxes;
	vector<stBoundingBox>anchor;
	anchor.push_back(boxes[0]);
	clusteredBoxes.push_back(anchor);
	for(int i = 1; i < boxes.size();i++)
	{		
		int found = 0;
		for(int k = 0; k < clusteredBoxes.size();k++)
		{
			vector<stBoundingBox>&anchor = clusteredBoxes[k];
			int anchorHeight = anchor[0].R1-anchor[0].R0+1;
			int curHeight = boxes[i].R1-boxes[i].R0+1;
			int diffX0 = abs(boxes[i].C0-anchor[0].C1);
			int diffX1 = abs(boxes[i].C1-anchor[0].C0);
			int minDiffX = std::min(diffX0,diffX1);
			int diffY0 = abs(boxes[i].R0-anchor[0].R0);
			int heightDiff = abs(anchorHeight-curHeight);
			if(minDiffX<curHeight/2 && heightDiff<3 && diffY0< 3)
			{
				anchor.push_back(boxes[i]);
				found = 1;
				break;
			}			
		}
		if(!found)
		{
			vector<stBoundingBox>anchor;
			anchor.push_back(boxes[i]);
			clusteredBoxes.push_back(anchor);
		}
	}
	stBoundingBox maxAreaBox;
	int maxArea=0;
	for(int i = 0; i < clusteredBoxes.size();i++)
	{
		vector<stBoundingBox>&anchor = clusteredBoxes[i];
		stBoundingBox merBox = anchor[0];
		for(int c=1;c<anchor.size();c++)
		{
			merBox = mergeBox(merBox,anchor[c]);
		}
		if( merBox.area()>maxArea)
		{
			maxArea = merBox.area();
			maxAreaBox = merBox;
		}			
	}
	return maxAreaBox;
}

void BarCodeRecognizer::locateBarCode(cv::Mat img,cv::Mat &barImg)
{
	Mat tempImg;
	int useWidth = 320;
	int useHeight = img.rows*useWidth/img.cols;

	resize(img,tempImg,Size(useWidth,useHeight));
	 
	Mat binaryImg;
	threshold(tempImg,binaryImg,5,255,THRESH_BINARY_INV);
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 3));
	//imshow("element", element);
	dilate(binaryImg,binaryImg,element);
	element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(binaryImg,binaryImg,element);
	//imshow("binary",binaryImg);

	//waitKey(0);
	

	mConnector.InitConnectedComponent(binaryImg.cols,binaryImg.rows);
	mConnector.ConnectComponent(binaryImg.data, binaryImg.step[0]);			//在这里的时候会把尺度再次缩小2倍

	int numObj = 0;
	float scaleX, scaleY;
	stBoundingBox *pBoxes = mConnector.GetBoundingBoxes(numObj, scaleX, scaleY);

	vector<stBoundingBox> boxes;
	for(int i = 0; i < numObj; i++)
	{
		stBoundingBox *pBox = pBoxes+i;
		int w = pBox->C1 - pBox->C0+1;
		int h = pBox->R1 - pBox->R0 + 1;
		int centerY = (pBox->R0 + pBox->R1)*scaleY/2;
		int testPos = (centerY>binaryImg.rows*0.3) && (centerY<binaryImg.rows*0.7);
		if(pBox->numPixel < pBox->area()*0.7 ||w*h<10||h<5)
			continue;
		if(testPos)
			continue;
		Rect rct( pBox->C0*scaleX,pBox->R0*scaleX,w*scaleX,h*scaleY);
		rectangle(binaryImg,rct,Scalar(255,0,0),3);
		boxes.push_back(*pBox);		
	}

	if(boxes.size()==0)
		return;

	//cluster boxes

	//chose one best
	stBoundingBox finalBox;
	/*float maxFillRate = 0;
	for(int i = 0; i< boxes.size();i++)
	{
		float fillRate = float(boxes[i].numPixel)/boxes[i].area();
		if(fillRate>maxFillRate)
		{
			maxFillRate = fillRate;
			finalBox = boxes[i];
		}
	}*/
	
	finalBox = clusterBox(boxes);
	float s1 = (float)(img.cols)/useWidth;
	int expanSize = s1*5;

	int w = finalBox.C1 - finalBox.C0+1;
	int h = finalBox.R1 - finalBox.R0 + 1;
	int x1 = finalBox.C0*scaleX*s1 - expanSize;
	int y1 = finalBox.R0*scaleY*s1 - expanSize;
	int w1 = w*scaleX*s1 + expanSize*2;
	int h1 = h*scaleY*s1 + expanSize*2;

	if(x1+w1>=img.cols)
		x1 = img.cols-w1-1;
	if(y1+h1>=img.rows)
		y1 = img.rows-h1-1;

	Rect srcRect(x1,y1,w1,h1);

	barImg = img(srcRect).clone();
	
	//imshow("binary",binaryImg);
	//imshow("bar",barImg);

	//waitKey(0);

}