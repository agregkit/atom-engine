#include "requestcloseswap.h"

#include <sstream>

CRequestCloseSwap::CRequestCloseSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete),
	firstClose_(false),
	secondClose_(false)
{

}

CRequestCloseSwap::~CRequestCloseSwap()
{

}

void CRequestCloseSwap::process()
{
	if (!document_->HasMember("first_cur") || !document_->HasMember("second_cur") || !document_->HasMember("first_addres") || !document_->HasMember("second_addres")) {
		sendBadJSONError();
		if (onComplete_) {
			onComplete_(this);
		}
	} else {
		std::string firstAddres = (*document_)["first_addres"].GetString();
		std::string secondAddres = (*document_)["second_addres"].GetString();
		std::string firstCur = (*document_)["first_cur"].GetString();
		std::string secondCur = (*document_)["second_cur"].GetString();

		std::stringstream key;
		key << "swaps:" << firstCur << "_" << secondCur << ":" << firstAddres << "_" << secondAddres;
		swapKey_ = key.str();

		if (document_->HasMember("first_close")) {
			firstClose_ = (*document_)["first_close"].GetBool();
		} 
		if (document_->HasMember("second_redemption_tx")) {
			firstClose_ = (*document_)["second_close"].GetBool();
		} 

		if (firstClose_ || secondClose_) {
			std::stringstream command;
			command << "GET " << swapKey_;
			redisAsyncCommand(redisContext_, &CRequestCloseSwap::getCallback, this, command.str().c_str());
		} else {
			sendBadJSONError();
			if (onComplete_) {
				onComplete_(this);
			}
		}
	}
}

std::string CRequestCloseSwap::getType()
{
	return "close_swap";
}

void CRequestCloseSwap::getCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestCloseSwap* request = reinterpret_cast<CRequestCloseSwap*>(data);
	if (value) {
		rapidjson::Document document;
		document.Parse(value->str);

		std::string secretHash = document["secret_hash"].GetString();
		std::string firstContract;
		std::string secondContract;
		std::string firstContractTx;
		std::string secondContractTx;

		bool withFirst = false;
		bool withSecond = false;
		if (document.HasMember("first") && !request->firstClose_) {
			withFirst = true;
			firstContract = document["first"]["contract"].GetString();
			firstContractTx = document["first"]["contract_tx"].GetString();
		}
		if (document.HasMember("second") && !request->secondClose_) {
			withSecond = true;
			secondContract = document["second"]["contract"].GetString();
			secondContractTx = document["second"]["contract_tx"].GetString();
		}

		bool withFirstRedemption = false;
		bool withSecondRedemption = false;

		std::string firstRedemption;
		std::string secondRedemption;
		if (withFirst && document["first"].HasMember("redemption_tx")) {
			withFirstRedemption = true;
			firstRedemption = document["first"]["redemption_tx"].GetString();
		}
		if (withSecond && document["second"].HasMember("redemption_tx")) {
			withSecondRedemption = true;
			secondRedemption = document["second"]["redemption_tx"].GetString();
		}

		std::stringstream command;
		if (!withFirst && !withSecond) {
			command << "DEL " << request->swapKey_;
		} else {
			std::stringstream value;
			value << "{\"secret_hash\": \"" << secretHash << "\", \"";
			if (withFirst) {
				value << "first\": {\"contract\": \"" << firstContract << "\", \"contract_tx\": \"" << firstContractTx << "\"";
				if (withFirstRedemption) {
					value << ", \"redemption_tx\": \"" << firstRedemption << "\"";
				}
				value << "}";
				if (withSecond) {
					value << ", ";
				}
			}
			if (withSecond) {
				value << "\"second\": {\"contract\": \"" << secondContract << "\", \"contract_tx\": \"" << secondContractTx << "\"";
				if (withSecondRedemption) {
				value << ", \"redemption_tx\": \"" << secondRedemption << "\"";
				}
				value << "}";
			}
			value << "}";

			command << "SET " << request->swapKey_ << " " << value.str();
		}
		redisAsyncCommand(request->redisContext_, nullptr, nullptr, command.str().c_str());
	} 
	request->sendSuccess();
	if (request->onComplete_) {
		request->onComplete_(request);
	}
}