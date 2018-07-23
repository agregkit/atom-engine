#include "requestswap.h"

#include <sstream>

CRequestSwap::CRequestSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestSwap::~CRequestSwap()
{

}

void CRequestSwap::process()
{
	if (!document_->HasMember("first_cur") || !document_->HasMember("second_cur") || !document_->HasMember("first_addres") || !document_->HasMember("second_addres") || !document_->HasMember("secret_hash")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		secretHash_ = (*document_)["secret_hash"].GetString();
		std::string firstAddres = (*document_)["first_addres"].GetString();
		std::string secondAddres = (*document_)["second_addres"].GetString();
		std::string firstCur = (*document_)["first_cur"].GetString();
		std::string secondCur = (*document_)["second_cur"].GetString();

		std::stringstream key;
		key << "swaps:" << firstCur << "_" << secondCur << ":" << firstAddres << "_" << secondAddres;
		swapKey_ = key.str();

		if (document_->HasMember("first_contract") && document_->HasMember("first_contract_tx")) {
			firstContract_ = (*document_)["first_contract"].GetString();
			firstContractTx_ = (*document_)["first_contract_tx"].GetString();

			std::stringstream command;
			command << "GET " << swapKey_;
			redisAsyncCommand(redisContext_, &CRequestSwap::getFirstCallback, this, command.str().c_str());
		} else if (document_->HasMember("second_contract") && document_->HasMember("second_contract_tx")) {
			secondContract_ = (*document_)["second_contract"].GetString();
			secondContractTx_ = (*document_)["second_contract_tx"].GetString();

			std::stringstream command;
			command << "GET " << swapKey_;
			redisAsyncCommand(redisContext_, &CRequestSwap::getSecondCallback, this, command.str().c_str());
		} else {
			sendBadJSONError();
			if (onComplete_) {
				onComplete_(this);
			}
		}
	}
}

std::string CRequestSwap::getType()
{
	return "swap";
}

void CRequestSwap::getFirstCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestSwap* request = reinterpret_cast<CRequestSwap*>(data);
	if (value) {
		request->addToSwap(value->str, "first", "second", request->firstContract_, request->firstContractTx_);
	} else {
		request->newSwap("first", request->firstContract_, request->firstContractTx_);
	}
}

void CRequestSwap::getSecondCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestSwap* request = reinterpret_cast<CRequestSwap*>(data);
	if (value) {
		request->addToSwap(value->str, "second", "first", request->secondContract_, request->secondContractTx_);
	} else {
		request->newSwap("second", request->secondContract_, request->secondContractTx_);
	}
}

void CRequestSwap::newSwap(const std::string& key, const std::string& contract, const std::string& contractTx)
{
	std::stringstream value;
	value << "{\"secret_hash\": \"" << secretHash_ << "\", \"" << key << "\": {\"contract\": \"" << contract << "\", \"contract_tx\": \"" << contractTx << "\"}}";
	std::stringstream command;
	command << "SET " << swapKey_ << " " << value.str();
	redisAsyncCommand(redisContext_, nullptr, nullptr, command.str().c_str());
	sendSuccess();
	if (onComplete_) {
		onComplete_(this);
	}
}

void CRequestSwap::addToSwap(const std::string& info, const std::string& key, const std::string& rkey, const std::string& contract, const std::string& contractTx)
{
	rapidjson::Document document;
	if (!document.Parse(info.c_str()).HasParseError() && document.HasMember("secret_hash") && document.HasMember(rkey.c_str())) {
		if (document[rkey.c_str()].IsObject() && document[rkey.c_str()].HasMember("contract") && document[rkey.c_str()].HasMember("contract_tx")) {
			std::string rcontract = document[rkey.c_str()]["contract"].GetString();
			std::string rcontractTx = document[rkey.c_str()]["contract_tx"].GetString();
			std::stringstream value;
			value << "{\"secret_hash\": \"" << secretHash_ << "\", \"" << rkey << "\": {\"contract\": \"" << rcontract << "\", \"contract_tx\": \"" << rcontractTx << "\"}, \"" << key << "\": {\"contract\": \"" << contract << "\", \"contract_tx\": \"" << contractTx << "\"}}";
			std::stringstream command;
			command << "SET " << swapKey_ << " " << value.str();
			redisAsyncCommand(redisContext_, nullptr, nullptr, command.str().c_str());
		}
	}
	sendSuccess();
	if (onComplete_) {
		onComplete_(this);
	}
}