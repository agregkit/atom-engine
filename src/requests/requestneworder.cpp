#include "requestneworder.h"

#include <sstream>

CRequestNewOrder::CRequestNewOrder(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestNewOrder::~CRequestNewOrder()
{

}

void CRequestNewOrder::process()
{
	if (!document_->HasMember("get_addres") || !document_->HasMember("send_cur") || !document_->HasMember("send_count") || !document_->HasMember("get_cur") || !document_->HasMember("get_count")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		std::string getAddres = (*document_)["get_addres"].GetString();
		std::string sendCur = (*document_)["send_cur"].GetString();
		std::string getCur = (*document_)["get_cur"].GetString();
		int sendCount = (*document_)["send_count"].GetInt();
		int getCount = (*document_)["get_count"].GetInt();

		std::stringstream key;
		std::stringstream value;
		std::stringstream command;

		key << "orders:" << sendCur << "_" << getCur << ":" << getAddres;
		value << "{\"send_count\": " << sendCount << ", \"get_count\": " << getCount << "}";
		command << "SET " << key.str() << " " << value.str();

		redisAsyncCommand(redisContext_, nullptr, nullptr, command.str().c_str());

		sendSuccess();

		if (onComplete_) {
			onComplete_(this);
		}
	}
}

std::string CRequestNewOrder::getType()
{
	return "new_order";
}