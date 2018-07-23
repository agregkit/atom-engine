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
	if (!document_->HasMember("second_addres") || !document_->HasMember("first_cur") || !document_->HasMember("first_count") || !document_->HasMember("second_cur") || !document_->HasMember("second_count")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		std::string secondAddres = (*document_)["second_addres"].GetString();
		std::string firstCur = (*document_)["first_cur"].GetString();
		std::string secondCur = (*document_)["second_cur"].GetString();
		int firstCount = (*document_)["first_count"].GetInt();
		int secondCount = (*document_)["second_count"].GetInt();

		std::stringstream key;
		std::stringstream value;
		std::stringstream command;

		key << "orders:" << firstCur << "_" << secondCur << ":" << secondAddres;
		value << "{\"first_count\": " << firstCount << ", \"second_count\": " << secondCount << "}";
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