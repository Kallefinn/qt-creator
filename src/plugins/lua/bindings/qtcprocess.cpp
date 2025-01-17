// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "../luaengine.h"

#include <utils/environment.h>
#include <utils/qtcprocess.h>

using namespace Utils;

namespace Lua::Internal {

void addProcessModule()
{
    LuaEngine::registerProvider(
        "Process", [](sol::state_view lua) -> sol::object {
            sol::table async = lua.script("return require('async')", "_process_").get<sol::table>();
            sol::function wrap = async["wrap"];

            sol::table process = lua.create_table();

            process["runInTerminal_cb"] = [](const QString &cmdline, const sol::function &cb) {
                Process *p = new Process;
                p->setTerminalMode(TerminalMode::Run);
                p->setCommand(CommandLine::fromUserInput((cmdline)));
                p->setEnvironment(Environment::systemEnvironment());

                QObject::connect(p, &Process::done, &LuaEngine::instance(), [p, cb]() {
                    cb(p->exitCode());
                });

                p->start();
            };

            process["runInTerminal"] = wrap(process["runInTerminal_cb"]);

            return process;
        });
}

} // namespace Lua::Internal
