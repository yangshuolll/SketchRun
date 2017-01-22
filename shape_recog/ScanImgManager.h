#pragma once
#include <string>
#include <vector>
#include <map>
class ScanImgManager
{
public:
	ScanImgManager(void);
	~ScanImgManager(void);

	void settFoldToScan(std::string strScanFold);

	void scanNewFilesToProcess(std::vector<std::string> &newFiles);

	void loadProcessedImages(std::string strScanFold);
	void updateProcessedFile(std::string strFile);

	std::string mScanningFold;	
	std::map<std::string,int>mProcessImgFlag;
	__time64_t mLastFileCreateTime;
};

