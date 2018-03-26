#include "StdAfx.h"
#include "Ticker.h"

Ticker::Ticker(void)
{
	sTicker = "";
}

Ticker::~Ticker(void)
{
}

Ticker::Ticker(const Ticker &aTicker)
{
	sTicker = aTicker.sTicker;
	fTickerOpen = aTicker.fTickerOpen;
	fTickerClose = aTicker.fTickerClose;
	Dates = aTicker.Dates;
}

Ticker::Ticker(string pTicker)
{
	sTicker = pTicker;
}

bool Ticker::operator==(const Ticker &aTicker)
{
	return sTicker==aTicker.sTicker;/////////////////////////
}

