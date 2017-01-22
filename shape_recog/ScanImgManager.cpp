#include "StdAfx.h"
#include "ScanImgManager.h"
#include "FileHelper.h"
#include "stringEx.h"

using namespace std;
ScanImgManager::ScanImgManager(void)
{
	mLastFileCreateTime = 0;
}


ScanImgManager::~ScanImgManager(void)
{
}

void ScanImgManager::settFoldToScan(std::string strScanFold)
{
	mScanningFold = strScanFold;
	loadProcessedImages(strScanFold);

}



void ScanImgManager::loadProcessedImages(std::string strScanFold)
{
	mProcessImgFlag.clear();

	string strProcessedLog = strScanFold + string("/processed.log");
	string strContent = FileHelper::ReadString(strProcessedLog);
	if(strContent.size()<3) //minimum string s like :1.png or 1.jpg
		return;

	stringEx exContent(strContent);
	vector<stringEx> lines = exContent.SplitToStrings('\n');
	for(int i = 0;i<lines.size();i++)
	{
		string strLine = lines[i].GetString();
		int pos = strLine.find_last_of('.');
		if(pos > 0)
			mProcessImgFlag[strLine] = 1;
	}
}

void ScanImgManager::scanNewFilesToProcess(std::vector<std::string> &newFiles)
{
	vector<string> strExts;
	strExts.push_back("png");
	strExts.push_back("jpg");

	std::vector<string> foundFiles;
	FileHelper::GetFiles(mScanningFold,foundFiles,false,strExts,mLastFileCreateTime);
	for(int i = 0; i < foundFiles.size();i++)
	{
		string strFilePath = mScanningFold + std::string("/") + foundFiles[i];
		__time64_t t = FileHelper::GetFileCreateTime(strFilePath);
		if(t > mLastFileCreateTime)
			mLastFileCreateTime = t;
		if(mProcessImgFlag.count(foundFiles[i])==0)
		{
			newFiles.push_back(foundFiles[i]);
		}
	}
}

void ScanImgManager::updateProcessedFile(std::string strFile)
{
	mProcessImgFlag[strFile] = 1;

	string strProcessedLog = mScanningFold + string("/processed.log");
	FILE *fp;
	fopen_s(&fp,strProcessedLog.c_str(),"a+");
	if(fp != nullptr)
	{
		fprintf(fp,"%s\n",strFile.c_str());
		fclose(fp);
	}
}