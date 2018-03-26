#include "StdAfx.h"
#include "PairPriceStd.h"


PairPriceStd::PairPriceStd(void)
{
	iPairID = -1;
	memset(sTicker1, '\0', TICKER_LEN);
	memset(sTicker2, '\0', TICKER_LEN);
	fPriceRatioStd = 0.0;
	iThreshold = 0;
	kcloseupper = 0;
	kcloselower = 0;
	kdiverge = 0;
	fMeanRatio = 0.0;
}

PairPriceStd::~PairPriceStd(void)
{
}

bool PairPriceStd::operator==(const PairPriceStd &aPair)/////////////
{
	return iPairID == aPair.iPairID;
}

PairPriceStd::PairPriceStd(int id, char * pTicker1, char * pTicker2, double ratio, int k, double kcloseupper_,
	double kcloselower_, double kdiverge_, double Mean_)
{
	iPairID = id;
	strncpy_s(sTicker1, pTicker1, TICKER_LEN);
	strncpy_s(sTicker2, pTicker2, TICKER_LEN);
	fPriceRatioStd = ratio;
	iThreshold = k;
	kcloseupper = kcloseupper_;
	kcloselower = kcloselower_;
	kdiverge = kdiverge_;
	fMeanRatio = Mean_;
}

ostream & operator<<(ostream &out, const PairPriceStd & aPairPriceStd)
{
	out << aPairPriceStd.iPairID << endl
		<< aPairPriceStd.sTicker1 << endl
		<< aPairPriceStd.sTicker2 << endl
		<< aPairPriceStd.fMeanRatio << endl
		<< aPairPriceStd.fPriceRatioStd << endl
		<< aPairPriceStd.iThreshold << endl
		<< aPairPriceStd.kcloseupper << endl
		<< aPairPriceStd.kcloselower << endl
		<< aPairPriceStd.kdiverge << endl;
	return out;
}