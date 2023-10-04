/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

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
