//
// Created by liao xiangsen on 11/17/20.
//

#include <sys/xattr.h>

#include <android/log.h>
#include "frida-core.h"

#include "tcp_client_server/server.h"
#include "injector/inject_agent_handler.h"
#include "injector/utils.h"
#include "base/utils.h"
#include "base/encoder.h"

TRACKER_INJECTOR_USING

const char *selinux_context = "u:object_r:frida_file:s0";

void InjectAgentHandler::handle(const Client &client, const char *msg, size_t size) {
    HackAction action;
    if (parseHackAction(action, msg, size) != InjectAgent) {
        return;
    }

    auto lock = std::unique_lock<std::mutex>(InjectAgentHandler::injectorLock, std::defer_lock);
    LOG_INFO("handle HackAction begin");

    int targetPid = action.data["pid"].GetInt();
    const char *agentPath = action.data["libPath"].GetString();
    const char *agentEntry = action.data["entryPoint"].GetString();
    const char *agentArgs = action.data["agentArgs"].GetString();

    frida_selinux_patch_policy();
    if (setxattr(agentPath, XATTR_NAME_SELINUX, selinux_context,
            strlen(selinux_context) + 1, 0) != 0) {
        LOG_ERROR("failed to set selinux permission");
        return;
    }

    FridaInjector* injector = frida_injector_new();
    LOG_INFO("try to inject agent(%s %s) into process %d", agentPath, agentArgs, targetPid);
    GError *error = nullptr;
    frida_injector_inject_library_file_sync(injector, targetPid,
            agentPath, agentEntry, agentArgs,
            nullptr, &error);
    if (error != nullptr) {
        LOG_ERROR("inject agent error: %s", error->message);
        g_clear_error(&error);
    } else {
        LOG_INFO("agent injected");
    }

    frida_injector_close_sync(injector, nullptr, nullptr);
    frida_unref(injector);

    const char *replyStr = basicReply(true);
    Server::sendToClient(client, replyStr, strlen(replyStr));
}
