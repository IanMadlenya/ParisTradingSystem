#pragma once
#include <iostream>
#include <stdio.h>
#include <string> //for std::string, we use string::operator= in friend function
#include "Ticker.h" 

using namespace std;
class EachStock
{
private:
	string Ticker;
	double price;
	int size;
	string sDate;
public:
	EachStock(void) { }
	~EachStock(void){}
	EachStock(string Ticker_, double p, int s, string pDate)
	{
		Ticker = Ticker_;
		price = p;
		size = s;
		sDate = pDate;
	}
	EachStock(const EachStock &each)
	{
		Ticker = each.Ticker;
		price = each.price;
		size = each.size;
		sDate = each.sDate;
	}
	string sGetDate(void) { return sDate; }
	void SetDate(string pDate) { sDate = pDate; }
	double fGetPrice(void) { return price; }
	int fGetSize(void) { return size; }
};

class Trade
{
private:
	int iPairID;
	string sTicker1;
	string sTicker2;
	double price1, price2;
	int size1, size2;
	string sDate1, sDate2;
	int status;
	double fProfit;
public:
	Trade(void);
	~Trade(void);
	Trade(int id, string pTicker1, string pTicker2, double p1, double p2, int s1, int s2, string pDate1, string pDate2);//, float profit: external pass 0.0
	Trade(const Trade &tr);

	int iGetPairID(void);
	string sGetTicker1(void);
	void SetTicker1(string pTicker1);
	string sGetTicker2(void);
	void SetTicker2(string pTicker2);
	string sGetDate1(void);
	string sGetDate2(void);
	double fGetPrice1(void);
	double fGetPrice2(void);
	int fGetSize1(void);
	int fGetSize2(void);

	double fGetProfit(void);
	void SetProfit(double profit);
	void SetStatus(int status_) { status = status_; }
	int fGetStatus(void);
	friend ostream & operator<<(ostream &out, const Trade & aTrade);
};



inline int Trade::iGetPairID(void)
{
	return iPairID;
}


inline string Trade::sGetTicker1(void)
{
	return sTicker1;
}

inline void Trade::SetTicker1(string pTicker1)
{
	sTicker1 = pTicker1;
}

inline string Trade::sGetTicker2(void)
{
	return sTicker2;
}

inline void Trade::SetTicker2(string pTicker2)
{
	sTicker2 = pTicker2;
}

inline string Trade::sGetDate1(void)
{
	return sDate1;
}

inline string Trade::sGetDate2(void)
{
	return sDate2;
}

inline int Trade::fGetSize1(void)
{
	return size1;
}
inline int Trade::fGetSize2(void)
{
	return size2;
}

inline double Trade::fGetPrice1(void)
{
	return price1;
}

inline double Trade::fGetPrice2(void)
{
	return price2;
}

inline double Trade::fGetProfit(void)
{
	return fProfit;
}

inline void Trade::SetProfit(double profit)
{
	fProfit = profit;
}

inline int Trade::fGetStatus(void)
{
	return status;
}