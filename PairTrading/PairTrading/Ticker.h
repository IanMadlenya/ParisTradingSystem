#pragma once
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;

const int TICKER_LEN = 11;
const int COMP_NAME_LEN = 255;
const int DATE_LEN = 11;

//user-defined struct
struct char_array {
public:
	char DateChar[DATE_LEN];
	/////////////try to delete
	char_array(const char_array &Input)//used VectorTicker.push_back(OneTicker); or we do not need this function, it is already deep copy
	{
		strncpy_s(DateChar, Input.DateChar, DATE_LEN);
	}
	char_array() { memset(DateChar, '\0', DATE_LEN); }
};

class Ticker
{
private:
	string sTicker;
	vector<double> fTickerOpen;
	vector<double> fTickerClose;
	char sDate[DATE_LEN];
	vector<char_array> Dates;
	char_array arra;
	
public:
	Ticker(void);
	Ticker(const Ticker &aTicker);
	Ticker(string pTicker);
	bool operator==(const Ticker &aTicker);
	~Ticker(void);

	void SetTradeInfo(double open, double close, char *Date)
	{
		fTickerOpen.push_back(open);
		fTickerClose.push_back(close);
		strncpy_s(arra.DateChar, Date, DATE_LEN);
		Dates.push_back(arra);
	}
	vector <double> GetOpen() { return fTickerOpen; }
	vector <double> GetClose() { return fTickerClose; }
	vector <char_array> GetDates() { return Dates; }
	string GetTicker() { return sTicker; }
};

