#include "requestnewtrade.h"

#include <sstream>
#include <hiredis/async.h>

CRequestNewTrade::CRequestNewTrade(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestNewTrade::~CRequestNewTrade()
{

}

void CRequestNewTrade::process()
{
	if (!document_->HasMember("first_addres") || !document_->HasMember("second_addres") || !document_->HasMember("first_cur") || !document_->HasMember("second_cur")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		firstAddres_ = (*document_)["first_addres"].GetString();
		secondAddres_ = (*document_)["second_addres"].GetString();
		firstCur_ = (*document_)["first_cur"].GetString();
		secondCur_ = (*document_)["second_cur"].GetString();

		std::stringstream key;
		std::stringstream command;
		key << "orders:" << firstCur_ << "_" << secondCur_ << ":" << secondAddres_;
		command << "GET " << key.str();

		redisAsyncCommand(redisContext_, &CRequestNewTrade::getCallback, this, command.str().c_str());
	}
}

std::string CRequestNewTrade::getType()
{
	return "new_trade";
}

void CRequestNewTrade::getCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestNewTrade* request = reinterpret_cast<CRequestNewTrade*>(data);
	if (value) {
		rapidjson::Document document;
		if (!document.Parse(value->str).HasParseError() && document.HasMember("first_count") && document.HasMember("second_count")) {
			std::stringstream keyDel;
			std::stringstream commandDel;
			keyDel << "orders:" << request->firstCur_ << "_" << request->secondCur_ << ":" << request->secondAddres_;
			commandDel << "DEL " << keyDel.str();
			redisAsyncCommand(request->redisContext_, nullptr, nullptr, commandDel.str().c_str());

			std::stringstream key;
			std::stringstream command;
			key << "trades:" << request->firstCur_ << "_" << request->secondCur_ << ":" << request->firstAddres_ << "_" << request->secondAddres_;
			command << "SET " << key.str() << " " << value->str;
			redisAsyncCommand(request->redisContext_, nullptr, nullptr, command.str().c_str());
		}
	}

	request->sendSuccess();

	if (request->onComplete_) {
		request->onComplete_(request);
	}
}