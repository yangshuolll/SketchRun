#include "stdafx.h"
#include "ComponentConnector.h"
#include <stdlib.h>		//for malloc
#include <memory.h>	//for memset

#define c_free(p) if(p){free(p);p=0;}
ComponentConnector::ComponentConnector()
{
	mSuperPixelWidth = 2;
	mSuperPixelHeight = 2;

	mSuperPixMapScanWidth = 2;
	mSuperPixMapScanHeight = 2;

	mIsInited = false;
}


ComponentConnector::~ComponentConnector()
{
}


void ComponentConnector::InitConnectedComponent(int maskWidth, int maskHeight)
{

	if (mIsInited)
	{
//		CCType *ccGst = mpConnectPara;
		stConnPara *ccGst = mpConnectPara;
		if (ccGst->maskWidth != maskWidth || ccGst->maskHeight != maskHeight)
			Relase();
		else
			return;
	}
	mpConnectPara = (stConnPara *)malloc(sizeof(stConnPara));
	stConnPara *ccGst = mpConnectPara;
	int	superPixMapW, superPixMapH;

	ccGst->maskWidth = maskWidth;
	ccGst->maskHeight = maskHeight;

	ccGst->superPixMapW = maskWidth / mSuperPixelWidth;
	ccGst->superPixMapH = maskHeight / mSuperPixelHeight;

	superPixMapW = ccGst->superPixMapW;
	superPixMapH = ccGst->superPixMapH;

	mProcessMaskWidth = superPixMapW;
	mProcessMaskHeight = superPixMapH;

	ccGst->dataSuperPixMap = (unsigned char*)malloc(superPixMapW*superPixMapH*sizeof(unsigned char));
	memset(ccGst->dataSuperPixMap, 0, superPixMapH*superPixMapW*sizeof(unsigned char));

	ccGst->dataSuperPixFG = (unsigned char*)malloc(superPixMapH*superPixMapW*sizeof(unsigned char));
	memset(ccGst->dataSuperPixFG, 0, superPixMapH*superPixMapW*sizeof(unsigned char));

	ccGst->eqCCLabelArray = (unsigned char*)malloc((mMaxLabelNum + 1) * sizeof(unsigned char));

	ccGst->dataSuperPixIntensity = (unsigned char*)malloc(superPixMapH*superPixMapW*sizeof(unsigned char));
	memset(ccGst->dataSuperPixIntensity, 0, superPixMapH*superPixMapW*sizeof(unsigned char));

	ccGst->boundingBoxes = (stBoundingBox *)malloc(mMaxLabelNum*sizeof(stBoundingBox));

	ccGst->avgObjectHeight = 0;
	ccGst->avgObjectWidth = 0;
	ccGst->numTotalObj = 0;
	mIsInited = true;
}

void ComponentConnector::ConnectComponent(unsigned char *dataImgFGMask, int withStep)
{
	stConnPara *ccGst = mpConnectPara;

	int	r, c, i, j, rBlk, cBlk;
	int	imgW, imgH, xA, xB, xC, xD;
	int	cumulatedBlkSum, superPixMapW, superPixMapH;
	int	ctFGPixels, imgDataWStep;
	int	lbVal, currLabel, xUL, yUL, xBR, yBR, R0, C0, R1, C1, objectSize;
	int R0t, C0t, R1t, C1t, tmp;

	int	tmpMaxLabel;

	unsigned char	*ptrImgData;
	unsigned char	*ptrImgData0;
	unsigned char	*ptrSuperPixMap;
	unsigned char	*ptrSuperPixMap0;
	unsigned char	*ptrSuperPixFG;
	unsigned char	*ptrSuperPixFG0;
	unsigned char	*ptrLabelMap;
	unsigned char	*ptrLabelMap0;
	unsigned char	*eqLabelArray;
	unsigned char	*ptrUCHAR;



	int avgObjectWidth;

	imgW = ccGst->maskWidth;
	imgH = ccGst->maskHeight;
	superPixMapW = ccGst->superPixMapW;
	superPixMapH = ccGst->superPixMapH;

	ptrSuperPixMap0 = ccGst->dataSuperPixMap;
	ptrSuperPixFG0 = ccGst->dataSuperPixFG;
	eqLabelArray = ccGst->eqCCLabelArray;

	ptrImgData0 = dataImgFGMask;

	tmpMaxLabel = 0;

	int totalPixelSize = superPixMapH*superPixMapW*sizeof(unsigned char);
	/* reset foreground super pixels.*/
	memset(ccGst->dataSuperPixMap, 0, totalPixelSize);
	memset(ccGst->dataSuperPixFG, 0, totalPixelSize);
	memset(ccGst->dataSuperPixIntensity, 0, totalPixelSize);

	ptrUCHAR = ccGst->eqCCLabelArray;

	for (i = 0; i < (mMaxLabelNum + 1); i += 1)
	{
		*ptrUCHAR = i;
		ptrUCHAR += 1;
	}

	//先统计superPixel Box中非0的个数，存放到ccGst->dataSuperPixFG
	//imgDataWStep = (mSuperPixelHeight * imgW);		//这个是两行了?
	imgDataWStep = mSuperPixelHeight * withStep;

	int patchIntensitySum = 0;
	unsigned char *ptrPatchIntensityImg;
	int patchPixelNum = mSuperPixelHeight*mSuperPixelWidth;

	for (rBlk = 0; rBlk < superPixMapH; rBlk += 1)
	{
		for (cBlk = 0; cBlk < superPixMapW; cBlk += 1)
		{
			ptrImgData = ptrImgData0 + (rBlk * imgDataWStep) + (cBlk * mSuperPixelWidth);
			cumulatedBlkSum = 0;
			patchIntensitySum = 0;
			for (r = 0; r < mSuperPixelHeight; r += 1)
			{
				for (c = 0; c < mSuperPixelWidth; c += 1)
				{
					if ((*ptrImgData) > 0) cumulatedBlkSum += 1;
					
					patchIntensitySum += *ptrImgData;
					
					ptrImgData += 1;
				}/* c */

				//ptrImgData += (imgW - mSuperPixelWidth);//在superPixel中行递增
				ptrImgData += (withStep - mSuperPixelWidth);//在superPixel中行递增
			}/* r */

			*(ptrSuperPixFG0 + (rBlk * superPixMapW) + cBlk) = cumulatedBlkSum;
			*(ccGst->dataSuperPixIntensity + rBlk*superPixMapW + cBlk) = patchIntensitySum / patchPixelNum;
		}/*end of: for ( cBlk )*/
	}/*end of: for ( rBlk )*/


	/**************************/
	/* Scan image for objects */
	/**************************/
	const int nCC_FG_SUPER_PIX_THD = 3;
	//再使用一个滑窗mSuperPixMapScanHeight x mSuperPixMapScanWidth去统计窗口内的非0个数，该非0个数为superPixel窗口内非0个数的累加
	for (r = 0; r < (superPixMapH - mSuperPixMapScanHeight); r += 1)
	{
		for (c = 0; c < (superPixMapW - mSuperPixMapScanWidth); c += 1)
		{
			/* sum up pixel in scanning window.*/

			ctFGPixels = 0;

			ptrSuperPixFG = ptrSuperPixFG0 + (r * superPixMapW) + c;

			for (i = 0; i < mSuperPixMapScanHeight; i += 1)
			{
				for (j = 0; j < mSuperPixMapScanWidth; j += 1)
				{
					ctFGPixels += (*ptrSuperPixFG);
					ptrSuperPixFG += 1;

				}/* col: j */

				ptrSuperPixFG += (superPixMapW - mSuperPixMapScanWidth);
			}/* row: i */


			/* mark foreground.*/

			if (ctFGPixels >= nCC_FG_SUPER_PIX_THD)
			{
				ptrSuperPixMap = ptrSuperPixMap0 + (r * superPixMapW) + c;

				for (i = 0; i < mSuperPixMapScanHeight; i += 1)
				{
					for (j = 0; j < mSuperPixMapScanWidth; j += 1)
					{
						*(ptrSuperPixMap) = 255;
						ptrSuperPixMap += 1;
					}

					ptrSuperPixMap += (superPixMapW - mSuperPixMapScanWidth);
				}

			}/*end of: mark foreground.*/

		}/*end of: for ( c )*/
	}/*end of: for ( r )*/
	//到此为止生成了新的前景图，这步骤其实并非是必须的


	//now the map is saved in ptrSuperPixMap
	/*	Connected component labeling.*/

	currLabel = 1;

	/* put label in [*ptrSuperPixFG].*/
	memset(ccGst->dataSuperPixFG, 0, superPixMapH*superPixMapW*sizeof(unsigned char));
	ptrLabelMap0 = ccGst->dataSuperPixFG;

	/* the 1st pass for setting label and flooding label.*/
	for (r = 1; r < superPixMapH; r += 1)
	{
		ptrSuperPixMap = ptrSuperPixMap0 + (r * superPixMapW) + 1;
		ptrLabelMap = ptrLabelMap0 + (r * superPixMapW) + 1;

		for (c = 1; c<superPixMapW; c += 1)
		{
			if ((*ptrSuperPixMap) > 0)
			{
				/*	xB	xC	xD	*/
				/*	xA	x		*/
				xA = *(ptrLabelMap - 1);
				xB = *(ptrLabelMap - 1 - superPixMapW);
				xC = *(ptrLabelMap - superPixMapW);
				xD = *(ptrLabelMap - superPixMapW + 1);

				lbVal = mMaxLabelNum;

				if (xA > 0)						lbVal = xA;
				if ((xB > 0) && (xB < lbVal))	lbVal = xB;
				if ((xC > 0) && (xC < lbVal))	lbVal = xC;
				if ((xD > 0) && (xD < lbVal))	lbVal = xD;

				if (lbVal < mMaxLabelNum)
				{
					/* make connection and flood label.*/
					if (xA > lbVal)
					{
						*(eqLabelArray + xA) = *(eqLabelArray + lbVal);
					}
					if (xB > lbVal)
					{
						*(eqLabelArray + xB) = *(eqLabelArray + lbVal);
					}
					if (xC > lbVal)
					{
						*(eqLabelArray + xC) = *(eqLabelArray + lbVal);
					}
					if (xD > lbVal)
					{
						*(eqLabelArray + xD) = *(eqLabelArray + lbVal);
					}
				}
				else
				{
					/* new label.*/
					lbVal = currLabel;
					if (currLabel < (mMaxLabelNum - 1))
					{
						currLabel += 1;
					}
				}

				/* save label for this pixel.*/
				*(ptrLabelMap) = lbVal;

			}/*end of: if foreground.*/

			ptrSuperPixMap += 1;
			ptrLabelMap += 1;
		}/* col */
	}/* row */


	/* the 2nd pass to flush with equivalent labels.*/
	for (r = 1; r < superPixMapH; r += 1)
	{
		ptrLabelMap = ptrLabelMap0 + (r * superPixMapW) + 1;

		for (c = 1; c<superPixMapW; c += 1)
		{
			lbVal = (*ptrLabelMap);

			if (lbVal  > 0)
			{
			
				/* equivalent label.*/
				lbVal = *(eqLabelArray + lbVal);
				*ptrLabelMap = lbVal;

				// find the maximal label.
				if (lbVal > tmpMaxLabel)
				{
					tmpMaxLabel = lbVal;
				}

				// display only:
				*(ccGst->dataSuperPixMap + (r * superPixMapW) + c) = lbVal;

			}/*end of: if foreground.*/

			ptrLabelMap += 1;
		}/* col */
	}/* row */

	// save the maximal label value.
	//ccGst->maxID = tmpMaxLabel;

	if (tmpMaxLabel > 0)
	{
		tmpMaxLabel = tmpMaxLabel;
	}

	//determine the bounding boxes of each object
	//boundingBoxes = ccGst->boundingBoxesTmp;
	stBoundingBox *boundingBoxes = ccGst->boundingBoxes;
	for (i = 0; i <= tmpMaxLabel; i++)
	{
		boundingBoxes[i].intensitySum = 0;
		boundingBoxes[i].numPixel = 0;
		boundingBoxes[i].C0 = 1000;
		boundingBoxes[i].R0 = 10000;
		boundingBoxes[i].C1 = -10000;
		boundingBoxes[i].R1 = -10000;
	}


	for (r = 1; r < superPixMapH; r += 1)
	{
		ptrLabelMap = ccGst->dataSuperPixMap + (r * superPixMapW) + 1;

		for (c = 1; c<superPixMapW; c += 1)
		{
			lbVal = *ptrLabelMap;
			ptrLabelMap++;

			if (lbVal >= 1)
			{
				if (lbVal >(mMaxLabelNum - 1)) lbVal = mMaxLabelNum - 1;

				lbVal = lbVal - 1;
				boundingBoxes[lbVal].numPixel += 1;
				if (r < boundingBoxes[lbVal].R0)  boundingBoxes[lbVal].R0 = r;
				if (c < boundingBoxes[lbVal].C0)  boundingBoxes[lbVal].C0 = c;
				if (r > boundingBoxes[lbVal].R1)  boundingBoxes[lbVal].R1 = r;
				if (c > boundingBoxes[lbVal].C1)  boundingBoxes[lbVal].C1 = c;
			
				boundingBoxes[lbVal].intensitySum += *(ccGst->dataSuperPixIntensity + r*superPixMapW + c);
			}

		}
	}

	int filterSizeMin = 8;
	ccGst->numTotalObj = FilterBoxWithPixelSize(boundingBoxes, tmpMaxLabel, filterSizeMin, 0);
}
int ComponentConnector::FilterBoxWithPixelSize(stBoundingBox *pBoxes, int numSrc, int filterSizeMin, int filterSizeMax)
{
	int numDst = 0;
	for (int i = 0; i < numSrc; i++)
	{
		if (pBoxes[i].numPixel <filterSizeMin || (filterSizeMax != 0 && pBoxes[i].numPixel < filterSizeMax))
			continue;
		else
		{
			pBoxes[numDst] = pBoxes[i];
			numDst += 1;
		}
	}
	return numDst;
}

stBoundingBox*ComponentConnector::GetBoundingBoxes(int &numObjs, float &scaleX, float &scaleY)
{

	numObjs = mpConnectPara->numTotalObj;
	scaleX = mSuperPixelWidth;
	scaleY = mSuperPixelHeight;
	return mpConnectPara->boundingBoxes;
}

void ComponentConnector::Relase()
{
	stConnPara *ccGst = mpConnectPara;

	c_free(ccGst->dataSuperPixFG);
	c_free(ccGst->dataSuperPixMap);
	c_free(ccGst->eqCCLabelArray);
	c_free(ccGst->dataSuperPixIntensity);
	c_free(ccGst->boundingBoxes);
	c_free(ccGst);
	
}