#pragma once
/* Copyright 2012. Bloomberg Finance L.P.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:  The above
* copyright notice and this permission notice shall be included in all copies
* or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#include "HistoryBloomberg.h"
#include <blpapi_session.h>
#include <blpapi_eventdispatcher.h>

#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_element.h>
#include <blpapi_name.h>
#include <blpapi_request.h>
#include <blpapi_subscriptionlist.h>
#include <blpapi_defs.h>
#include <blpapi_exception.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace BloombergLP;
using namespace blpapi;


void HistoryBloomberg::run(int argc, char **argv)
{

	d_host = "localhost";
	d_port = 8194;
	if (!parseCommandLine(argc, argv)) return;

	SessionOptions sessionOptions;
	sessionOptions.setServerHost(d_host.c_str());
	sessionOptions.setServerPort(d_port);

	std::cout << "Connecting to " << d_host << ":" << d_port << std::endl;
	Session session(sessionOptions);
	if (!session.start()) {
		std::cerr << "Failed to start session." << std::endl;
		return;
	}
	if (!session.openService("//blp/refdata")) {
		std::cerr << "Failed to open //blp/refdata" << std::endl;
		return;
	}
	Service refDataService = session.getService("//blp/refdata");
	Request request = refDataService.createRequest("HistoricalDataRequest");


	std::string ticker_;
	for (size_t m = 0; m<tickerlist.size(); m++)
	{
		ticker_ = tickerlist[m] + " US Equity";
		//const char *cstr1 = ticker_.c_str(); //change the datatype of ticker to pointer to const char and append them to securities
		request.getElement("securities").appendValue(ticker_.c_str());
	}

	request.getElement("fields").appendValue("PX_LAST");
	request.getElement("fields").appendValue("OPEN");
	request.set("periodicityAdjustment", "ACTUAL");
	request.set("periodicitySelection", "DAILY");
	request.set("startDate", start_day.c_str());
	request.set("endDate", end_day.c_str());
	//request.set("maxDataPoints", 200);

	std::cout << "Sending Request: " << request << std::endl;

	try {
		session.sendRequest(request);
	}
	catch (Exception e) {
		std::cout << "Cannot send request" << std::endl;
		return;
	}

	while (true)
	{
		Event event = session.nextEvent();
		MessageIterator msgIter(event);

		while (msgIter.next())
		{
			Message msg = msgIter.message();
			if ((event.eventType() != Event::PARTIAL_RESPONSE) &&
				(event.eventType() != Event::RESPONSE))//Event.EventType.Constants.RESPONSE: final event
			{
				continue;
			}
			Element securityDataArray = msg.getElement("securityData");

			//error processing
			if (securityDataArray.hasElement("securityError"))
			{
				Element securityError =
					securityDataArray.getElement("securityError");
				securityError.print(std::cout);
			}

			//exception processing
			Element field_exceptions =
				securityDataArray.getElement("fieldExceptions");
			if (field_exceptions.numValues() > 0)
			{
				Element element = field_exceptions.getValueAsElement(0);
				Element field_id = element.getElement("fieldId");
				Element error_info = element.getElement("errorInfo");
				Element error_message = error_info.getElement("message");
				std::cout << field_id << "\n";
				std::cout << error_message << "\n";
			}

			char cdate[DATE_LEN];
			double open;
			double close;

			//Element securityName = securityDataArray.getElement("security");
			std::string ticker_temp = securityDataArray.getElementAsString("security");
			Element fieldData = securityDataArray.getElement("fieldData");

			std::string::size_type pos;
			pos = ticker_temp.find(' ', 0);
			ticker_temp = ticker_temp.substr(0, pos);
			Ticker OneTicker(ticker_temp);

			int numItems = fieldData.numValues();
			for (int i = 0; i < numItems; ++i)
			{
				Element DayData = fieldData.getValueAsElement(i);

				memset((void*)cdate, '\0', DATE_LEN);
				std::string date = DayData.getElementAsString("date");
				strcpy_s(cdate, date.c_str());
				open = 0.0;
				close = 0.0;

				if (DayData.hasElement("OPEN"))
				{
					open = DayData.getElementAsFloat64("OPEN");
					close = DayData.getElementAsFloat64("PX_LAST");
				}
				OneTicker.SetTradeInfo(open, close, cdate);
			}
			VectorTicker.push_back(OneTicker);
		}
		if (event.eventType() == Event::RESPONSE)
		{
			break;
		}
	}

}