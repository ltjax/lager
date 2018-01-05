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

#include <catch.hpp>

#include <lager/store.hpp>
#include <lager/event_loop/manual.hpp>
#include <optional>

namespace {
    struct action {};
}

TEST_CASE("basic")
{
    auto viewed = std::optional<int>{std::nullopt};
    auto view   = [&] (auto model) { viewed = model; };
    auto store  = lager::make_store<action>(
        int{},
        [](auto model, auto action) {return model+1; },
        view,
        lager::with_manual_event_loop{});

    CHECK(viewed);
    CHECK(*viewed == 0);
    CHECK(store.current() == 0);

    store.dispatch(action{});
    CHECK(viewed);
    CHECK(*viewed == 1);
    CHECK(store.current() == 1);
}

TEST_CASE("effect as a result")
{
    auto called = 0;
    auto effect = [&] (lager::context<int> ctx) { ++called; };
    auto store  = lager::make_store<int>(
        0,
        [=] (int model, int action) {
            return std::make_pair(model + action, effect);
        },
        lager::noop,
        lager::with_manual_event_loop{});

    store.dispatch(2);
    CHECK(store.current() == 2);
    CHECK(called == 1);
}
