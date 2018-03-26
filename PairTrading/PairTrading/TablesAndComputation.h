#ifndef TablesAndComputation_h
#define TablesAndComputation_h
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
#include "PairPriceStd.h"
#include "HistoryBloomberg.h"
#include "RealTimeTrading.h"

#import <C:\\Program Files\\Common Files\\System\\ado\\msado15.dll>  \
    rename( "EOF", "AdoNSEOF" )

using namespace std;

// Connection string for ADO
// Verify the location of database, make sure the path is correct and accessible
//_bstr_t bstrConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Users\\Song\\Documents\\PairTrading.accdb;";
//_bstr_t bstrConnect = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=C:\\Users\\Bloomberg Lab User\\Downloads\\Ping\\PairTrading\\PairTrading\\PairTrading.mdb;";
//_bstr_t bstrConnect = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=C:\\Users\\pingf\\OneDrive\\PairTrading\\PairTrading\\PairTrading.mdb;";
_bstr_t bstrConnect = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=C:\\Users\\FREUser\\Downloads\\Ping\\PairTrading1\\PairTrading\\PairTrading\\PairTrading.mdb;";

//Update Pairs table
int UpdatePairTable(vector<string> &fullticker);

//Retrieve all pairs
int RetrievePairTable(vector<string> &tickers, vector<pair<string, string>> &StockPair);

//Update historical price table for every ticker
int UpdatePriceTable(vector<Ticker> VectorTicker);

//Update historical price table for every pair, including BackTestPrice table
int UpdatePairPriceTable(vector<pair<string, string>> StockPair);

//Update Pair Price Ratio for PairPriceRatio table
int UpdatePairPriceRatioTable(vector<pair<string, string>> StockPair);

//Retrieve Pair Price Ratio from PairPriceRatio table
int RetrievePairPriceRatioTable(map<pair<string, string>, PairPriceStd> & PairPriceStdTable, int k, double kclose1, double kclose2, double kdiverge_);

//Retrieve data from BackTestPrice table
int RetrieveBackTestPriceTable(map<pair<string, string>, vector<Trade>> &TradeTable);

//Enter price info for a pair and execute trading strategy
int EnterPairTrade(map<pair<string, string>, PairPriceStd> & PairPriceStdTable);

//Calculate profits for backtesting and real time trading
int UpdateProfit(std::map< pair<std::string, std::string>, std::vector<Trade> > &TradePair, map<pair<string, string>, PairPriceStd> & PairPriceStdTable);

//Update BackTestTrades table
int UpdateBackTestTradeTable(map<pair<string, string>, vector<Trade>> TradeTable);

//Update RealTimeTrades table
int UpdateRealTimeTradeTable(map<pair<string, string>, vector<Trade>> TradePair);

//Error handle
void PrintComError(_com_error &e);

int UpdateBackTestTradeTable(map<pair<string, string>, vector<Trade>> TradeTable)
{
	map<pair<string, string>, vector<Trade>>::iterator mIt;
	vector<Trade>::iterator vIt;
	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			int iPairID = 0;
			char ticker1[TICKER_LEN];
			char ticker2[TICKER_LEN];
			int status = 0;
			double fProfit = 0;
			char sDate[DATE_LEN];
			char sQuery[256];
			int CountCloseStatus = 0;
			sprintf_s(sQuery, "DELETE BackTestTrades.* FROM BackTestTrades;");
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
			ADODB::adLockUnspecified, ADODB::adCmdText);
			
			for (mIt = TradeTable.begin(); mIt != TradeTable.end(); mIt++)
			{
				//cout << mIt->first.first << " " << mIt->first.second << endl;
				for (vIt = mIt->second.begin(); vIt != mIt->second.end(); vIt++)
				{
					status = vIt->fGetStatus();
					memset((void*)ticker1, '\0', DATE_LEN);
					memset((void*)ticker2, '\0', DATE_LEN);
					memset((void*)sDate, '\0', DATE_LEN);
					memset((void*)sQuery, '\0', 256);
					strncpy_s(ticker1, vIt->sGetTicker1().c_str(), TICKER_LEN);
					strncpy_s(ticker2, vIt->sGetTicker2().c_str(), TICKER_LEN);

					iPairID = vIt->iGetPairID();
					fProfit = vIt->fGetProfit();
					strncpy_s(sDate, vIt->sGetDate1().c_str(), DATE_LEN);
					//cout  << " " << iPairID << " " << sDate << " " << status << " " << fProfit << endl;
					status = vIt->fGetStatus();

					sprintf_s(sQuery, "INSERT INTO BackTestTrades VALUES(%d,'%s', '%s', '%s', %d, %.2f);", iPairID, ticker1, ticker2, sDate, status, fProfit);
					//'%s'--In SQL: Insert into trades values(3,5,'dd',2.2);
					//we do not have cstring in C++, have cstring in C--printf(%s %f), s- cstring, so here we use char[] not string
					ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
					hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
						ADODB::adLockUnspecified, ADODB::adCmdText);
					/*
					//only show the position when to take and when to close
					if (status == 1 || status == -1)
					{
						if (CountCloseStatus == 0)
						{
							memset((void*)ticker1, '\0', DATE_LEN);
							memset((void*)ticker2, '\0', DATE_LEN);
							memset((void*)sDate, '\0', DATE_LEN);
							memset((void*)sQuery, '\0', 256);
							strncpy_s(ticker1, vIt->sGetTicker1().c_str(), TICKER_LEN);
							strncpy_s(ticker2, vIt->sGetTicker2().c_str(), TICKER_LEN);

							iPairID = vIt->iGetPairID();
							fProfit = vIt->fGetProfit();
							strncpy_s(sDate, vIt->sGetDate1().c_str(), DATE_LEN);
							//cout  << " " << iPairID << " " << sDate << " " << status << " " << fProfit << endl;
							status = vIt->fGetStatus();
							
							sprintf_s(sQuery, "INSERT INTO BackTestTrades VALUES(%d,'%s', '%s', '%s', %d, %.2f);", iPairID, ticker1, ticker2, sDate, status, fProfit);
							//'%s'--In SQL: Insert into trades values(3,5,'dd',2.2);
							//we do not have cstring in C++, have cstring in C--printf(%s %f), s- cstring, so here we use char[] not string
							ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
							hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
								ADODB::adLockUnspecified, ADODB::adCmdText);
						}
						CountCloseStatus++;
					}
					else if (status == 2 || status == -2)
					{ 
						memset((void*)ticker1, '\0', DATE_LEN);
						memset((void*)ticker2, '\0', DATE_LEN);
						memset((void*)sDate, '\0', DATE_LEN);
						memset((void*)sQuery, '\0', 256);
						strncpy_s(ticker1, vIt->sGetTicker1().c_str(), TICKER_LEN);
						strncpy_s(ticker2, vIt->sGetTicker2().c_str(), TICKER_LEN);

						iPairID = vIt->iGetPairID();
						fProfit = vIt->fGetProfit();
						strncpy_s(sDate, vIt->sGetDate1().c_str(), DATE_LEN);
						//cout  << " " << iPairID << " " << sDate << " " << status << " " << fProfit << endl;
						status = vIt->fGetStatus();
						
						sprintf_s(sQuery, "INSERT INTO BackTestTrades VALUES(%d,'%s', '%s', '%s', %d, %.2f);", iPairID, ticker1, ticker2, sDate, status, fProfit);
						//'%s'--In SQL: Insert into trades values(3,5,'dd',2.2);
						//we do not have cstring in C++, have cstring in C--printf(%s %f), s- cstring, so here we use char[] not string
						ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
						hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
							ADODB::adLockUnspecified, ADODB::adCmdText);
						CountCloseStatus = 0;
					}
					*/
				}
			}
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		PrintComError(e);
	}
	CoUninitialize();
	return 0;
}


int UpdateRealTimeTradeTable(map<pair<string, string>, vector<Trade>> TradePair)
{
	map<pair<string, string>, vector<Trade>>::iterator mIt;
	vector<Trade>::iterator vIt;
	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			int iPairID = 0;
			double p1, p2;
			double fProfit = 0.0;
			int status = 0;
			char sDate1[DATE_LEN];
			char sDate2[DATE_LEN];
			char sQuery[256];
			//int size = 0;
			char ticker1[TICKER_LEN];
			char ticker2[TICKER_LEN];
			/*
			sprintf_s(sQuery, "CREATE TABLE RealTimeTrades (PairID INT, Ticker1 VARCHAR(255), Ticker2 VARCHAR(255), Price1 DOUBLE, Price2 DOUBLE, Time1 DATE, Time2 DATE, Position INT, Profit DOUBLE);");
			*/
			sprintf_s(sQuery, "DELETE RealTimeTrades.* FROM RealTimeTrades;");
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			
			for (mIt = TradePair.begin(); mIt != TradePair.end(); mIt++)
			{
				//cout << mIt->first.first << " " << mIt->first.second << endl;
				for (vIt = mIt->second.begin(); vIt != mIt->second.end(); vIt++)
				{
					memset((void*)ticker1, '\0', DATE_LEN);
					memset((void*)ticker2, '\0', DATE_LEN);
					memset((void*)sDate1, '\0', DATE_LEN);
					memset((void*)sDate2, '\0', DATE_LEN);
					memset((void*)sQuery, '\0', 256);

					iPairID = vIt->iGetPairID();
					p1 = vIt->fGetPrice1();
					p2 = vIt->fGetPrice2();
					fProfit = vIt->fGetProfit();
					status = vIt->fGetStatus();
					strncpy_s(ticker1, vIt->sGetTicker1().c_str(), TICKER_LEN);
					strncpy_s(ticker2, vIt->sGetTicker2().c_str(), TICKER_LEN);
					strncpy_s(sDate1, vIt->sGetDate1().c_str(), DATE_LEN);
					strncpy_s(sDate2, vIt->sGetDate2().c_str(), DATE_LEN);

					//cout << iPairID << " " << p1 << " " << p2 << " " << sDate1 << " " << sDate2 << " " << status << " " << fProfit << endl;
					sprintf_s(sQuery, "INSERT INTO RealTimeTrades VALUES (%d,'%s','%s', %.2f, %.2f,'%s','%s', %d, %.2f);", iPairID, ticker1, ticker2, p1, p2, sDate1, sDate2, status, fProfit);
					hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
						ADODB::adLockUnspecified, ADODB::adCmdText);
				}
			}
			
			memset((void*)sQuery, '\0', 256);
			sprintf_s(sQuery, "DROP TABLE RealtimeTotalProfit;");
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			//cout << "drop ok" << endl;
			
			memset((void*)sQuery, '\0', 256);
			sprintf_s(sQuery, "SELECT SUM(RealTimeTrades.Profit) As TotalProfit INTO RealtimeTotalProfit From RealTimeTrades");
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			//cout << "sum ok" << endl;
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();
	return 0;
}

int UpdateProfit(std::map< pair<std::string, std::string>, std::vector<Trade> > &TradePair,
	map<pair<string, string>, PairPriceStd> & PairPriceStdTable)
{
	double stock1price1 = 0;
	double stock2price1 = 0;
	double stock1price2 = 0;
	double stock2price2 = 0;
	double sd = 0;
	double fProfit = 0;
	int kopen = 0;
	double kcloseupper = 0;
	double kcloselower = 0;
	double kdiverge = 0.0;
	double MeanRatio = 0.0;
	int vol1 = 0;
	int vol2 = 0;
	int status = 0;
	int counttrade = 0;
	bool hold= false;

	map<pair<string, string>, vector<Trade>>::iterator mIt;
	vector<Trade>::iterator vIt;
	for (mIt = TradePair.begin(); mIt != TradePair.end(); mIt++)
	{
		sd = PairPriceStdTable[mIt->first].fGetPriceRatioStd(); 
		kopen = PairPriceStdTable[mIt->first].iGetThreshold();
		sd = sd / sqrt(28);
		MeanRatio = PairPriceStdTable[mIt->first].fGetMeanRatio();
		kcloseupper = PairPriceStdTable[mIt->first].Getkcloseupper();
		kcloselower = PairPriceStdTable[mIt->first].Getkcloselower();
		kdiverge = PairPriceStdTable[mIt->first].Getkdiverge();

		//cout << mIt->first.first << " " << mIt->first.second << " " << sd << " " << kopen << " " << kcloseupper << " " << kcloselower << " " << kdiverge << " ";
		vIt = mIt->second.begin();
		cout << mIt->first.first << " " << mIt->first.second << endl;
		while (true)
		{
			if (vIt == mIt->second.end())
				break;
			stock1price1 = vIt->fGetPrice1();
			stock2price1 = vIt->fGetPrice2();

			//trading volume
			vol1 = 10000;
			vol2 = static_cast<int>(vol1*(stock1price1 / stock2price1));
			
			//open position
			//if (stock1price1 / stock2price1 > sd * kopen + MeanRatio)
			if (log(stock1price1 / stock2price1) > sd * kopen + MeanRatio)
			{
				status = -1; 
				hold = true;
				counttrade++;	
			}
			//else if (stock1price1 / stock2price1 < - sd * kopen + MeanRatio)
			else if (log(stock1price1 / stock2price1) < -sd * kopen + MeanRatio)
			{
				status = 1;
				hold = true;
				counttrade++;
			}
			vIt->SetStatus(status);
			vIt++;
			//cout <<"outside while: "<< stock1price1 << " " <<  stock1price2 << " " << counttrade << endl;
			while (hold)
			{
				if (vIt == mIt->second.end()) 
				{
					status = 0;
					//cout<<"---set 0---------------------"<< stock1price1 << " " << stock1price2 << " " << counttrade << endl;
					for (int i = 0; i < counttrade; i++)
					{
						//cout << "---i: " << i << endl;
						(--vIt)->SetStatus(status);
					}
					counttrade = 0;
					hold = false;
					fProfit = 0;
					vIt++;
					break;
				}	
				stock1price2 = vIt->fGetPrice1();
				stock2price2 = vIt->fGetPrice2();

				//close position
				//if ((stock2price1 / stock2price1 < sd * kcloseupper + MeanRatio &&  stock2price1 / stock2price1 > sd * kcloselower + MeanRatio)
					//|| counttrade == 3 || stock1price1 / stock2price1 > sd * kdiverge + MeanRatio)
				if ((log(stock2price1 / stock2price1) < sd * kcloseupper + MeanRatio &&  log(stock2price1 / stock2price1) > sd * kcloselower + MeanRatio)
					|| counttrade == 3 || log(stock1price1 / stock2price1) > sd * kdiverge + MeanRatio)
				{
					if (status == 1)
						status = 2;
					else if (status == -1)
						status = -2;
					
					hold = false;
					counttrade = 0;
				}
				//cout << "inside while -- "<<stock2price1 << " " <<stock2price2 <<" "<< counttrade << endl;
				if (hold)
					counttrade++;
				vIt->SetStatus(status);
				if (status == 2)
				{
					fProfit = vol1*(stock1price2 - stock1price1) - vol2*(stock2price2 - stock2price1);
					status = 0;
				}	
				else if (status == -2)
				{
					fProfit = -vol1*(stock1price2 - stock1price1) + vol2*(stock2price2 - stock2price1);
					status = 0;
				}	
				//cout << vol1 << " " << vol2 << " " << fProfit << endl;
				vIt->SetProfit(fProfit);
				fProfit = 0;
				vIt++;
			}			
		}
	}
	return 0;
}


int UpdatePairTable(vector<string> &fullticker)
{
	//retrieve pairs info from csv. file
	vector<string> id, ticker1, ticker2;
	size_t index;
	ifstream file("./Pairs.csv");//*relative path
	fullticker.clear();
	while (file.good())
	{
		std::string line;
		getline(file, line);  //skip the fisrt row in csv file
		while (getline(file, line))
		{
			id.push_back(line.substr(0, line.find(',')));
			index = line.find(',');
			ticker1.push_back(line.substr(index + 1, line.find(',', index + 1) - index - 1));
			index = line.find(',', index + 1);
			ticker2.push_back(line.substr(index + 1, line.find(',', index + 1) - index - 1));
			fullticker.push_back(ticker1.back());
			fullticker.push_back(ticker2.back());
		}
	}
	std::vector<std::string>::iterator it;
	std::sort(fullticker.begin(), fullticker.end());
	fullticker.erase(std::unique(fullticker.begin(), fullticker.end()), fullticker.end());

	//connect to SQL
	HRESULT hResult = CoInitialize(0);//Initializes the COM library on the current thread and identifies the concurrency model as single-thread apartment (STA).
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			char sQuery[756];
			char query[756];
			char cID[756];
			char cticker1[756];
			char cticker2[756];
			string SQLDrop = "DELETE * FROM Pairs;";
			strcpy_s(sQuery, SQLDrop.c_str());
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			/*
			string SQL = "CREATE TABLE Pairs (ID VARCHAR(255), Ticker1 VARCHAR(255), Ticker2 VARCHAR(255), PRIMARY KEY(ID));";
			*/
			for (size_t i = 0; i < id.size(); i++)
			{
				if (SUCCEEDED(hResult))
				{
					memset((void*)query, '\0', 256);
					memset((void*)sQuery, '\0', 256);
					memset((void*)cID, '\0', 256);
					memset((void*)cticker1, '\0', 256);
					memset((void*)cticker2, '\0', 256);
					strcpy_s(cID, id[i].c_str()); 
					strcpy_s(cticker1, ticker1[i].c_str());
					strcpy_s(cticker2, ticker2[i].c_str());
					//string SQLInput = "INSERT INTO Pairs VALUES('%s','%s','%s');";
					//strcpy_s(query, SQLInput.c_str());
					sprintf_s(sQuery, "INSERT INTO Pairs VALUES('%s','%s','%s');", cID, cticker1, cticker2);

					hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
						ADODB::adLockUnspecified, ADODB::adCmdText);
				}
			}
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();
	return 0;
}


int UpdatePriceTable(vector<Ticker> VectorTicker)
{
	//In the preceding code fragment, success or failure is returned to the HRESULT variable,
	HRESULT hResult = CoInitialize(0);//Initializes the COM library on the current thread and identifies the concurrency model as single-thread apartment (STA).
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			char sQuery[756];
			char query[756];
			string ticker;
			string SQLDrop, SQLCreate;

			for (size_t i = 0; i < VectorTicker.size(); i++)
			{
				memset((void*)sQuery, '\0', 256);
				
				ticker = VectorTicker[i].GetTicker();
				SQLDrop = "DELETE * FROM " + ticker + ";";
				strcpy_s(sQuery, SQLDrop.c_str());
				hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
					ADODB::adLockUnspecified, ADODB::adCmdText);
				//If the table has primary key, cannot drop it first
				/*
				SQLCreate = "CREATE TABLE " + ticker +
					" (DateOfTrade DATE, OpenPrice DOUBLE, ClosePrice DOUBLE);";
				*/
				vector <double> Open = VectorTicker[i].GetOpen();
				vector <double> Close = VectorTicker[i].GetClose();
				vector <char_array> Date = VectorTicker[i].GetDates();

				if (SUCCEEDED(hResult))
				{
					for (size_t k = 0; k < Open.size(); k++)
					{
						memset((void*)query, '\0', 256);
						memset((void*)sQuery, '\0', 256);
						
						string SQLInput = "INSERT INTO " + ticker + " VALUES('%s',%.2f,%.2f);";
						strcpy_s(query, SQLInput.c_str());
						//sprintf_s(sQuery, query, Date[k], Close[k], Open[k]); wrong
						sprintf_s(sQuery, query, Date[k].DateChar, Open[k], Close[k]); /////////////////////////////
						hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
							ADODB::adLockUnspecified, ADODB::adCmdText);
						
					}
				}
			}
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		PrintComError(e);
	}
	CoUninitialize();
	return 0;
}

int RetrievePairTable(vector<string> &tickers, vector<pair<string, string>> &StockPair)
{
	vector<string> v1, v2;
	int id = -1;
	char ticker1[TICKER_LEN];
	char ticker2[TICKER_LEN];
	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			_bstr_t query = "SELECT * FROM Pairs;";
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			hResult = pRecSet->Open(query, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			if (SUCCEEDED(hResult))
			{
				ADODB::Fields* pFields = NULL;
				hResult = pRecSet->get_Fields(&pFields);
				_variant_t aField;
				StockPair.clear(); //empty first
				tickers.clear();
				while (!pRecSet->AdoNSEOF)
				{
					long lIndex = 0;
					memset((void*)ticker1, '\0', TICKER_LEN);
					memset((void*)ticker2, '\0', TICKER_LEN);
					aField = pFields->GetItem(lIndex++)->GetValue();
					id = int(aField);
					strncpy_s(ticker1, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					strncpy_s(ticker2, bstr_t(pFields->GetItem(lIndex)->GetValue()), TICKER_LEN);
					v1.push_back(ticker1);
					//v1.push_back(string(ticker1)); do not need to convert data type as call constructor string(ticker1)
					v2.push_back(ticker2);
					tickers.push_back(ticker1);
					tickers.push_back(ticker2);

					pair<string, string> aTickerPair(ticker1, ticker2);
					StockPair.push_back(aTickerPair);
					pRecSet->MoveNext();
				}
			}
			pRecSet->Close();
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();

	sort(tickers.begin(), tickers.end());
	tickers.erase(unique(tickers.begin(), tickers.end()), tickers.end());
	return 0;
}

int UpdatePairPriceTable(vector<pair<string, string>> StockPair, string MidDate, string StartDa)
{
	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
		char sQuery[756];
		char query[756];
		string ticker1, ticker2;
		string tablename;
		string SQLDrop1, SQLDrop2;
		string SQLInput1, SQLInput2;

		memset((void*)sQuery, '\0', 256);
		SQLDrop2 = "DELETE * FROM BackTestPrice;";
		strcpy_s(sQuery, SQLDrop2.c_str());
		hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
			ADODB::adLockUnspecified, ADODB::adCmdText);
		
		if (SUCCEEDED(hResult))
		{
			for (size_t i = 0; i < StockPair.size(); i++)
			{
				ticker1 = StockPair[i].first;
				ticker2 = StockPair[i].second;
				tablename = ticker1 + ticker2;

				memset((void*)query, '\0', 256);
				memset((void*)sQuery, '\0', 256);
				SQLInput2 = "INSERT INTO BackTestPrice (Ticker1, Ticker2, Dates, Ticker1Open, Ticker1Close, Ticker2Open, Ticker2Close) SELECT '%s', \
						'%s', A.DateOfTrade, A.OpenPrice, A.ClosePrice, B.OpenPrice, B.ClosePrice FROM " + ticker1 + " AS A, "
					+ ticker2 + " AS B WHERE A.DateOfTrade = B.DateOfTrade and A.DateOfTrade >= #" + MidDate + "# ORDER BY A.DateOfTrade;";
				//DateOfTrade >= #05/01/2017#  (#: keep the data formart, then it can be compared correctly)

				strcpy_s(query, SQLInput2.c_str());
				sprintf_s(sQuery, query, ticker1.c_str(), ticker2.c_str());
				hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
					ADODB::adLockUnspecified, ADODB::adCmdText);

				memset((void*)sQuery, '\0', 256);
				SQLDrop1 = "DELETE * FROM " + tablename+";";
				strcpy_s(sQuery, SQLDrop1.c_str());
				hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
				//cout << "delete table " << tablename << endl;
				/*
				string SQLCreate = "CREATE TABLE BackTestPrice (Ticker1 VARCHAR(255), Ticker2 VARCHAR(255), Dates DATE, Ticker1Open DOUBLE, Ticker1Close DOUBLE, Ticker2Open DOUBLE, Ticker2Close DOUBLE);";
				*/
				if (SUCCEEDED(hResult))
				{
					memset((void*)query, '\0', 256);
					memset((void*)sQuery, '\0', 256);
					/*
					SQLInput = "SELECT '%s' AS Ticker1, '%s' AS Ticker2, A.DateOfTrade AS Dates, A.OpenPrice AS Ticker1Open, A.ClosePrice AS\
					Ticker1Close, B.OpenPrice AS Ticker2Open, B.ClosePrice AS Ticker2Close INTO " + tablename + " FROM " + ticker1 + " AS A, " 
						+ ticker2 + " AS B WHERE A.DateOfTrade = B.DateOfTrade ORDER BY A.DateOfTrade;";
					*/
					SQLInput1 = "INSERT INTO " + tablename + "(Ticker1, Ticker2, Dates, Ticker1Open, Ticker1Close, Ticker2Open, Ticker2Close) SELECT '%s', \
						'%s', A.DateOfTrade, A.OpenPrice, A.ClosePrice, B.OpenPrice, B.ClosePrice FROM " + ticker1 + " AS A, "
						+ ticker2 + " AS B WHERE A.DateOfTrade = B.DateOfTrade and A.DateOfTrade < #" + MidDate + "# and A.DateOfTrade > #" + StartDa + "# ORDER BY A.DateOfTrade; ";
					strcpy_s(query, SQLInput1.c_str());
					sprintf_s(sQuery, query, ticker1.c_str(), ticker2.c_str());
					hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
						ADODB::adLockUnspecified, ADODB::adCmdText);					
				}
			}
			/*
			memset((void*)query, '\0', 256);
			memset((void*)sQuery, '\0', 256);
			SQLInput2 = "SELECT * FROM BackTestPrice ORDER BY Ticker1, Ticker2;";																				  //DateOfTrade >= #05/01/2017#  (#: keep the data formart, then it can be compared correctly)
			strcpy_s(query, SQLInput2.c_str());
			sprintf_s(sQuery, query, ticker1.c_str(), ticker2.c_str());
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			*/
			pConnect->Close();
		}		
	}		
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();
	return 0;
}

int UpdatePairPriceRatioTable(vector<pair<string, string>> StockPair)
{
	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			string ticker1, ticker2;
			string tablename;
			string SQLInput;
			char sQuery[756];
			char query[756];
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");

			/*CREATE TABLE PairPriceRatio(Id int, Ticker1 VARCHAR(255), Ticker2 VARCHAR(255), MeanPriceRatio double, StdPriceRatio double);*/

			string SQLDelete = "DELETE * FROM PairPriceRatio;";
			strcpy_s(sQuery, SQLDelete.c_str());
			hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			if (SUCCEEDED(hResult))
			{
				for (size_t i = 0; i < StockPair.size(); i++)
				{
					ticker1 = StockPair[i].first;
					ticker2 = StockPair[i].second;
					tablename = ticker1 + ticker2;

					memset((void*)query, '\0', 256);
					memset((void*)sQuery, '\0', 256);
					//SQLInput = "INSERT INTO PairPriceRatio(Id, Ticker1, Ticker2, MeanPriceRatio, StdPriceRatio) SELECT %d, '%s', '%s',\
					//	Format(AVG(Ticker1Close / Ticker2Close), 'Fixed'), Format(StDev(Ticker1Close / Ticker2Close), 'Fixed') FROM " + tablename + ";";
					
				//	for log strategy
					SQLInput = "INSERT INTO PairPriceRatio(Id, Ticker1, Ticker2, MeanPriceRatio, StdPriceRatio) SELECT %d, '%s', '%s',\
						Format(AVG(LOG(Ticker1Close / Ticker2Close)), 'Fixed'), Format(StDev(LOG(Ticker1Close / Ticker2Close)), 'Fixed') FROM " + tablename + ";";
					strcpy_s(query, SQLInput.c_str());
					sprintf_s(sQuery, query, i + 1, ticker1.c_str(), ticker2.c_str());
					hResult = pRecSet->Open(sQuery, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
						ADODB::adLockUnspecified, ADODB::adCmdText);
				}
			}
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();
	return 0;
}

int RetrievePairPriceRatioTable(map<pair<string, string>, PairPriceStd> & PairPriceStdTable, int k, double kclose1, double kclose2, double kdiverge_)
{
	int id = -1;
	char ticker1[TICKER_LEN];
	char ticker2[TICKER_LEN];
	double ratio = 0.0;
	double meanratio = 0.0;
	HRESULT hResult = CoInitialize(0);//Initializes the COM library on the current thread and identifies the concurrency model as single-thread apartment (STA).
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			_bstr_t query = "SELECT * FROM PairPriceRatio;";
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");

			hResult = pRecSet->Open(query, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			if (SUCCEEDED(hResult))
			{
				ADODB::Fields* pFields = NULL;
				hResult = pRecSet->get_Fields(&pFields);
				_variant_t aField;
			
				PairPriceStdTable.clear();//Removes all elements from the map container (which are destroyed), leaving the container with a size of 0.
				while (!pRecSet->AdoNSEOF)
				{
					long lIndex = 0;
					memset((void*)ticker1, '\0', TICKER_LEN);//in case that strncpy_s makes new ticker and old ticker mixed
					memset((void*)ticker2, '\0', TICKER_LEN);
					aField = pFields->GetItem(lIndex++)->GetValue();
					id = int(aField);
					strncpy_s(ticker1, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					strncpy_s(ticker2, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					aField = pFields->GetItem(lIndex++)->GetValue();
					meanratio = double(aField);
					aField = pFields->GetItem(lIndex)->GetValue();
					ratio = double(aField);
					
					pair<string, string> aTickerPair(ticker1, ticker2);
					PairPriceStd aPairPriceStd(id, ticker1, ticker2, ratio, k, kclose1,
						kclose2, kdiverge_, meanratio);
					PairPriceStdTable.insert(pair<pair<string, string>, PairPriceStd>(aTickerPair, aPairPriceStd));
					pRecSet->MoveNext();
				}
			}
			pRecSet->Close();
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}

	// Release COM
	CoUninitialize();
	return 0;
}

int RetrieveBackTestPriceTable(map<pair<string, string>, vector<Trade>> &TradeTable)
{
	int id = 0;
	char ticker1[TICKER_LEN];
	char ticker2[TICKER_LEN];
	double open1 = 0.0;
	double close1 = 0.0;
	double open2 = 0.0;
	double close2 = 0.0;
	int size1 = 0;
	int size2 = 0;
	char date1[DATE_LEN];
	char date2[DATE_LEN];
	double profit = 0.0;

	vector<Trade> PairTrades;
	PairTrades.clear();

	memset((void*)ticker1, '\0', TICKER_LEN);
	memset((void*)ticker2, '\0', TICKER_LEN);
	memset((void*)date1, '\0', DATE_LEN);
	memset((void*)date2, '\0', DATE_LEN);
	pair<string, string> TickerPair(ticker1, ticker2);

	HRESULT hResult = CoInitialize(0);
	if (FAILED(hResult))
	{
		return -1;
	}
	try
	{
		ADODB::_ConnectionPtr pConnect("ADODB.Connection");
		hResult = pConnect->Open(bstrConnect, "admin", "", ADODB::adConnectUnspecified);
		if (SUCCEEDED(hResult))
		{
			_bstr_t query = "SELECT * FROM BackTestPrice;";
			ADODB::_RecordsetPtr pRecSet("ADODB.Recordset");
			hResult = pRecSet->Open(query, _variant_t((IDispatch *)pConnect, true), ADODB::adOpenUnspecified,
				ADODB::adLockUnspecified, ADODB::adCmdText);
			if (SUCCEEDED(hResult))
			{
				ADODB::Fields* pFields = NULL;
				hResult = pRecSet->get_Fields(&pFields);
				_variant_t aField;

				cout << "RetrieveBackTestPriceTable " << endl;
				map<pair<string, string>, vector<Trade>>::iterator mIt;
	
				while (!pRecSet->AdoNSEOF)
				{
					long lIndex = 0;
					memset((void*)ticker1, '\0', TICKER_LEN);
					memset((void*)ticker2, '\0', TICKER_LEN);
					memset((void*)date1, '\0', DATE_LEN);
					memset((void*)date2, '\0', DATE_LEN);
					strncpy_s(ticker1, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					strncpy_s(ticker2, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					strncpy_s(date1, bstr_t(pFields->GetItem(lIndex++)->GetValue()), DATE_LEN);
					pair<string, string> NewPair(ticker1, ticker2);
					
					aField = pFields->GetItem(lIndex++)->GetValue();
					open1 = aField;
					aField = pFields->GetItem(lIndex++)->GetValue();
					close1 = aField;
					aField = pFields->GetItem(lIndex++)->GetValue();
					open2 = aField;
					aField = pFields->GetItem(lIndex)->GetValue();
					close2 = aField;
					//the pointer begins to go through another pair
					if (NewPair != TickerPair)
					{
						//if it is not the first pair, we need to store data into the map first and clear it
						if (id != 0)
							TradeTable.insert(pair<pair<string, string>, vector<Trade>>(TickerPair, PairTrades));
						TickerPair = NewPair;
						auto it = TradeTable.find(NewPair);
						if (it == TradeTable.end())
							id++;
						PairTrades.clear();
						
					}
					else
					{
						//when it is the first line of the pair, we ignore the open price of the first line;
						//when it is not, we take care of open prices and closes prices
						Trade aTrade1(id, ticker1, ticker2, open1, open2, size1, size2, date1, date2); 
						cout << id << " " << ticker1 << " " << ticker2 << " " << date1 << endl;
						PairTrades.push_back(aTrade1);
					}
					Trade aTrade2(id, ticker1, ticker2, close1, close2, size1, size2, date1, date2);
					cout << id << " " << ticker1 << " " << ticker2 << " " << date1 << endl;
  					PairTrades.push_back(aTrade2);
					pRecSet->MoveNext();
				}
				TradeTable.insert(pair<pair<string, string>, vector<Trade>>(TickerPair, PairTrades));
			}
			pRecSet->Close();
			pConnect->Close();
		}
	}
	catch (_com_error& e)
	{
		// Handle Exception
		PrintComError(e);
	}
	// Release COM
	CoUninitialize();
	return 0;
}


int EnterPairTrade(map<pair<string, string>, PairPriceStd> & PairPriceStdTable)
{
	string ticker1;
	string ticker2;
	double stock1price1 = 0;
	double stock2price1 = 0;
	double stock1price2 = 0;
	double stock2price2 = 0;
	double sd = 0;
	double fProfit = 0;
	int kopen = 0;
	double kcloseupper = 0;
	double kcloselower = 0;
	double kdiverge = 0.0;
	double MeanRatio = 0.0;
	int vol1 = 0;
	int vol2 = 0;
	int status = 0;
	bool hold = false;
	
	cout << "Enter a pair trade:" << endl;
	cout << "ticker1: ";
	cin >> ticker1;
	getchar();
	cout << "ticker2: ";
	cin >> ticker2;
	getchar();
	cout << "Ticker 1 Previous Price: ";
	cin >> stock1price1;
	getchar();
	cout << "Ticker 2 Previous Price: ";
	cin >> stock2price1;
	getchar();
	cout << "Ticker 1 Present Price: ";
	cin >> stock1price2;
	getchar();
	cout << "Ticker 2 Present Price: ";
	cin >> stock2price2;
	getchar();

	pair<string, string> aTickerPair(ticker1, ticker2);
	map<pair<string, string>, PairPriceStd>::iterator mIt;
	mIt = PairPriceStdTable.find(aTickerPair);
	if (mIt == PairPriceStdTable.end())
	{
		cout << "Invalid pair: " << ticker1 << " " << ticker2 << endl;
		return -1;
	}
	sd = PairPriceStdTable[mIt->first].fGetPriceRatioStd();
	sd = sd / sqrt(28);
	kopen = PairPriceStdTable[mIt->first].iGetThreshold();
	MeanRatio = PairPriceStdTable[mIt->first].fGetMeanRatio();
	kcloseupper = PairPriceStdTable[mIt->first].Getkcloseupper();
	kcloselower = PairPriceStdTable[mIt->first].Getkcloselower();
	kdiverge = PairPriceStdTable[mIt->first].Getkdiverge();

	//trading volume
	vol1 = 10000;
	vol2 = static_cast<int>(vol1*(stock1price1 / stock2price1));

	//open position
	if (log(stock1price1 / stock2price1) > sd * kopen + MeanRatio)
	{
		cout << "Short the pair" << endl;
		status = -1;
		hold = true;
	}
	else if (log(stock1price1 / stock2price1) < -sd * kopen + MeanRatio)
	{
		cout << "Long the pair" << endl;
		status = 1;
		hold = true;
	}
	else
		cout << "Do not trade" << endl;
	if (hold)
	{
		//close position
		if ((log(stock2price1 / stock2price1) < sd * kcloseupper + MeanRatio &&  log(stock2price1 / stock2price1) > sd * kcloselower + MeanRatio)
			 || log(stock1price1 / stock2price1) > sd * kdiverge + MeanRatio)
		{
			if (status == 1)
				status = 2;
			else if (status == -1)
				status = -2;
			hold = false;
			cout << "Then close the position" << endl;
		}
		else
			cout << "Then hold the position" << endl;

		if (status == 2)
		{
			fProfit = vol1*(stock1price2 - stock1price1) - vol2*(stock2price2 - stock2price1);
			status = 0;
		}
		else if (status == -2)
		{
			fProfit = -vol1*(stock1price2 - stock1price1) + vol2*(stock2price2 - stock2price1);
			status = 0;
		}
		cout << vol1 << " " << vol2 << " " << fProfit << endl;
	}

	return 0;
}

void PrintComError(_com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());

	// Print Com errors.
	printf("Error\n");
	printf("\tCode = %08lx\n", e.Error());
	printf("\tCode meaning = %s\n", (char *)e.ErrorMessage());
	printf("\tSource = %s\n", (LPCSTR)bstrSource);
	printf("\tDescription = %s\n", (LPCSTR)bstrDescription);
}
#endif
#pragma once
