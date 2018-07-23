#include "requestgetswap.h"

#include <sstream>

CRequestGetSwap::CRequestGetSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestGetSwap::~CRequestGetSwap()
{

}

void CRequestGetSwap::process()
{
	if (!document_->HasMember("first_cur") || !document_->HasMember("second_cur") || !document_->HasMember("first_address") || !document_->HasMember("second_addres")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		std::string firstCur = (*document_)["first_cur"].GetString();
		std::string secondCur = (*document_)["second_cur"].GetString();
		std::string firstAddres = (*document_)["first_address"].GetString();
		std::string secondAddres = (*document_)["second_addres"].GetString();

		std::stringstream key;
		key << "swaps:" << firstCur << "_" << secondCur << ":" << firstAddres << "_" << secondAddres;
		std::string swapKey = key.str();

		std::stringstream command;
		command << "GET " << key.str();

		redisAsyncCommand(redisContext_, &CRequestGetSwap::getCallback, this, command.str().c_str());
	}
}

std::string CRequestGetSwap::getType()
{
	return "get_swap";
}

void CRequestGetSwap::getCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestGetSwap* request = reinterpret_cast<CRequestGetSwap*>(data);
	if (value) {
		request->sendReply(value->str);
	} else {
		request->sendReply("{\"result\": \"swap not found\"}");
	}
	if (request->onComplete_) {
		request->onComplete_(request);
	}
}