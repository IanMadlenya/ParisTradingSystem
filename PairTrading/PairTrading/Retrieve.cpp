#include "stdafx.h"
#include "Retrieve.h"
using namespace std;

int Retrieve(map<int, Pair> & PairsTable)
{
	int id = -1;
	char ticker1[TICKER_LEN];
	char ticker2[TICKER_LEN];
	float price1 = 0;
	float price2 = 0;
	float std1 = 0;
	float std2 = 0;
	int k = 0;

	// Connection string for ADO 
	_bstr_t bstrConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Documents and Settings\\Song Tang\\My Documents\\PairTradingDb.accdb;";
	   HRESULT hResult = CoInitialize( 0 );
    if( FAILED( hResult ))
    {
        return -1;
    }
    try
    {
        ADODB::_ConnectionPtr pConnect("ADODB.Connection");
        hResult = pConnect->Open( bstrConnect, "admin", "", ADODB::adConnectUnspecified );
        if (SUCCEEDED(hResult))
        {
			_bstr_t query = "SELECT * FROM PairPricesAvgStd;";
            ADODB::_RecordsetPtr pRecSet( "ADODB.Recordset" );
            hResult = pRecSet->Open( query, _variant_t((IDispatch *) pConnect, true), ADODB::adOpenUnspecified,
                                     ADODB::adLockUnspecified, ADODB::adCmdText);
            if( SUCCEEDED( hResult ))
            {
                ADODB::Fields* pFields = NULL;
                hResult = pRecSet->get_Fields( &pFields );
                _variant_t aField;

                while( !pRecSet->AdoNSEOF )
                {
					long lIndex=0;
					memset((void*)ticker1, '\0', TICKER_LEN);
					memset((void*)ticker2, '\0', TICKER_LEN);
					aField = pFields->GetItem(lIndex++)->GetValue();
					id = int(aField);
					strncpy_s(ticker1, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					strncpy_s(ticker2, bstr_t(pFields->GetItem(lIndex++)->GetValue()), TICKER_LEN);
					aField = pFields->GetItem(lIndex++)->GetValue();
					price1 = float(aField);
					aField = pFields->GetItem(lIndex++)->GetValue();
					price2 = float(aField);
					aField = pFields->GetItem(lIndex++)->GetValue();
					std1 = float(aField);
					aField = pFields->GetItem(lIndex)->GetValue();
					std2 = float(aField);
					Pair aPair(id, ticker1, ticker2, price1, price2, std1, std2, k);
					PairsTable.insert(pair<int, Pair>(id, aPair));
                    pRecSet->MoveNext();
                }
            }
            pRecSet->Close();
            pConnect->Close();
        }
    }
    catch( _com_error& e )
    {
        // Handle Exception
    }
 
    // Release COM
    CoUninitialize();

	return 0;
}