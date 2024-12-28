#include <iostream>
#include <string>
#include <atlbase.h>
#include "pch.h"
#include <comutil.h>  // ��� ������ � BSTR
#include <afxstr.h>    // ��� CString
using namespace std;

//������� �� �������� BSTR � STRING
string BSTRToString(BSTR bstr) {
	int len = SysStringLen(bstr);
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, bstr, len, NULL, 0, NULL, NULL);
	string str(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, bstr, len, &str[0], sizeNeeded, NULL, NULL);
	return str;
}

//������� �� �������� STRING � BSTR
BSTR CStringToBSTR(const CString& cstr)
{
	// �������� ������ ��� const wchar_t* � ������� BSTR
	BSTR bstr = SysAllocString(cstr.GetString());  // �������� ������ �� CString
	return bstr;
}

double roundToThreeDecimalPlaces(double value) {
	return round(value * 1000.0) / 1000.0;
}