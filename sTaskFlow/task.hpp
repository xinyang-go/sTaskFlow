//
// Created by xinyang on 2020/2/10.
//

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include <future>
#include "utils.hpp"

namespace stf {
    template<typename Callable, typename ...T>
    class TaskNode {
    public:
        using RetureType = typename GetCurrTaskRetureType<Callable, T...>::type;
    private:
        using PredecessorType = std::tuple<std::shared_ptr<T>...>;

        Callable func;
        PredecessorType predecessors;
        std::shared_future<RetureType> future;

        template<int idx, typename Callable_, typename T_>
        auto call_task(Callable_ func, T_ &futures_tuple) {
            if constexpr (std::is_same_v<decltype(std::get<idx>(futures_tuple).get()), void>) {
                std::get<idx>(futures_tuple).get();
                if constexpr (idx < std::tuple_size_v<T_> - 1) {
                    return call_task<idx + 1>(func, futures_tuple);
                } else {
                    return func();
                }
            } else {
                Bind1st f(func, std::get<idx>(futures_tuple).get());
                if constexpr (idx < std::tuple_size_v<T_> - 1) {
                    return call_task<idx + 1>(f, futures_tuple);
                } else {
                    return f();
                }
            }
        }

        template<int idx = 0>
        auto run_predecessors() {
            if constexpr (idx < std::tuple_size_v<PredecessorType>)
                return std::tuple_cat(std::make_tuple(std::get<idx>(predecessors)->run()), run_predecessors<idx + 1>());
            else
                return std::make_tuple();
        }

        template<int idx = 0>
        void reset_predecessors() {
            if constexpr (idx < std::tuple_size_v<PredecessorType>) {
                std::get<idx>(predecessors)->reset_task();
                reset_predecessors<idx + 1>();
            }
        }

    public:
        TaskNode(Callable f, std::shared_ptr<T> &... nodes) : func(f), predecessors(std::make_tuple(nodes...)) {};

        TaskNode(Callable f, std::tuple<std::shared_ptr<T>...> &nodes) : func(f), predecessors(nodes) {};

        void reset_task() {
            if (future.valid()) future.get();
            eat_future(std::move(future));
            reset_predecessors();
        }

        auto run() {
            if (future.valid()) return future;
            if constexpr (sizeof...(T) > 0) {
                auto futures = run_predecessors();
                std::packaged_task<RetureType()> task([=]() {
                    return call_task<0>(func, futures);
                });
                future = task.get_future();
                std::thread(std::move(task)).detach();
            } else {
                std::packaged_task<RetureType()> task(func);
                future = task.get_future();
                std::thread(std::move(task)).detach();
            }
            return future;
        }
    };

    template<typename Callable, typename ...T>
    auto make_task_node(Callable func, std::shared_ptr<T>... nodes) {
        return std::make_shared<TaskNode<Callable, T...>>(func, nodes...);
    }

    template<typename Callable, typename ...T>
    auto make_task_node(Callable func, std::tuple<std::shared_ptr<T>...> nodes) {
        return std::make_shared<TaskNode<Callable, T...>>(func, nodes);
    }
}


#endif /* _TASK_HPP_ */
