#include "requestorders.h"

#include <hiredis/async.h>

CRequestOrders::CRequestOrders(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete),
	isFirstKey_(true)
{

}

CRequestOrders::~CRequestOrders()
{

}

void CRequestOrders::process()
{
	if (!document_->HasMember("send_cur") || !document_->HasMember("get_cur")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		std::string sendCur = (*document_)["send_cur"].GetString();
		std::string getCur = (*document_)["get_cur"].GetString();

		std::stringstream key;
		std::stringstream command;
		key << "orders:" << sendCur << "_" << getCur << ":*";
		command << "keys " << key.str();

		redisAsyncCommand(redisContext_, &CRequestOrders::getCallback, this, command.str().c_str());
	}
}

std::string CRequestOrders::getType()
{
	return "orders";
}

void CRequestOrders::getCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestOrders* request = reinterpret_cast<CRequestOrders*>(data);
	if (value) {
		for (size_t i = 0; i < value->elements; ++i) {
			std::string str = value->element[i]->str;
			request->keys_.push_back(str);
		}
		if (request->keys_.empty()) {
			request->sendReply("{[]}");
			if (request->onComplete_) {
				request->onComplete_(request);
			}
		} else {
			request->res_ << "{[";
			request->getNextValue();
		}
	} else {
		request->sendReply("{[]}");
		if (request->onComplete_) {
			request->onComplete_(request);
		}
	}
}

void CRequestOrders::getValueCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestOrders* request = reinterpret_cast<CRequestOrders*>(data);
	if (request->keys_.empty()) {
		request->res_ << "]}";
		request->sendReply(request->res_.str());
		if (request->onComplete_) {
			request->onComplete_(request);
		}
	} else {
		rapidjson::Document doc;
		if (!doc.Parse(value->str).HasParseError() && doc.HasMember("send_count") && doc.HasMember("get_count")) {
			std::stringstream ss;
			ss << "{\"key\": " << request->curKey_ << ", \"send_count\": " << doc["send_count"].GetInt() << ", \"get_count\": " << doc["get_count"].GetInt() << "}";
			if (!request->isFirstKey_) {
				request->res_ << ", ";
			}
			request->isFirstKey_ = false;	
			request->res_ << ss.str();
		}
		request->getNextValue();
	}
}

void CRequestOrders::getNextValue()
{
	auto it = keys_.begin();
	curKey_ = *it;
	keys_.erase(it);
	std::stringstream command;
	command << "GET " << curKey_;
	redisAsyncCommand(redisContext_, &CRequestOrders::getValueCallback, this, command.str().c_str());
}