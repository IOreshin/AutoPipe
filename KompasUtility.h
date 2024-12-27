#pragma once
#include <string>
#include <atlbase.h>
using namespace std;

string BSTRToString(BSTR bstr);

BSTR CStringToBSTR(const std::string& str);