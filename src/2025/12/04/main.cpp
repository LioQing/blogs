#include <print>
#include <cstdint>
#include <typeinfo>
#include <string>

// C++ Template

template <typename T> auto addition(T a, T b) -> T {
    return a + b;
}

template <typename T> struct MyGenericStruct {
    T my_data_of_type_t;
};

// Template Specialization

template <typename T> struct Vec4 {
    Vec4(T x, T y, T z, T w) : data{x, y, z, w} {}

    auto get(std::size_t i) -> T {
        return data[i];
    }

private:
    T data[4];
};

template <> struct Vec4<bool> {
    Vec4(bool x, bool y, bool z, bool w)
        : data((std::uint8_t)x | (y << 1) | (z << 2) | (w << 3)) {}

    auto get(std::size_t i) -> bool {
        return (data >> i) & 1;
    }

private:
    std::uint8_t data;
};

// Pattern Matching

template <int N> auto fib() -> int {
    return fib<N - 1>() + fib<N - 2>();
}

template <> auto fib<1>() -> int {
    return 1;
}

template <> auto fib<0>() -> int {
    return 0;
}

// Pattern Matching & Binding

// template <int X, int Y> struct Vec2 {};

// template <typename T> auto len_sq() -> int {
//     static_assert(false, "Unsupported type");
// }

// template <int X, int Y> auto len_sq<Vec2<X, Y>>() -> int {
//     return X * X + Y * Y;
// }

// MSVC: error C2768: 'len_sq': illegal use of explicit template arguments
// GCC: error: non-class, non-variable partial specialization len_sq<Vec2<X, Y> >’
//             is not allowed

// Pattern Matching & Binding

template <int X, int Y> struct Vec2 {};

template <typename T> struct len_sq;

template <int X, int Y> struct len_sq<Vec2<X, Y>> {
    static constexpr int value = X * X + Y * Y;
};

// Returning Struct Values

template <int X, int Y, int Z> struct Vec3 {};

template <typename T, int Z> struct ext;

template <int X, int Y, int Z> struct ext<Vec2<X, Y>, Z> {
    using value = Vec3<X, Y, Z>;
};

template <typename T> struct show {};

template <int X, int Y, int Z> struct show<Vec3<X, Y, Z>> {
    static auto value() -> std::string {
        return std::format("Vec3<{}, {}, {}>", X, Y, Z);
    }
};

// String & Cons

template <typename X, typename Xs> struct Cons {};
struct Nil {};

template <char C> struct Char {};
template <int N> struct Int {};

template <typename X, typename Xs> struct show<Cons<X, Xs>> {
    static auto value() -> std::string {
        return std::format(
            "Cons<{}, {}>",
            show<X>::value(),
            show<Xs>::value()
        );
    }
};

template <> struct show<Nil> {
    static auto value() -> std::string {
        return "Nil";
    }
};

template <char C> struct show<Char<C>> {
    static auto value() -> std::string {
        return std::format("'{}'", C);
    }
};

template <int N> struct show<Int<N>> {
    static auto value() -> std::string {
        return std::format("{}", N);
    }
};

// String & Cons

template <const char* Cs> struct Str {};

template <typename S, std::size_t I, std::size_t N> struct StrView {};

template <typename T> struct to_cons;

template <const char* S, std::size_t I, std::size_t N> struct to_cons<StrView<Str<S>, I, N>> {
    using value = Cons<
        Char<S[I]>,
        typename to_cons<StrView<Str<S>, I + 1, N>>::value
    >;
};

template <const char* S, std::size_t N> struct to_cons<StrView<Str<S>, N, N>> {
    using value = Nil;
};

template <const char* Cs> struct to_cons<Str<Cs>> {
    static constexpr std::size_t const_strlen(const char* str) {
        std::size_t len = 0;
        while (*str != '\0') {
            ++len;
            ++str;
        }
        return len;
    }

    using value = typename to_cons<StrView<
        Str<Cs>, 
        0,
        const_strlen(Cs)
    >>::value;
};

// Higher Order Functions

template <template <typename> typename F, typename Xs> struct map {};

template <template <typename> typename F, typename X, typename Xs>
struct map<F, Cons<X, Xs>> {
    using value = Cons<
        typename F<X>::value,
        typename map<F, Xs>::value
    >;
};

template <template <typename> typename F> struct map<F, Nil> {
    using value = Nil;
};

template <typename X, typename Y> struct add {};

template <int X, int Y> struct add<Int<X>, Int<Y>> {
    using value = Int<X + Y>;
};

template <typename T> using add_two = add<T, Int<2>>;

// Simple Calculator

template <typename L, typename O, typename R> struct eval;

template <int L, int R> struct eval<Int<L>, Char<'+'>, Int<R>> {
    using value = Int<L + R>;
};

template <int L, int R> struct eval<Int<L>, Char<'-'>, Int<R>> {
    using value = Int<L - R>;
};

template <int L, int R> struct eval<Int<L>, Char<'*'>, Int<R>> {
    using value = Int<L * R>;
};

template <int L, int R> struct eval<Int<L>, Char<'/'>, Int<R>> {
    using value = Int<L / R>;
};

template <typename Acc, template <typename, typename> typename F, typename Xs> struct foldl;

template <typename Acc, template <typename, typename> typename F, typename X, typename Xs>
struct foldl<Acc, F, Cons<X, Xs>> {
    using value = typename foldl<typename F<Acc, X>::value, F, Xs>::value;
};

template <typename Acc, template <typename, typename> typename F> struct foldl<Acc, F, Nil> {
    using value = Acc;
};

template <typename C> struct to_digit {};

template <char C> struct to_digit<Char<C>> {
    using value = Int<C - '0'>;
};

template <typename Cond, typename Then, typename Else> struct select {};

template <typename Then, typename Else> struct select<std::true_type, Then, Else> {
    using value = Then;
};

template <typename Then, typename Else> struct select<std::false_type, Then, Else> {
    using value = Else;
};

template <typename Input> struct Calculator {
    template <typename Acc, typename C> struct evaluator {};

    template <int Value, char Op, char C> struct evaluator<
        std::tuple<Int<Value>, Char<Op>>,
        Char<C>
    > {
        using value = typename select<
            std::bool_constant<
                C >= '0' && C <= '9' ||
                C == '+' || C == '-' || C == '*' || C == '/'
            >,
            typename select<
                std::bool_constant<C >= '0' && C <= '9'>,
                std::tuple<
                    typename eval<
                        Int<Value>,
                        Char<Op>,
                        typename to_digit<Char<C>>::value
                    >::value,
                    Char<'+'> // a placeholder for the next operator
                >,
                std::tuple<Int<Value>, Char<C>>
            >::value,
            std::tuple<Int<Value>, Char<Op>> // ignore invalid characters
        >::value;
    };

    template <typename R> struct get_result {};

    template <int Value, char Op> struct get_result<std::tuple<Int<Value>, Char<Op>>> {
        using value = Int<Value>;
    };
    
    using Cons = typename to_cons<Input>::value;
    using Folded = typename foldl<
        std::tuple<Int<0>, Char<'+'>>,
        evaluator,
        Cons
    >::value;
    using value = typename get_result<Folded>::value;
};

// Main

int main() {
auto my_struct = MyGenericStruct<int> { .my_data_of_type_t = 1 };
int result = addition(my_struct.my_data_of_type_t, 2);

std::println("{} of type {}", result, typeid(result).name());
// 3 of type int

auto vec4f = Vec4<float>(1.0f, 2.0f, 3.0f, 4.0f);
std::println("size_of(vec4f) = {}", sizeof(vec4f));
// size_of(vec4f) = 16

auto vec4b = Vec4<bool>(true, false, true, false);
std::println("size_of(vec4b) = {}", sizeof(vec4b));
// size_of(vec4b) = 1

std::println("fib<7>() = {}", fib<7>());
// fib<7>() = 13

std::println("len_sq<Vec2<3, 4>>::value = {}", len_sq<Vec2<3, 4>>::value);
// len_sq<Vec2<3, 4>>::value = 25

std::println("ext<Vec2<1, 2>, 3>::value = {}", show<ext<Vec2<1, 2>, 3>::value>::value());
// ext<Vec2<1, 2>, 3>::value = Vec3<1, 2, 3>

using Foo = Cons<Char<'f'>, Cons<Char<'o'>, Cons<Char<'o'>, Nil>>>;
std::println("Foo = {}", show<Foo>::value());
// Foo = Cons<'f', Cons<'o', Cons<'o', Nil>>>

static constexpr char bar_cstr[] = "bar";
using BarStr = Str<bar_cstr>;
using BarCons = typename to_cons<BarStr>::value;
std::println("BarStr as Cons = {}", show<BarCons>::value());
// BarStr as Cons = Cons<'b', Cons<'a', Cons<'r', Nil>>>

using ConsOfInts = Cons<Int<2>, Cons<Int<3>, Cons<Int<5>, Nil>>>;
using ConsOfIntsPlusTwo = typename map<add_two, ConsOfInts>::value;
std::println("map<add_two, ConsOfInts>::value = {}", show<ConsOfIntsPlusTwo>::value());
// map<add_two, ConsOfInts>::value = Cons<4, Cons<5, Cons<7, Nil>>>

static constexpr char inp[] = "1 + 2 * 3 / 4 - 5";
using InputStr = Str<inp>;
using Result = typename Calculator<InputStr>::value;
std::println("Result of '{}' = {}", inp, show<Result>::value());
// Result of '1 + 2 * 3 / 4 - 5' = -3

    return 0;
}
