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

#include <lager/util.hpp>
#include <lager/context.hpp>

#include <type_traits>
#include <memory>

namespace lager {

template <typename Action,
          typename Model,
          typename ReducerFn,
          typename ViewFn,
          typename EventLoop>
struct store
{
    using action_t     = Action;
    using model_t      = Model;
    using reducer_t    = ReducerFn;
    using view_t       = ViewFn;
    using event_loop_t = EventLoop;
    using context_t    = context<Action>;

    store(model_t init,
          reducer_t reducer,
          view_t view,
          event_loop_t loop)
        : impl_{std::make_unique<impl>(
            std::move(init),
            std::move(reducer),
            std::move(view),
            std::move(loop))}
    {}

    void dispatch(action_t action)
    { impl_->dispatch(action); }

    const model_t& current() const
    { return impl_->model; }

    context_t get_context()
    { return impl_->context; }

private:
    struct impl
    {
        event_loop_t loop;
        model_t model;
        reducer_t reducer;
        view_t view;
        context_t context;

        impl(model_t init_,
             reducer_t reducer_,
             view_t view_,
             event_loop_t loop_)
            : loop{std::move(loop_)}
            , model{std::move(init_)}
            , reducer{std::move(reducer_)}
            , view{std::move(view_)}
            , context{[this] (auto ev) { dispatch(ev); },
                      [this] (auto fn) { loop.async(fn); },
                      [this] { loop.finish(); },
                      [this] { loop.pause(); },
                      [this] { loop.resume(); }}
        {
            loop.post([=] {
                view(model);
            });
        };

        void dispatch(action_t action)
        {
            loop.post([=] {
                this->model = invoke_reducer(reducer, model, action,
                                       [&] (auto&& effect) {
                                           LAGER_FWD(effect)(context);
                                       });
                view(this->model);
            });
        }
    };

    std::unique_ptr<impl> impl_;
};

template <typename Action,
          typename Model,
          typename ReducerFn,
          typename ViewFn,
          typename EventLoop,
          typename Enhancer>
auto make_store(Model&& init,
                ReducerFn&& reducer,
                ViewFn&& view,
                EventLoop&& loop,
                Enhancer&& enhancer)
{
    return store<Action, std::decay_t<Model>, std::decay_t<ReducerFn>, std::decay_t<ViewFn>, std::decay_t<EventLoop>>(
        std::forward<Model>(init),
        std::forward<ReducerFn>(reducer),
        std::forward<ViewFn>(view),
        std::forward<EventLoop>(loop));
}

template <typename Action,
          typename Model,
          typename ReducerFn,
          typename ViewFn,
          typename EventLoop>
auto make_store(Model&& init,
                ReducerFn&& reducer,
                ViewFn&& view,
                EventLoop&& loop)
{
    return make_store<Action>(
        std::forward<Model>(init),
        std::forward<ReducerFn>(reducer),
        std::forward<ViewFn>(view),
        std::forward<EventLoop>(loop),
        identity);
}

} // namespace lager
