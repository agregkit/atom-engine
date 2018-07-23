#ifndef RequestTradeH
#define RequestTradeH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestTrade : public CRequest
{
public:
	CRequestTrade(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestTrade();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getCallback(redisAsyncContext* context, void* res, void* data);
	static void getTradeCallback(redisAsyncContext* context, void* res, void* data);
private:
	std::string secondAddres_;
	std::string keyPrefix_;
	std::string tradeKey_;
};

#endif