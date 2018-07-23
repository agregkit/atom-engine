#include "swapserver.h"

#include "utils.h"
#include "requestsprocessor.h"
#include "requestsprocessormt.h"

#include <cstdint>
#include <iostream>
#include <evhttp.h>

CSwapServer::CSwapServer() :
    started_(false)
{

}

CSwapServer::~CSwapServer()
{

}

bool CSwapServer::run()
{
    settings_.loadFromFile("settings.ini");

    if (!event_init()) {
        LOG_ERROR << "Failed to init libevent." << std::endl;
        return false;
    }

    int workersCount = settings_.getValue<int>("SwapServer", "WorkersCount", 1);

    event_config *cfg = event_config_new();
    event_base* base = event_base_new_with_config(cfg);
    if (!base) {
        event_config_free(cfg);
        LOG_ERROR << "Creating event_base object failed" << std::endl;
        return false;
    }

    evhttp* http = evhttp_new(base);
    if (!http) {
        event_base_free(base);
        event_config_free(cfg);
        LOG_ERROR << "Creating http server failed" << std::endl;
        return false;
    }

    evhttp_set_cb(http, "/swap", &CSwapServer::onRequest, this);

    std::string SrvAddress = settings_.getString("SwapServer", "Ip", "127.0.0.1");
    std::uint16_t SrvPort = settings_.getValue<std::uint16_t>("SwapServer", "Port", 8080);

    evhttp_bound_socket* handle = evhttp_bind_socket_with_handle(http, SrvAddress.c_str(), SrvPort);
    if (!handle) {
        evhttp_free(http);
        event_base_free(base);
        event_config_free(cfg);
        LOG_ERROR << "Bind with IP = " << SrvAddress << " Port = " << SrvPort << " failed" << std::endl;
        return false;
    }

    started_ = true;
    if (workersCount > 1) {
        requestsProcessor_ = std::make_unique<CRequestsProcessorMT>(base, settings_);
    } else {
        requestsProcessor_ = std::make_unique<CRequestsProcessor>(base, settings_);
    }
    requestsProcessor_->start(workersCount);

    LOG_INFO << "Run at IP = " << SrvAddress << " Port = " << SrvPort << " Workers count = " << workersCount << std::endl;

    event_base_dispatch(base);

    requestsProcessor_->stop();

    evhttp_free(http);
    event_base_free(base);
    event_config_free(cfg);

    return true;
}

void CSwapServer::onRequest(evhttp_request *req, void *arg)
{
    CSwapServer* swapServer = reinterpret_cast<CSwapServer*>(arg);
    swapServer->requestsProcessor_->addRequest(req);
}