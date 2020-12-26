//
// Created by liao xiangsen on 12/23/20.
//

#include <frida-gum.h>
#include <gumpp.hpp>

#include "agent/interceptor_handler.h"
#include "agent/utils.h"
#include "base/utils.h"
#include "binder/types.h"
#include "binder/driver_types.h"

TRACKER_AGENT_USING
const char *InterceptorHandler::LOG_TAG = "tracker.agent[" STRINGIFY(ARCH_ABI) "].interceptor";
std::mutex InterceptorHandler::interceptLock;
std::map<uintptr_t, Gum::InvocationListener*> InterceptorHandler::activeListener;

void InterceptorHandler::LoggingIoctl::on_enter(Gum::InvocationContext *context) {
    unsigned cmd = reinterpret_cast<uintptr_t>(context->get_nth_argument_ptr(1));
    if (cmd != binder::BINDER_WRITE_READ) {
        this->skip = true;
        return;
    }

    this->skip = false;
    this->customDataPtr = context->get_nth_argument_ptr(2);
    auto dataPtr = reinterpret_cast<binder::binder_write_read*>(this->customDataPtr);

    if (dataPtr->write_size > dataPtr->write_consumed && dataPtr->write_buffer != 0) {
        auto protocol = *(reinterpret_cast<unsigned*>(dataPtr->write_buffer));
        if (protocol != binder::BC_TRANSACTION) {
            return;
        }

        binder::BinderRPCContext rpcContext(dataPtr);
        LOG_INFO("ENTER ioctl({ client_pid: %d, interface: %s, method: %u })",
                 rpcContext.clientPID, rpcContext.interface.c_str(), rpcContext.methodID);
    }
}

void InterceptorHandler::LoggingIoctl::on_leave(Gum::InvocationContext *context) {
//    int ret = reinterpret_cast<uintptr_t>(context->get_nth_argument_ptr(0));
//    auto data = reinterpret_cast<int*>(this->customDataPtr);
//    LOG_INFO("LEAVE ioctl(0, 0, 0) = %d", ret);
}

void InterceptorHandler::handle(const Client &client, const char *msg, size_t size) {
    LOG_INFO("intercept handle begin");
    HackAction action;
    switch (parseHackAction(action, msg, size)) {
        case AttachInterceptor: {
            const char *moduleName = action.data["moduleName"].GetString();
            const char *funcSymbol = action.data["funcSymbol"].GetString();
            gpointer funcAddress = GUINT_TO_POINTER(gum_module_find_export_by_name(moduleName, funcSymbol));
            if (funcAddress == nullptr) {
                LOG_ERROR("Function(%s) not exist in %s", funcSymbol, moduleName);
                return;
            }

            if (activeListener.find(uintptr_t(funcAddress)) != activeListener.end()) {
                LOG_ERROR("Function(%p) has already been attached", funcAddress);
                return;
            }

            auto lock = std::unique_lock<std::mutex>(interceptLock, std::defer_lock);
            LOG_INFO("handle AttachInterceptor begin, intercept function(%p)", funcAddress);
            auto interceptor = agent::fridaContext->interceptor;
            interceptor->begin_transaction();
            auto realInterceptor = new InterceptorHandler::LoggingIoctl;
            interceptor->attach(funcAddress, realInterceptor, nullptr);
            interceptor->end_transaction();
            activeListener.emplace(uintptr_t(funcAddress), realInterceptor);
        } break;
        case QueryInterceptor: {

        } break;
        default: return;
    }
}


