#include "StdAfx.h"
#include "Trade.h"

Trade::Trade(void)
{
	iPairID = -1;
	status = 0;
	fProfit = 0.0;
}

Trade::~Trade(void)
{
}

Trade::Trade(int id, string pTicker1, string pTicker2, double p1, double p2, int s1, int s2, string pDate1, string pDate2)
{
	iPairID = id;
	sTicker1 = pTicker1;
	sTicker2 = pTicker2;
	price1 = p1;
	price2 = p2;
	size1 = s1;
	size2 = s2;
	sDate1 = pDate1;
	sDate2 = pDate2;
	status = 0;
	fProfit = 0.0;
}
Trade::Trade(const Trade &tr)
{
	iPairID = tr.iPairID;
	sTicker1 = tr.sTicker1;
	sTicker2 = tr.sTicker2;
	price1 = tr.price1;
	price2 = tr.price2;
	size1 = tr.size1;
	size2 = tr.size2;
	sDate1 = tr.sDate1;
	sDate2 = tr.sDate2;
	status = tr.status;
	fProfit = tr.fProfit;

}
ostream & operator<<(ostream &out, const Trade & aTrade)
{
	out << aTrade.iPairID << " " << " " << aTrade.sTicker1 << aTrade.sTicker2 <<
		aTrade.price1 << " "  << aTrade.price2 //<< " " << " " << aTrade.size1 << " " << aTrade.size2 
		<< " " << " "  << aTrade.sDate1 << " " << " " << aTrade.fProfit << endl;
	return out;
}