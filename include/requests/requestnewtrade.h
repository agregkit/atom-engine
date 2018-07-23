#ifndef RequestNewTradeH
#define RequestNewTradeH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestNewTrade : public CRequest
{
public:
	CRequestNewTrade(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestNewTrade();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getCallback(redisAsyncContext* context, void* res, void* data);
private:
	std::string firstAddres_;
	std::string secondAddres_;
	std::string firstCur_;
	std::string secondCur_;
};

#endif