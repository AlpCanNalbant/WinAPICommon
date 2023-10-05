// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include "Module.hpp"
#include "Log.hpp"
#include "Path.hpp"
#include "String.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "KeySender.hpp"
#include "Registry.hpp"

namespace WCmn
{
    using PathModule = Modules::Module<Modules::Path>;
    using StringModule = Modules::Module<Modules::String>;
    using SystemModule = Modules::Module<Modules::System>;
    using TimeModule = Modules::Module<Modules::Time>;
    using WindowModule = Modules::Module<Modules::Window>;
    using KeySenderModule = Modules::Module<Modules::KeySender>;
    using RegistryModule = Modules::Module<Modules::Registry>;

    static const PathModule Path;
    static const StringModule String;
    static const SystemModule System;
    static const TimeModule Time;
    static const WindowModule Window;
    static const KeySenderModule KeySender;
    static const RegistryModule Registry;
}
