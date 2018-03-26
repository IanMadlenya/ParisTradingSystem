#pragma once
// Example Bloomberg API C++ Program (c)2014 Richard Holowczak
// Portions (c) Bloomberg, LLC

// Bloomberg API Includes
#include <blpapi_defs.h>
#include <blpapi_correlationid.h>
#include <blpapi_element.h>
#include <blpapi_event.h>
#include <blpapi_exception.h>
#include <blpapi_message.h>
#include <blpapi_session.h> 
#include <blpapi_subscriptionlist.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>  //for time(NULL)
#include "Trade.h"
//#include <unordered_map> ////////////////////////////

using  namespace BloombergLP;
using  namespace blpapi;

int run(int argc, char* argv[], time_t timeInterval1, std::vector<std::string> tickers1,
	std::vector<pair<std::string, std::string>> PairsInfo1,
	std::map< pair<std::string, std::string>, std::vector<Trade> > &TradePair);

class BloombergEventHandler : public EventHandler
{
private:
	time_t t0;
	time_t timeInterval;
	std::vector<int> CountPair;
	std::vector<std::string> tickers;
	std::vector<pair<std::string, std::string>> PairsInfo;
	//unordered_map containers are faster than map containers to access individual elements by their key, although they are generally less efficient for range iteration through a subset of their elements.
	//std::unordered_map < pair<int, std::string>, EachStock > EachStockInfo;
	//std::map < std::string, EachStock > EachStockInfo;
	std::map < pair<int, std::string>, EachStock > EachStockInfo;
	std::map< pair<std::string, std::string>, std::vector<Trade> > TradePair;
	

public:
	// Empty constructor
	BloombergEventHandler()
	{
	}
	void SetMember(time_t t01, time_t timeInterval1, std::vector<std::string> tickers1,
		std::vector<pair<std::string, std::string>> PairsInfo1)
	{
		t0 = t01;
		timeInterval = timeInterval1;
		tickers = tickers1;
		PairsInfo = PairsInfo1;
		CountPair.resize(PairsInfo1.size());
		std::fill(CountPair.begin(), CountPair.end(), 0);
	}
	std::map< pair<std::string, std::string>, std::vector<Trade> > GetTradePair()
	{
		return TradePair;
	}
	// The actual event handler method
	bool processEvent(const Event &event, Session *session) //where to call?
	{
		// We are only interested in market data events related to
		// our subscription
		if (event.eventType() == Event::SUBSCRIPTION_DATA)
		{
			// Each event may have multiple messages. So
			// create a message iterator
			MessageIterator msgIter(event);
			// Iterate over each of the messages

			while (msgIter.next())
			{
				time_t t1 = time(NULL);

				// Get one message and store it in msg

				Message msg = msgIter.message();
				// For Debugging use the msg.print method to
				// display all of the fields:
				//msg.print(std::cout);
				// std::cout << std::endl;
				// Declare a pointer to the topic of the message
				const char *topic = (char *)msg.correlationId().asPointer();
				// If the elements are present, then print out the message 
				if (msg.hasElement("LAST_TRADE", true) &&
					msg.hasElement("SIZE_LAST_TRADE", true))
				{
					// Show the topic
					std::string ticker = topic;
					ticker = ticker.substr(0, ticker.find(" "));
					std::cout << "Ticker:   " << ticker << ",";

					// Fetch the necessary elements from the message
					double price = msg.getElementAsFloat64("LAST_TRADE");
					int size_lasttrade = msg.getElementAsInt32("SIZE_LAST_TRADE");
				    std::string EventTime = msg.getElementAsString("TIME");
					std::cout << price << ",";
					std::cout << size_lasttrade;
					std::cout << std::endl;

					char ttemp[200];
					strftime(ttemp, sizeof(ttemp), "%Y/%m/%d %H:%M:%S", localtime(&t1));//localtime_s()
					std::cout << "Time: " << ttemp << endl;

					time_t xx = t1 - t0;
					cout << "time interval : "<<xx << endl;
					if (t1 - t0 > timeInterval) {
						std::cout << "t1" << t1 << endl;
						t0 = time(0);
						std::cout << "new t0" << t0 << endl;
						std::cout << "---------Clear map: " << endl;
						EachStockInfo.clear();
						std::fill(CountPair.begin(), CountPair.end(), 0);
					}
					//std::map < std::string, EachStock >::iterator it;
					std::map < pair<int, std::string>, EachStock >::iterator it;
					std::vector<pair<std::string, std::string>>::iterator ItPair;
					int id = 0;
					double p1, p2;
					int s1, s2;
					string ticker1, ticker2, t1, t2;
					for (ItPair = PairsInfo.begin(); ItPair != PairsInfo.end(); ItPair++)
					{
						id++;
						if (CountPair[id - 1] == 2)
							continue;
						if ( ticker != (*ItPair).first &&  ticker != (*ItPair).second)
							continue;
						it = EachStockInfo.find(make_pair(id, ticker));
						cout << "Update the ticker in EchaStockInfo for pair "<<id << endl;
						//Update the corresponding ticker info
						EachStockInfo[make_pair(id, ticker)] = EachStock(ticker, price, size_lasttrade, EventTime);
						
						if (it == EachStockInfo.end())
						{
							CountPair[id - 1]++;
							//cout << "CountPair "<<id<<"  "<<CountPair[id - 1] << endl;
							
							if (CountPair[id - 1] == 2)
							{
								cout << "Get the real time price for pair " << id << endl;
								p1 = EachStockInfo[make_pair(id, (*ItPair).first)].fGetPrice();
								s1 = EachStockInfo[make_pair(id, (*ItPair).first)].fGetSize();
								t1 = EachStockInfo[make_pair(id, (*ItPair).first)].sGetDate();
								p2 = EachStockInfo[make_pair(id, (*ItPair).second)].fGetPrice();
								s2 = EachStockInfo[make_pair(id, (*ItPair).second)].fGetSize();
								t2 = EachStockInfo[make_pair(id, (*ItPair).second)].sGetDate();

								Trade tra(id, (*ItPair).first, (*ItPair).second, p1, p2, s1, s2, t1, t2);
								TradePair[*ItPair].push_back(tra);
							}
						}		
						
						//pairs info complete
						
					}

				} // end if hasElement

			} // while loop over msgIter

		} // end if eventType is subscription
		return true;
	} // end bool processEvent


}; // end class BloombergEventHandler

int run(int argc, char* argv[], time_t timeInterval1, std::vector<std::string> tickers1,
	std::vector<pair<std::string, std::string>> PairsInfo1, 
	std::map< pair<std::string, std::string>, std::vector<Trade> > &TradePair)//do I need to pass??
{
	int result = 0;
	// Define a security and a field
	std::cout << "Start to retrieve real time price and calculate profit now, press any key and enter when you want to stop:"
		<< std::endl;
	const char *fields = "LAST_PRICE";
	// Define a pointer to our event handler based on our class
	BloombergEventHandler *sessionEventHandler;
	// Set of sessionOptions
	SessionOptions sessionOptions;
	sessionOptions.setServerHost("localhost");
	sessionOptions.setServerPort(8194);
	// Instantiate the new EventHandler object
	sessionEventHandler = new BloombergEventHandler();
	// Create a Session object using the sessionOptions
	// and event handler
	Session session(sessionOptions, sessionEventHandler);
	// Start the Session
	result = session.start();
	// Open up the Service to the market data feed
	result = session.openService("//blp/mktdata");

	// Create a list of subscriptions
	SubscriptionList subscriptions;
	//Set up the current system time as t01 
	time_t t01 = time(NULL);
	sessionEventHandler->SetMember(t01, timeInterval1, tickers1, PairsInfo1);
	///////////////////////////////???????????????????????????????????????????????????
	// Add a subscription and create the CorrelationId on the fly
	for (size_t numstock = 0; numstock < tickers1.size(); numstock++) {
		tickers1[numstock] = tickers1[numstock] + " US Equity";
		subscriptions.add(tickers1[numstock].c_str(), fields, "",
			CorrelationId((char *)tickers1[numstock].c_str()));
		//if here not using vector, the CorrelationId would be same for every ticker.
		//std::cout << CorrelationId((char *)tickers[numstock].c_str()) << std::endl;
	}

	// Kick off the subscriptions
	session.subscribe(subscriptions);
	// At this point we are all done. Events should start
	// coming in and our event handler will get called one
	// time for each event.
	// The "main" thread will sit here below waiting for you
	// to type in a number and press enter.
	std::cin >> result;
	// Cancel our subscription 
	session.unsubscribe(subscriptions);
	// Stop this session
	session.stop();
	TradePair = sessionEventHandler->GetTradePair();
	return 0;
} // end Main
