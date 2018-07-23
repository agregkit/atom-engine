#include "requesttrade.h"

#include <sstream>
#include <hiredis/async.h>
#include <string>

CRequestTrade::CRequestTrade(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestTrade::~CRequestTrade()
{

}

void CRequestTrade::process()
{
	if (!document_->HasMember("second_addres") || !document_->HasMember("first_cur") || !document_->HasMember("second_cur")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		secondAddres_ = (*document_)["second_addres"].GetString();
		std::string firstCur = (*document_)["first_cur"].GetString();
		std::string secondCur = (*document_)["second_cur"].GetString();

		std::stringstream key;
		std::stringstream value;
		std::stringstream command;

		key << "trades:" << firstCur << "_" << secondCur;
		keyPrefix_ = key.str();
		key << ":*";
		command << "keys " << key.str();

		redisAsyncCommand(redisContext_, &CRequestTrade::getCallback, this, command.str().c_str());

		sendSuccess();

		if (onComplete_) {
			onComplete_(this);
		}
	}
}

std::string CRequestTrade::getType()
{
	return "trade";
}

void CRequestTrade::getCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestTrade* request = reinterpret_cast<CRequestTrade*>(data);
	if (value) {
		bool tradeFounded = false;
		for (size_t i = 0; i < value->elements; ++i) {
			std::string str = value->element[i]->str;
			size_t pos = str.rfind(request->secondAddres_);
			if (str.find(request->keyPrefix_) != std::string::npos && pos != std::string::npos && pos + request->secondAddres_.length() == str.length()) {
				tradeFounded = true;
				request->tradeKey_ = str;
				break;
			}
		}
		if (tradeFounded) {
			std::stringstream command;
			command << "GET " << request->tradeKey_;
			redisAsyncCommand(request->redisContext_, &CRequestTrade::getTradeCallback, request, command.str().c_str());
		} else {
			request->sendReply("{\"result\": \"trade not found\"}");
		}
	} else {
		request->sendReply("{\"result\": \"trade not found\"}");
	}

	if (request->onComplete_) {
		request->onComplete_(request);
	}
}

void CRequestTrade::getTradeCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestTrade* request = reinterpret_cast<CRequestTrade*>(data);
	if (value) {
		rapidjson::Document doc;
		std::stringstream ss;
		if (!doc.Parse(value->str).HasParseError() && doc.HasMember("first_count") && doc.HasMember("second_count")) {
			ss << "{\"key\": " << request->tradeKey_ << ", \"first_count\": " << doc["first_count"].GetInt() << ", \"second_count\": " << doc["second_count"].GetInt() << "}";
		}
		request->sendReply(ss.str());
	} else {
		request->sendReply("{\"result\": \"trade not found\"}");
	}

	if (request->onComplete_) {
		request->onComplete_(request);
	}
}