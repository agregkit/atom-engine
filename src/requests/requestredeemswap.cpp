#include "requestredeemswap.h"

#include <sstream>

CRequestRedeemSwap::CRequestRedeemSwap(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	CRequest(redisContext, req, std::move(document), onComplete)
{

}

CRequestRedeemSwap::~CRequestRedeemSwap()
{

}

void CRequestRedeemSwap::process()
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

		if (document_->HasMember("first_redemption_tx")) {
			redemptionTx_ = (*document_)["first_redemption_tx"].GetString();

			std::stringstream command;
			command << "GET " << swapKey_;
			redisAsyncCommand(redisContext_, &CRequestRedeemSwap::getFirstCallback, this, command.str().c_str());
		} else if (document_->HasMember("second_redemption_tx")) {
			redemptionTx_ = (*document_)["second_redemption_tx"].GetString();

			std::stringstream command;
			command << "GET " << swapKey_;
			redisAsyncCommand(redisContext_, &CRequestRedeemSwap::getSecondCallback, this, command.str().c_str());
		} else {
			sendBadJSONError();
			if (onComplete_) {
				onComplete_(this);
			}
		}
	}
}

std::string CRequestRedeemSwap::getType()
{
	return "redeem_swap";
}

void CRequestRedeemSwap::getFirstCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestRedeemSwap* request = reinterpret_cast<CRequestRedeemSwap*>(data);
	if (value) {
		request->setRedemption(value->str, "first", request->redemptionTx_);
	} else {
		request->sendSuccess();
		if (request->onComplete_) {
			request->onComplete_(request);
		}
	}
}

void CRequestRedeemSwap::getSecondCallback(redisAsyncContext* context, void* res, void* data)
{
	redisReply* value = reinterpret_cast<redisReply*>(res);
	CRequestRedeemSwap* request = reinterpret_cast<CRequestRedeemSwap*>(data);
	if (value) {
		request->setRedemption(value->str, "second", request->redemptionTx_);
	} else {
		request->sendSuccess();
		if (request->onComplete_) {
			request->onComplete_(request);
		}
	}
}

void CRequestRedeemSwap::setRedemption(const std::string& info, const std::string& key, const std::string& redemptionTx)
{
	rapidjson::Document document;
	if (!document.Parse(info.c_str()).HasParseError() && document.HasMember("secret_hash") && ((document.HasMember("first") && key == "first") || (document.HasMember("second") && key == "second"))) {
		std::string secretHash = document["secret_hash"].GetString();
		std::string firstContract;
		std::string secondContract;
		std::string firstContractTx;
		std::string secondContractTx;

		bool withFirst = false;
		bool withSecond = false;
		if (document.HasMember("first")) {
			withFirst = true;
			firstContract = document["first"]["contract"].GetString();
			firstContractTx = document["first"]["contract_tx"].GetString();
		}
		if (document.HasMember("second")) {
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

		std::stringstream value;
		value << "{\"secret_hash\": \"" << secretHash << "\", \"";
		if (withFirst) {
			value << "first\": {\"contract\": \"" << firstContract << "\", \"contract_tx\": \"" << firstContractTx << "\"";
			if (key == "first") {
				value << ", \"redemption_tx\": \"" << redemptionTx << "\"";
			} else if (withFirstRedemption) {
				value << ", \"redemption_tx\": \"" << firstRedemption << "\"";
			}
			value << "}";
			if (withSecond) {
				value << ", ";
			}
		}
		if (withSecond) {
			value << "\"second\": {\"contract\": \"" << secondContract << "\", \"contract_tx\": \"" << secondContractTx << "\"";
			if (key == "second") {
				value << ", \"redemption_tx\": \"" << redemptionTx << "\"";
			} else if (withSecondRedemption) {
				value << ", \"redemption_tx\": \"" << secondRedemption << "\"";
			}
			value << "}";
		}
		value << "}";

		std::stringstream command;
		command << "SET " << swapKey_ << " " << value.str();
		redisAsyncCommand(redisContext_, nullptr, nullptr, command.str().c_str());
	} 

	sendSuccess();
	if (onComplete_) {
		onComplete_(this);
	}
}