#pragma once
#include <string>
#include <atlbase.h>
#include "KompasUtility.h"
using namespace std;


string BSTRToString(BSTR bstr);

BSTR CStringToBSTR(const std::string& str);

double roundToThreeDecimalPlaces(double value);

int roundToInt(double value);