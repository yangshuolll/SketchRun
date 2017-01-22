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
	int ccSuperPixelSize;			//patch�Ĵ�С
	int	superPixMapW;				//patch�ĺ������
	int	superPixMapH;				//patch���������


	unsigned char*		dataSuperPixFG;			//ͳ��Сpatch�ڵķ�0����
	unsigned char*		dataSuperPixMap;		//Сpatch�ڷ�0����������ֵ�Ļ�����Сpatch������ֵ��Ϊ255
	
	unsigned char*		eqCCLabelArray;
	unsigned char*		dataSuperPixIntensity;	//ԭʼmaskͼ�ĻҶ�ֵ�������ŵ������ź��patch�ľ�ֵ

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

	int mSuperPixelWidth;			//�����صĿ�
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

