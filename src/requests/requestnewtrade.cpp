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
	if (!document_->HasMember("send_addres") || !document_->HasMember("get_addres") || !document_->HasMember("send_cur") || !document_->HasMember("get_cur")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		sendAddres_ = (*document_)["get_addres"].GetString();
		getAddres_ = (*document_)["send_addres"].GetString();
		sendCur_ = (*document_)["get_cur"].GetString();
		getCur_ = (*document_)["send_cur"].GetString();

		std::stringstream key;
		std::stringstream command;
		key << "orders:" << sendCur_ << "_" << getCur_ << ":" << getAddres_;
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
		if (!document.Parse(value->str).HasParseError() && document.HasMember("send_count") && document.HasMember("get_count")) {
			std::stringstream key;
			std::stringstream command;
			key << "trades:" << request->sendCur_ << "_" << request->getCur_ << ":" << request->sendAddres_ << "_" << request->getAddres_;
			command << "SET " << key.str() << " " << value->str;
			redisAsyncCommand(request->redisContext_, nullptr, nullptr, command.str().c_str());
		}
	}

	request->sendSuccess();

	if (request->onComplete_) {
		request->onComplete_(request);
	}
}