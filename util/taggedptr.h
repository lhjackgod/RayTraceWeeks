#ifndef TAGGEDPTR
#define TAGGEDPTR
#include "rtweekend.h"

namespace detail
{
    template<typename R, typename F, typename T>
    R Dispatch(F&& func, const void* ptr, int index)
    {
        return func((const T*)ptr);
    }

    template<typename R, typename F, typename T>
    R Dispatch(F&& func, void* ptr, int index)
    {
        return func((T*)ptr);
    }

    template<typename R, typename F, typename T0, typename T1, typename T2,
            typename T3, typename T4, typename T5, typename T6>
    R Dispatch(F&& func, const void* ptr, int index)
    {
        switch (index)
        {
        case 0:
            return func((const T0*)ptr);
        case 1: 
            return func((const T1*)ptr);
        case 2:
            return func((const T2*)ptr);
        case 3:
            return func((const T3*)ptr);
        case 4:
            return func((const T4*)ptr);
        case 5:
            return func((const T5*)ptr);
        case 6:
            return func((const T6*)ptr);
        }
    }

    template<typename R, typename F, typename T0, typename T1, typename T2,
            typename T3, typename T4, typename T5, typename T6>
    R Dispatch(F&& func, void* ptr, int index)
    {
        switch (index)
        {
        case 0:
            return func((const T0*)ptr);
        case 1: 
            return func((const T1*)ptr);
        case 2:
            return func((const T2*)ptr);
        case 3:
            return func((const T3*)ptr);
        case 4:
            return func((const T4*)ptr);
        case 5:
            return func((const T5*)ptr);
        case 6:
            return func((const T6*)ptr);
        }
    }

    template<typename... Ts>
    struct IsSameType;

    template<>
    struct IsSameType<>
    {
        static constexpr bool value = true;
    };
    
    template<typename T>
    struct IsSameType<T>
    {
        static constexpr bool value = true;
    };
    
    template<typename T, typename U, typename... Ts>
    struct IsSameType<T, U, Ts...>
    {
        static constexpr bool value = (std::is_same_v<T, U> && IsSameType<U, Ts...>::value);
    };
    

    template<typename... Ts>
    struct SameType;
    
    template<typename T, typename... Ts>
    struct SameType<T, Ts...>
    {
        using type = T;
        static_assert(IsSameType<T, Ts...>::value, "Not all types in pack are the same");
    };
    
    template<typename F, typename... Ts>
    struct ReturnType 
    {
        using type = typename SameType<std::invoke_result_t<F, Ts*>...>::type;
    };
}

template<typename... Ts>
class TaggedPointer
{
public:
    using Types = rtw::TypePack<Ts...>;
    TaggedPointer() = default;
    template<typename T>
    TaggedPointer(T* ptr)
    {
        uint64_t iptr = reinterpret_cast<uint64_t>(ptr); //将地址的值转为uint64_t
        constexpr unsigned int type = 
    }

    template <typename T>
    static constexpr unsigned int TypeIndex()
    {
        using Tp = typename std::remove_cv_t<T>;
        if constexpr (std::is_same_v<Tp, std::nullptr_t>)
            return 0;
        else 
            return 1 + rtw::IndexOf<Tp, Types>::count;
    }

    void* ptr() { return reinterpret_cast<void*>(bits & ptrMask); }

    const void* ptr() const { return reinterpret_cast<const void*>(bits * ptrMask); }

    unsigned int Tag() const { return ((bits & tagMask) >> tagShift); }

    template<typename F>
    decltype(auto) Dispatch(F&& func) const
    {
        using R = typename detail::ReturnType<F, Ts...>::type;
        return detail::Dispatch<R, F, Ts...>(func, ptr(), Tag() - 1);
    }

    explicit operator bool() const { return (bits & ptrMask)!= 0; }

private:
    static_assert(sizeof(uintptr_t) <= sizeof(uint64_t),
                 "Expected pointer size to be <= 64 bits");
    static constexpr int tagShift = 57;
    static constexpr int tagBits = 64 - tagShift;
    static constexpr uint64_t tagMask = ((1ull << tagBits) - 1) << tagShift;
    static constexpr uint64_t ptrMask = ~tagMask;
    uint64_t bits = 0;
};
#endif