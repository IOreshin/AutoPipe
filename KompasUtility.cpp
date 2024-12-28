#include <iostream>
#include <string>
#include <atlbase.h>
#include "pch.h"
#include <comutil.h>  // Для работы с BSTR
#include <afxstr.h>    // Для CString
using namespace std;

//Функция по переводу BSTR в STRING
string BSTRToString(BSTR bstr) {
	int len = SysStringLen(bstr);
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, bstr, len, NULL, 0, NULL, NULL);
	string str(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, bstr, len, &str[0], sizeNeeded, NULL, NULL);
	return str;
}

//Функция по переводу STRING в BSTR
BSTR CStringToBSTR(const CString& cstr)
{
	// Получаем строку как const wchar_t* и создаем BSTR
	BSTR bstr = SysAllocString(cstr.GetString());  // Получаем строку из CString
	return bstr;
}

double roundToThreeDecimalPlaces(double value) {
	return round(value * 1000.0) / 1000.0;
}