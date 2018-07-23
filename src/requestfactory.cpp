#include "requestfactory.h"

#include "request.h"
#include "requestnewtrade.h"
#include "requestneworder.h"
#include "requestorders.h"
#include "requesttrades.h"
#include "requesttrade.h"
#include "requestswap.h"
#include "requestgetswap.h"
#include "requestredeemswap.h"
#include "requestcloseswap.h"

#include <document.h>
#include <string>

TRequestUniquePtr CRequestFactory::createRequest(redisAsyncContext* redisContext, evhttp_request* req, OnRequestProcessingCompleteFunc onComplete)
{
	evbuffer *requestBuffer = evhttp_request_get_input_buffer(req);
	size_t requestLen = evbuffer_get_length(requestBuffer);
	char* requestDataBuffer = new char[requestLen];
	evbuffer_copyout(requestBuffer, requestDataBuffer, requestLen);

	std::unique_ptr<rapidjson::Document> document = std::make_unique<rapidjson::Document>();
	if (!document->Parse(requestDataBuffer).HasParseError() && document->IsObject() && document->HasMember("type")) {
		TRequestUniquePtr request;

		std::string requestType = (*document)["type"].GetString();
		if (requestType == "new_order") {
			request = std::make_unique<CRequestNewOrder>(redisContext, req, std::move(document), onComplete);
		} 
		if (requestType == "new_trade") {
			request = std::make_unique<CRequestNewTrade>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "orders") {
			request = std::make_unique<CRequestOrders>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "trades") {
			request = std::make_unique<CRequestTrades>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "trade") {
			request = std::make_unique<CRequestTrade>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "swap") {
			request = std::make_unique<CRequestSwap>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "get_swap") {
			request = std::make_unique<CRequestGetSwap>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "redeem_swap") {
			request = std::make_unique<CRequestRedeemSwap>(redisContext, req, std::move(document), onComplete);
		}
		if (requestType == "close_swap") {
			request = std::make_unique<CRequestCloseSwap>(redisContext, req, std::move(document), onComplete);
		}

		return request;
	}

	delete[] requestDataBuffer;
	return TRequestUniquePtr();
}