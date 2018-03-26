#ifndef HistoryBloomberg_h
#define HistoryBloomberg_h
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "Ticker.h"
#include <algorithm>  //std::sort

class HistoryBloomberg
{
private:
	std::string         d_host;
	int                 d_port;
	std::string start_day, end_day;
	std::vector <std::string> tickerlist;  //no duplicates of 30 pairs
	vector <Ticker> VectorTicker;

	void printUsage()
	{
		std::cout << "Usage:" << std::endl
			<< "    Retrieve reference data " << std::endl
			<< "        [-ip        <ipAddress  = localhost>" << std::endl
			<< "        [-p         <tcpPort    = 8194>" << std::endl;
	}

	bool parseCommandLine(int argc, char **argv)
	{
		for (int i = 1; i < argc; ++i) {
			if (!std::strcmp(argv[i], "-ip") && i + 1 < argc) {
				d_host = argv[++i];
				continue;
			}
			if (!std::strcmp(argv[i], "-p") && i + 1 < argc) {
				d_port = std::atoi(argv[++i]);
				continue;
			}
			printUsage();
			return false;
		}
		return true;
	}

public:
	HistoryBloomberg() {}//////////////////////& copy constructor
	HistoryBloomberg(std::vector<std::string> tickerlist_, std::string start, std::string end)
		:tickerlist(tickerlist_), start_day(start), end_day(end)
	{
	}
	void SetTickerlist(vector<string> tickerlist_)
	{
		tickerlist = tickerlist_;
	}
	void run(int argc, char **argv);
	vector <Ticker> GetVectorTicker() { return VectorTicker; }
};



#endif // !History_Bloomberg_h
#pragma once
