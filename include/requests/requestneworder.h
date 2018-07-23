#ifndef RequestNewOrderH
#define RequestNewOrderH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestNewOrder : public CRequest
{
public:
	CRequestNewOrder(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestNewOrder();

	virtual void process() override;
	virtual std::string getType() override;
};

#endif