//
// Created by xinyang on 2020/2/11.
//

#ifndef _UILTS_HPP_
#define _UILTS_HPP_


namespace stf {
    template<typename T_>
    void eat_future(std::shared_future<T_> f) {};

    template<typename t>
    using GetRetureType = typename t::RetureType;

    template<typename Callable_, typename ...T_>
    class GetCurrTaskRetureType {
    public:
        using type = std::result_of_t<Callable_(GetRetureType<T_>...)>;
    };

    template<typename Callable_>
    class GetCurrTaskRetureType<Callable_> {
    public:
        using type = std::result_of_t<Callable_()>;
    };

    template<typename Callable_, typename Arg1st>
    class Bind1st {
    private:
        Callable_ func;
        Arg1st arg1st;
    public:
        Bind1st(Callable_ f, Arg1st arg) : func(f), arg1st(arg) {};

        template<typename ...Args>
        auto operator()(Args...args) {
            return func(arg1st, args...);
        }
    };
}


#endif /* _UILTS_HPP_ */
