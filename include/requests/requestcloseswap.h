#ifndef RequestCloseSwapH
#define RequestCloseSwapH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestCloseSwap : public CRequest
{
public:
	CRequestCloseSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestCloseSwap();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getCallback(redisAsyncContext* context, void* res, void* data);
private:
	std::string swapKey_;
	bool firstClose_;
	bool secondClose_;
};

#endif