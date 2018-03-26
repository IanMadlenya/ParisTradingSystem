// PairTrading.cpp  
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "TablesAndComputation.h"
using namespace std;

int main(int argc, char **argv)
//int _tmain(int argc, _TCHAR* argv[])
{	
	int choice = -1;
	int k = 1;
	double kcloseup = 0.3;
	double kcloselow = - 0.3;
	double kdiv = 4;
	ofstream fout;
	fout.open ("outputs.txt");
  
	map<pair<string, string>, PairPriceStd> PairPriceStdTable;
	map<pair<string, string>, PairPriceStd>::iterator pairIt;

	map<pair<string, string>, vector<Trade>> TradeTable;//for backtesting 
	map<pair<string, string>, vector<Trade>>::iterator tradeIt;
	vector<Trade>::iterator vIt;

	std::map< pair<std::string, std::string>, std::vector<Trade> > TradePair;//for real time record

	string startdate, enddate;
	startdate = "20070501";
	enddate = "20171031";
	//backtest:05/2017-11/2017, priceratio:11/2016 -04/2017
	string StartDa = "11/01/2016";
	string Middate = "05/01/2017";
	//priceratiotable: 10/2017-11/2017
	string StartDateReal = "9/01/2017";
	string EndDateReal = "10/20/2017";
	vector<string> FullTickers;
	vector<pair<string, string>> Pairs;
	RetrievePairTable(FullTickers, Pairs);/////////////
	HistoryBloomberg example(FullTickers, startdate, enddate);
	//HistoryBloomberg example2(FullTickers, StartDateReal,EndDateReal);
	do
	{
		cout << endl
		<< "0 - Update Pairs Info.\n"
		<< "1 - Select k (default k = 1).\n"
		<< "2 - Run Back Test.\n"
		<< "3 - Update Historical Data.\n"
		<< "4 - Real Time Trading.\n"
		<< "5 - Enter a Pair Trade.\n"
		<< "6 - Exit.\n"
		<< "Enter your choice and press return: ";
		cin >> choice;
		time_t timeIntervalUser;
		switch (choice)
		{
			case 0:
				UpdatePairTable(FullTickers);
				example.SetTickerlist(FullTickers);
				break;

			case 1:
				cout << "Please enter an integer value for threshold of open position: ";
				cin >> k;
				getchar();
				cout << "Please enter a value for the upper bound of close position: ";
				cin >> kcloseup;
				cout << "Please enter a negative value for the lower bound of close position: ";
				cin >> kcloselow;
				cout << "Please enter a value for the divergence tolerance: ";
				cin >> kdiv;
				RetrievePairPriceRatioTable(PairPriceStdTable, k, kcloseup, kcloselow, kdiv);
				cout << "The PairPriceStdTable contains:\n";
				fout << "The PairPriceStdTable contains:\n";
				for ( pairIt=PairPriceStdTable.begin(); pairIt != PairPriceStdTable.end(); pairIt++ )
				{
					cout //<< pairIt->first.first << " " << pairIt->first.second << endl
						 << (*pairIt).second << endl;
					fout //<< pairIt->first.first << " " << pairIt->first.second << endl
						 << (*pairIt).second << endl;
				}
				break;
				
			case 2:
				UpdatePairPriceTable(Pairs, Middate, StartDa);//update tables: BackTestPrice and price table for every pair
				UpdatePairPriceRatioTable(Pairs);

				RetrievePairPriceRatioTable(PairPriceStdTable, k, kcloseup, kcloselow, kdiv);
				RetrieveBackTestPriceTable(TradeTable);
				
				UpdateProfit(TradeTable, PairPriceStdTable);
				UpdateBackTestTradeTable(TradeTable);
				cout << "The TradeTable contains:\n";
				fout << "The TradeTable contains:\n";
				for ( tradeIt=TradeTable.begin(); tradeIt != TradeTable.end(); tradeIt++ )
				{	
					//cout << tradeIt->first.first << " " << tradeIt->first.second << endl;
					//fout << tradeIt->first.first << " " << tradeIt->first.second << endl;
					for ( vIt = tradeIt->second.begin(); vIt != tradeIt->second.end(); vIt++ )
					{
						cout << (*vIt) << endl; //call friend ostream & operator<<(ostream &out, const Trade & aTrade)
						fout << (*vIt) << endl;
					}
				}
				break;

			case 3:
				example.run(argc, argv);
				UpdatePriceTable(example.GetVectorTicker());
				break;

			case 4:
				UpdatePairPriceTable(Pairs, EndDateReal, StartDateReal);
				UpdatePairPriceRatioTable(Pairs);
				RetrievePairPriceRatioTable(PairPriceStdTable, k, kcloseup, kcloselow, kdiv);

				cout << "Please input the trading time interval you prefer: ";
				cin >> timeIntervalUser;
				run(argc, argv, timeIntervalUser, FullTickers, Pairs, TradePair);
				UpdateProfit(TradePair, PairPriceStdTable);
				UpdateRealTimeTradeTable(TradePair);
				break;

			case 5:	
				UpdatePairPriceTable(Pairs, EndDateReal, StartDateReal);
				UpdatePairPriceRatioTable(Pairs);

				RetrievePairPriceRatioTable(PairPriceStdTable, k, kcloseup, kcloselow, kdiv);
				EnterPairTrade(PairPriceStdTable);
				break;

			case 6:
				cout << "End of Program.\n";
				break;

			default:
				cout << "Not a Valid Choice. \n" << "Choose again.\n";
				break;

		}
	} while (choice!=6);
	
	fout.close();
	getchar();	
    return 0;
}
