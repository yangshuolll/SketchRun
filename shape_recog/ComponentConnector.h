#pragma once

typedef struct _stBoundingBox
{
	int numPixel;
	int intensitySum;
	int C0;
	int R0;
	int C1;
	int R1;
	int area()const
	{
		return (C1 - C0)*(R1 - R0);
	}
	bool operator ==(const _stBoundingBox &rhb)const
	{
		return C0 == rhb.C0 && C1 == rhb.C1 && R0 == rhb.R0 &&R1 == rhb.R1 &&
			numPixel == rhb.numPixel && intensitySum == rhb.intensitySum;
	}
}stBoundingBox;

typedef struct _stConnPara
{
	int	maskWidth;
	int	maskHeight;
	int ccSuperPixelSize;			//patch的大小
	int	superPixMapW;				//patch的横向个数
	int	superPixMapH;				//patch的纵向个数


	unsigned char*		dataSuperPixFG;			//统计小patch内的非0个数
	unsigned char*		dataSuperPixMap;		//小patch内非0个数大于阈值的话将该小patch内像素值设为255
	
	unsigned char*		eqCCLabelArray;
	unsigned char*		dataSuperPixIntensity;	//原始mask图的灰度值，这里存放的是缩放后的patch的均值

	stBoundingBox *boundingBoxes;
	int	numTotalObj;

	double avgObjectWidth;
	double avgObjectHeight;



}stConnPara;

class ComponentConnector
{
public:
	ComponentConnector();
	~ComponentConnector();

	//CCType *mpConnectPara;
	stConnPara *mpConnectPara;
	int mProcessWidth;
	int mProcessHeight;

	int mSuperPixelWidth;			//超像素的宽
	int mSuperPixelHeight;

	int mProcessMaskWidth;
	int mProcessMaskHeight;

	int mSuperPixMapScanWidth;
	int mSuperPixMapScanHeight;

	static const int mMaxLabelNum = 200;

	int mFilterSizeMin;


	bool mIsInited;

	void InitConnectedComponent(int maskWidth, int maskHeight);
	
	void ConnectComponent(unsigned char *dataImgFGMask,int withStep);
	
	stBoundingBox *GetBoundingBoxes(int &numObjs,float &scaleX,float &scaleY);


	
int FilterBoxWithPixelSize(stBoundingBox *pBoxes, int numSrc, int filterSizeMin, int filterSizeMax);

void Relase();

};

