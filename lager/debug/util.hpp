//
// lager - library for functional interactive c++ programs
// Copyright (C) 2017 Juan Pedro Bolivar Puente
//
// This file is part of lager.
//
// lager is free software: you can redistribute it and/or modify
// it under the terms of the MIT License, as detailed in the LICENSE
// file located at the root of this source code distribution,
// or here: <https://github.com/arximboldi/lager/blob/master/LICENSE>
//

#pragma once

#include <lager/config.hpp>

namespace lager {

// copied from cppreference, in practice, use scelta::visit,
// atria::match, mpark::match, etc.
template <class... Ts> struct visitor : Ts... { using Ts::operator()...; };
template <class... Ts> visitor(Ts...) -> visitor<Ts...>;

inline const char* resources_path()
{
    auto env_resources_path = std::getenv("LAGER_RESOURCES_PATH");
    return env_resources_path
        ? env_resources_path
        : LAGER_PREFIX_PATH "/share/lager";
}

} // namespace lager
