#ifndef RequestRedeemSwapH
#define RequestRedeemSwapH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestRedeemSwap : public CRequest
{
public:
	CRequestRedeemSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestRedeemSwap();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getFirstCallback(redisAsyncContext* context, void* res, void* data);
	static void getSecondCallback(redisAsyncContext* context, void* res, void* data);
	void setRedemption(const std::string& info, const std::string& key, const std::string& redemptionTx);
private:
	std::string swapKey_;
	std::string redemptionTx_;
};

#endif