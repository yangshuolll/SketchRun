#pragma once

/****************************************************************
Author:	Justin Ko
Date :		2013-7-23
Description: ����std::string����չ����
*****************************************************************/




#include <string>
#include <vector>

class stringEx
{
public:
	stringEx(void);
	stringEx(char *pStr);
	stringEx(std::string str);
	void Trim();
	void TrimLeft();
	void TrimRight();
	void MakeLower();
	void MakeUpper();
	stringEx ReadLine(std::string strMark);//strMark������ʼ��
	stringEx PopLine(std::string strMark);
	stringEx PopLine();
	
	//ȥ�����ұ߷���ĸ�ķ��ţ���ո񣬻��з�	
	void TrimNoCharacter();

	void Erase(size_t nOffset,size_t nLenght);
	std::vector<stringEx> SplitToStrings(char seperator);
	std::vector<stringEx> SplitToStringsEx(std::string seperator);
	std::vector<int> GetSeperatorPos(char seperator);
	std::vector<int> GetSeperatorPosEx(std::string seperator);
	size_t FindStrings(std::string strMark);
	std::string GetString();
	void Copy(std::string str);
	std::vector<float>SplitToFloats(char seperator);
	std::vector<int> SplitToIntegers(char seperator);
	void ReplaceCharacter(char srcChar,char newChar);

		
	double ToDigital();
	operator std::string()
	{
		return m_str;
	}
	~stringEx(void);

private:
	std::string m_str;
};
