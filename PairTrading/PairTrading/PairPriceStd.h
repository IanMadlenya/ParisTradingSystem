#pragma once
#include <iostream>
#include "Ticker.h"
using namespace std;


class PairPriceStd
{
private:
	int iPairID;
	char sTicker1[TICKER_LEN];
	char sTicker2[TICKER_LEN];
	double fPriceRatioStd;
	int iThreshold;//open position
	double kcloseupper;
	double kcloselower;
	double kdiverge;
	double fMeanRatio;
public:
	char * sGetTicker1(void);
	char * sGetTicker2(void);
	double fGetPriceRatioStd(void);
	int iGetThreshold(void);
	double Getkcloselower(void);
	double Getkcloseupper(void);
	double Getkdiverge(void);
	double fGetMeanRatio(void);
	bool operator==(const PairPriceStd &aPair);
	PairPriceStd(void);
	~PairPriceStd(void);
	PairPriceStd::PairPriceStd(int id, char * pTicker1, char * pTicker2, double ratio, int k, double kcloseupper_,
		double kcloselower_, double kdiverge_, double Mean_);


	friend ostream & operator<<(ostream &out, const PairPriceStd & aPairPriceStd);
};

inline char *PairPriceStd::sGetTicker1(void)
{
	return sTicker1;
}
inline char *PairPriceStd::sGetTicker2(void)
{
	return sTicker2;
}
inline double PairPriceStd::fGetPriceRatioStd(void)
{
	return fPriceRatioStd;
}

inline int PairPriceStd::iGetThreshold(void)
{
	return iThreshold;
}

inline double PairPriceStd::Getkcloseupper(void)
{
	return kcloseupper;
}

inline double PairPriceStd::Getkcloselower(void)
{
	return kcloselower;
}

inline double PairPriceStd::Getkdiverge(void)
{
	return kdiverge;
}

inline double PairPriceStd::fGetMeanRatio(void)
{
	return fMeanRatio;
}