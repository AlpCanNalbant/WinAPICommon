// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WCmn::Modules
{
    template <std::derived_from<Modules::IModule> T>
    T *Module<T>::operator->() const
    {
        static const std::unique_ptr<T> GenericModule = std::make_unique<T>();
        return GenericModule.get();
    }
}
