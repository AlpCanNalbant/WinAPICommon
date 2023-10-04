/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

namespace WCmn::Modules
{
    template <std::derived_from<Modules::IModule> T>
    T *Module<T>::operator->() const
    {
        static const std::unique_ptr<T> GenericModule = std::make_unique<T>();
        return GenericModule.get();
    }
}
