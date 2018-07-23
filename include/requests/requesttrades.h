#ifndef RequestTradesH
#define RequestTradesH

#include "request.h"

#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>
#include <sstream>

typedef std::vector<std::string> TKeys;

class CRequestTrades : public CRequest
{
public:
	CRequestTrades(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestTrades();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getCallback(redisAsyncContext* context, void* res, void* data);
	static void getValueCallback(redisAsyncContext* context, void* res, void* data);
	void getNextValue();
private:
	TKeys keys_;
	std::stringstream res_;
	std::string curKey_;
	bool isFirstKey_;
};

#endif