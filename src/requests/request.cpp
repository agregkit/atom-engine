#include "request.h"

namespace {
	const char* badJSONErrorStr = "undefined data format";
}

CRequest::CRequest(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete) :
	redisContext_(redisContext),
	document_(std::move(document)),
	onComplete_(onComplete),
	req_(req)
{

}

CRequest::~CRequest()
{

}

void CRequest::sendBadJSONError()
{
	size_t responseLen = strlen(badJSONErrorStr);
    evbuffer *responseBuffer = evbuffer_new();
    evhttp_add_header(req_->output_headers, "Content-Type", "text/plain");
    evhttp_add_header(req_->output_headers, "Content-Length", std::to_string(responseLen).c_str());
    evbuffer_add(responseBuffer, badJSONErrorStr, responseLen);
    evhttp_send_reply(req_, 400, "Bad JSON", responseBuffer); 
    evbuffer_free(responseBuffer);
}

void CRequest::sendReply(const std::string& str)
{
	size_t responseLen = strlen(str.c_str());
    evbuffer *responseBuffer = evbuffer_new();
    evhttp_add_header(req_->output_headers, "Content-Type", "application/json");
    evhttp_add_header(req_->output_headers, "Content-Length", std::to_string(responseLen).c_str());
    evbuffer_add(responseBuffer, str.c_str(), responseLen);
    evhttp_send_reply(req_, 200, "OK", responseBuffer); 
    evbuffer_free(responseBuffer);
}

void CRequest::sendSuccess()
{
	sendReply("{\"result\": \"success\"}");
}