#ifndef RequestSwapH
#define RequestSwapH

#include "request.h"

#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

class CRequestSwap : public CRequest
{
public:
	CRequestSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequestSwap();

	virtual void process() override;
	virtual std::string getType() override;
private:
	static void getFirstCallback(redisAsyncContext* context, void* res, void* data);
	static void getSecondCallback(redisAsyncContext* context, void* res, void* data);
	void newSwap(const std::string& key, const std::string& contract, const std::string& contractTx);
	void addToSwap(const std::string& info, const std::string& key, const std::string& rkey, const std::string& contract, const std::string& contractTx);
private:
	std::string swapKey_;
	std::string secretHash_;
	std::string firstContract_;
	std::string firstContractTx_;
	std::string secondContract_;
	std::string secondContractTx_;
};

#endif