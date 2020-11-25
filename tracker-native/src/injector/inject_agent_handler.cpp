//
// Created by liao xiangsen on 11/17/20.
//

#include <string>
#include <atomic>
#include <cstdlib>
#include <sys/xattr.h>

#include <android/log.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <frida-core.h>

#include "tcp_client_server/tcp_server.h"
#include "injector/inject_agent_handler.h"
#include "utils.h"

#define LOG_TAG "Injector.InjectAgent[" STRINGIFY(ARCH_ABI) "]"
#define  LOG_FAULT(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define  LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

const char *selinux_context = "u:object_r:frida_file:s0";

void InjectAgentHandler::handle(const Client & client, const char * msg, size_t size) {
//    InjectAgentHandler::mtx.lock();
    LOG_INFO("handle HackAction begin");
    HackAction action;
    if (!parseHackAction(action, msg, size) || action.command != InjectAgent) {
        return;
    }

    int targetPid = action.data["pid"].GetInt();
    const char *agentPath = action.data["agentPath"].GetString();
    const char *agentEntry = action.data["agentEntry"].GetString();
    int agentListenPort = action.data["agentListenPort"].GetInt();
    const char *agentArgs = std::to_string(agentListenPort).c_str();

    frida_init();
    frida_selinux_patch_policy();
    if (setxattr(agentPath, XATTR_NAME_SELINUX, selinux_context,
            strlen(selinux_context) + 1, 0) != 0) {
        LOG_ERROR("failed to set selinux permission\n");
        return;
    }

    FridaInjector* injector = frida_injector_new();
    LOG_INFO("try to inject agent(%s %s) into process %d\n", agentPath, agentArgs, targetPid);
    GError *error = nullptr;
    frida_injector_inject_library_file_sync(injector, targetPid,
            agentPath, agentEntry, agentArgs,
            nullptr, &error);
    if (error != nullptr) {
        LOG_ERROR("inject agent error: %s\n", error->message);
        g_clear_error(&error);
    } else {
        LOG_INFO("agent injected\n");
    }

    frida_injector_close_sync(injector, nullptr, nullptr);
    frida_unref(injector);
    frida_deinit();
//    std::string replyMsg = "server got this msg: " + std::string(action.data["agentPath"].GetString());
//    TcpServer::sendToClient(client, replyMsg.c_str(), replyMsg.size());
//    InjectAgentHandler::mtx.unlock();
}

