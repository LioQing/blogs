# Functional Template C++ Programming

2025-12-04

---

C++ is one of my first programming languages and I used to write a lot of C++ code in my early days. As a result, I have a pretty good understanding of the language and its features, including the complex yet powerful template system.

Since [C++ template metaprogramming is turing-complete](https://en.cppreference.com/w/cpp/language/template_metaprogramming.html) and C++ is basically the complete opposite of functional programming (before the addition of [ranges library](https://en.cppreference.com/w/cpp/ranges.html)), so I got the idea of simulating functional programming with C++ template metaprogramming when I was studying a functional programming course in the spring semester of 2023.

And that, is exactly my topic today. Let's go through my journey of rediscovering C++ template as a functional programming language, and take a look at how I built an arithmetic expression evaluator in it.

> [!NOTE]
>
> The C++ code in this blog is compiled with MSVC and GCC using C++26 latest features.
>
> MSVC compiler is at version `Microsoft (R) C/C++ Optimizing Compiler Version 19.44.35217 for x86` and compiled with `cl main.cpp /std:c++latest`.
>
> GCC is at version `g++-14 (Ubuntu 14.2.0-4ubuntu2~24.04) 14.2.0` and compiled with `g++-14 main.cpp -o main -std=c++2c`.
>
> The Haskell code in this blog is run with GHCi at version `GHCi, version 9.6.7`.

## C++ Template

Before diving into it, let's get a basic overview of C++ template metaprogramming.

### Templates are just smarter macros

C++ templates are basically macros, but more powerful than C macros. They can be used to generate structs, classes, or functions for it's generic arguments as analyzed by the compiler in compile-time.

```cpp
template <typename T> auto addition(T a, T b) -> T {
    return a + b;
}

template <typename T> struct MyGenericStruct {
    T my_data_of_type_t;
};

auto my_struct = MyGenericStruct<int> { .my_data_of_type_t = 1 };
int result = addition(my_struct.my_data_of_type_t, 2);

std::println("{} of type {}", result, typeid(result).name());
// 3 of type int
```

In the example above, because we instnatiated `MyGenericStruct` with an `int` as the generic argument, the compiler generates a `MyGenericStruct<int>` type. Similarly, because `addition` is supplied with `int` arguments, the compiler was able to infer the generic argument to be `int`, and thus generate an `addition<int>` function.

This is very useful for defining custom wrapper data types like `std::vector` which stores a collection of element of its generic argument.

### Specialization helps hide implementation details

Specialization is one of the features that makes templates so powerful.

With specialization, the programmer defining the struct or function can use a different definition for specific generic argument.

For example, with a `Vec4` struct template with a generic argument of type `T`.

```cpp
template <typename T> struct Vec4 {
    Vec4(T x, T y, T z, T w) : data{x, y, z, w}
    {}

    auto get(std::size_t i) -> T {
        return data[i];
    }

private:
    T data[4];
};

auto vec4f = Vec4<float>(1.0f, 2.0f, 3.0f, 4.0f);
std::println("size_of(vec4f) = {}", sizeof(vec4f));
// size_of(vec4f) = 16
```

We can then define a different implementation by defining a `Vec4<bool>` specialization.

```cpp
template <> struct Vec4<bool> {
    Vec4(bool x, bool y, bool z, bool w)
        : data((std::uint8_t)x | (y << 1) | (z << 2) | (w << 3))
    {}

    auto get(std::size_t i) -> bool {
        return (data >> i) & 1;
    }

private:
    std::uint8_t data;
};

auto vec4b = Vec4<bool>(true, false, true, false);
std::println("size_of(vec4b) = {}", sizeof(vec4b));
// size_of(vec4b) = 1
```

This way, we can guarantee `Vec4<bool>` takes up only 8 bits. Where as if we did not defina a specialization for it, `Vec4<bool>` may take up 4 times 8 - 32 bits.

This is because many compiler implementation uses 8 bits (1 byte) for `bool` for alignment purposes (and [C++ standard doesn't specify the size of `bool`](https://en.cppreference.com/w/cpp/language/types.html#Boolean_type)).

## Specialization as Pattern Matching

Because of the existence of template specialization, there must exist some kind of match and resolve mechanism in the compiler to find the specialization, which is the specialization matching system.

This mechanism is very similar to pattern matching, where in functional programming languages like Haskell, we can match arguments against specific constant values or data constructors, called patterns, to return a different value.

### Using specialization matching as pattern matching

Since it is very common to use pattern matching to do branching in Haskell, let's take a look at a simple example.

Take a fibonacci number function as an example. In Haskell, we would usually match against 0 to return 0, 1 to return 1, and the other values to the recursive calls.

```haskell
fib :: Int -> Int
fib 0 = 0
fib 1 = 1
fib n = fib (n - 1) + fib (n - 2)

-- >>> fib 7
-- 13
```

And given that C++ template also allows certain constant values to be the generic arguments, we can do the same in C++, which some of you may have seen it.

```cpp
template <int N> auto fib() -> int {
    return fib<N - 1>() + fib<N - 2>();
}

template <> auto fib<1>() -> int {
    return 1;
}

template <> auto fib<0>() -> int {
    return 0;
}

std::println("fib<7>() = {}", fib<7>());
// fib<7>() = 13
```

Of course, one downside of this approach is that the argument `N` must be a compile-time constant.

### How about destructuring?

One key feature of pattern matching in Haskell is the ability to destructure components while matching the arguments.

For example, if we have a data constructor `Vec2` with 2 arguments, or components, we can match against it to destructure it, in other words to bind its components to variables in the function. Here we bind the first and second component of `Vec2` to `x` and `y` respectively, then we can use it to calculate the length squared.

```haskell
data Vec2 = Vec2 Int Int

len :: Vec2 -> Int
len (Vec2 x y) = x * x + y * y

-- >>> len (Vec2 3 4)
-- 25
```

Now let's try doing that in C++.

```cpp
template <int X, int Y> struct Vec2 {};

template <typename T> auto len_sq() -> int {
    static_assert(false, "Unsupported type");
}

template <int X, int Y> auto len_sq<Vec2<X, Y>>() -> int {
    return X * X + Y * Y;
}
```

Oops, we ge the following errors!

```
MSVC: error C2768: 'len_sq': illegal use of explicit template arguments
GCC: error: non-class, non-variable partial specialization len_sq<Vec2<X, Y> >’
            is not allowed
```

Turns out, partial specialization is not allowed on functions in C++ standard.

### Using partial struct/class template specialization for destructuring

Well, GCC's error message gave us a hint on how we might solve this issue - use it on a class!

But how are we going to define a function body and return value in a class? Since we are doing everything in compile-time, the most logical solution is probably to use a static constexpr.

```cpp
template <int X, int Y> struct Vec2 {};

template <typename T> struct len_sq;

template <int X, int Y> struct len_sq<Vec2<X, Y>> {
    static constexpr int value = X * X + Y * Y;
};

std::println("len_sq<Vec2<3, 4>>::value = {}", len_sq<Vec2<3, 4>>::value);
// len_sq<Vec2<3, 4>>::value = 25
```

And it works!

## Struct/Class as Value

Now that we can pass arguments into and return values out of a template struct/class to treat it like a function, how do we construct and pass around more complex data structures like lists? After all, generic arguments can only be primitive types like `int`, or `typename`.

In fact, we have been doing it in the previous sections! `Vec2` was basically holding the components as its member variables, and we were accessing its member variables by destructuring it. Although it is not a named member variable, it should be good enough.

### Returning struct values

Okay, so we have figured out how to pass more complex data structures around, how do we return it?

For example, what if we want to extend `Vec2` to a 3 components data structure `Vec3`? Here is how we would do it in Haskell.

```haskell
data Vec3 = Vec3 Int Int Int
    deriving Show

ext :: Vec2 -> Int -> Vec3
ext (Vec2 x y) z = Vec3 x y z

-- >>> ext (Vec2 1 2) 3
-- Vec3 1 2 3
```

In essence, we want to pass in a data structure, and get a data structure in return.

We can leverage the [`using` alias](https://en.cppreference.com/w/cpp/language/namespace.html) to let callers access the return value via type alias.

```cpp
template <int X, int Y, int Z> struct Vec3 {};

template <typename T, int Z> struct ext;

template <int X, int Y, int Z> struct ext<Vec2<X, Y>, Z> {
    using value = Vec3<X, Y, Z>;
};
```

The caller just need to "call" the `ext` "function" using the syntax `ext<Vec2<1, 2>, 3>::value`.

As an important note, we should use `using value = typename T::value;` if the return value is the return value of another function call. This is required because of the way C++ parsers are defined needs to be able to know ahead of time that the alias is a type.

### Outputting struct values

Hmm... let's try to print it out.

```cpp
std::println("ext<Vec2<1, 2>, 3>::value = {}", typeid(ext<Vec2<1, 2>, 3>::value).name());
// MSVC: ext<Vec2<1, 2>, 3>::value = struct Vec3<1,2,3>
// GCC: ext<Vec2<1, 2>, 3>::value = 4Vec3ILi1ELi2ELi3EE
```

The MSVC output looks fine, but look at the GCC output! There is no way that is readable.

Let's try to define some way to output a struct in a more standardized way.

```cpp
template <typename T> struct show {};

template <int X, int Y, int Z> struct show<Vec3<X, Y, Z>> {
    static auto value() -> std::string {
        return std::format("Vec3<{}, {}, {}>", X, Y, Z);
    }
};

std::println("ext<Vec2<1, 2>, 3>::value = {}", show<ext<Vec2<1, 2>, 3>::value>::value());
// ext<Vec2<1, 2>, 3>::value = Vec3<1, 2, 3>
```

And this is much better!

> [!NOTE]
>
> Input/output are runtime operations, there is no way to put that into compile time without any external tools.
>
> This is why we can and should use a normal function to convert it into strings. It makes it easier to implement too because we can just use `std::format`.

### Building a list

With what we have, we can build one of the fundamental data structures in computer science, a list!

In functional programming, it is very common to use [cons](https://en.wikipedia.org/wiki/Cons) and nil to construct lists. Let's see how they are defined in Haskell.

```haskell
data Cons a = Cons a (Cons a) | Nil
    deriving Show

-- >>> Cons 'f' (Cons 'o' (Cons 'o' Nil))
-- Cons 'f' (Cons 'o' (Cons 'o' Nil))
```

While in Haskell we had to use `a` to represent a generic type, in C++ template we don't have to. This is because C++ template is not "typed", as long as it is a typename, we can pass it in.

```cpp
template <typename X, typename Xs> struct Cons {};
struct Nil {};
```

You may have noticed, this way we cannot pass in primitive types like `int` or `char`, since they are not `typename`s.

But like what Java does with wrapper primitive classes, we can make "wrapper primitive struct templates" so that primitive types can be passed into `typename` parameters.

```cpp
template <char C> struct Char {};
template <int N> struct Int {};
```

Now we can construct and print a list!

```cpp
using Foo = Cons<Char<'f'>, Cons<Char<'o'>, Cons<Char<'o'>, Nil>>>;
std::println("Foo = {}", show<Foo>::value());
// Foo = Cons<'f', Cons<'o', Cons<'o', Nil>>>
```

### Reading a string

While like C, we can represent string as a list of chars, it is not convenient to specify characters one by one in `Cons`.

If we want to do that, we can make use of the fact that `const char*` can be passed as template arguments.

```cpp
template <const char* Cs> struct Str {};
```

Next, we will need a `StrView` which resembles C++ standard library's `std::string_view` where we will track the string itself, and the start and the end of the view.

```cpp
template <typename S, std::size_t I, std::size_t N> struct StrView {};
```

This lets us iterate through the string in a recursive manner. For example, we can use it to convert a `StrView` to a `Cons`.

```cpp
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
```

This way, we can create a `Cons` from `StrView`.

> [!NOTE]
>
> The reason why we cannot do a C-style iteration over the string by incrementing `const char*` and reading the first character until `'\0'` is because pointer arithmetic operations are not actually executed in compile-time.
>
> If we try the following program:
>
> ```cpp
> static constexpr const char inp1[] = "123";
> static constexpr const char* inp2 = inp + 1;
> using InputStr = Str<inp2>;
> ```
>
> We would get the following errors when compiling the program.
>
> ```
> MSVC: error C2371: 'main::InputStr': redefinition; different basic types
> GCC: error: ‘(((const char*)(& inp)) + 1)’ is not a valid template argument for ‘const char*’ because it is not the address of a variable
> ```

The next problem, is how do we convert a `Str` to `StrView`? We can actually make use of `constexpr` functions in this case, to calculate the length of the string at compile-time.

```cpp
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
```

And with this, we can convert any string literal into a list of chars!

```cpp
static constexpr char bar_cstr[] = "bar";
using BarStr = Str<bar_cstr>;
using BarCons = typename to_cons<BarStr>::value;
std::println("BarStr as Cons = {}", show<BarCons>::value());
// BarStr as Cons = Cons<'b', Cons<'a', Cons<'r', Nil>>>
```

## Higher-order functions

Another key feature of functional programming langauges is the ability to pass functions into higher-order functions. This enable some nice syntax for operating on wrappers like lists.

This is achievable through template-template parameters, something that I didn't know until I started working on this!

Take the classic [map](https://en.wikipedia.org/wiki/Map_(higher-order_function)) function as an example, which takes a function `f` and a list (or any iterable) `xs`, and applies `f` on each element `x` in `xs`, i.e. `xs = [x1, x2, x3, ...] => map(f, xs) = [f(x1), f(x2), f(x3), ...]`

```cpp
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
```

Now let's take a concrete example of adding 2 to every element in a list using the `map` function we just defined.

```cpp
template <typename X, typename Y> struct add {};

template <int X, int Y> struct add<Int<X>, Int<Y>> {
    using value = Int<X + Y>;
};

template <typename T> using add_two = add<T, Int<2>>;

using ConsOfInts = Cons<Int<2>, Cons<Int<3>, Cons<Int<5>, Nil>>>;
using ConsOfIntsPlusTwo = typename map<add_two, ConsOfInts>::value;
std::println("map<add_two, ConsOfInts>::value = {}", show<ConsOfIntsPlusTwo>::value());
// map<add_two, ConsOfInts>::value = Cons<4, Cons<5, Cons<7, Nil>>>
```

## Let's build a simple calculator!

With all these tools at our disposal, let's build something a bit more complicated - a simple calculator.

To keep things short, we will not handle any parentheses and any operator precedence. Everything will be evaluated from left to right, and on non-negative single digit integer values only, an example of such expression would be `1 + 2 * 3 / 4 - 5` and the answer would be -3.

### Starts with evaluating single operator

To get started, we can start small with a function that evaluates the result of a single operator's operation. We want to handle the 4 basic operators: +, -, *, /, and evaluate them on 2 integers.

```cpp
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
```

### We also need some helper functions

To be able to evaluate expressions from left to right with multiple operators, we will need to accumulate the result after each operation.

In functional programming, this would usually be done with a [fold](https://en.wikipedia.org/wiki/Fold_(higher-order_function)) function. Since we are evaluating from left to right, let's define a `foldl` function that accumulate results from left to right.

```cpp
template <typename Acc, template <typename, typename> typename F, typename Xs> struct foldl;

template <typename Acc, template <typename, typename> typename F, typename X, typename Xs>
struct foldl<Acc, F, Cons<X, Xs>> {
    using value = typename foldl<typename F<Acc, X>::value, F, Xs>::value;
};

template <typename Acc, template <typename, typename> typename F> struct foldl<Acc, F, Nil> {
    using value = Acc;
};
```

Nice, on top of that, we also need a way to convert characters to integers - a `to_digit` function. Since we are only operating on single digit integers, this is quite easy.

```cpp
template <typename C> struct to_digit {};

template <char C> struct to_digit<Char<C>> {
    using value = Int<C - '0'>;
};
```

Great! Now the final helper function - a `Select` function. While we can do branching via pattern matching, this will simplify branching for boolean conditions.

```cpp
template <typename Cond, typename Then, typename Else> struct select {};

template <typename Then, typename Else> struct select<std::true_type, Then, Else> {
    using value = Then;
};

template <typename Then, typename Else> struct select<std::false_type, Then, Else> {
    using value = Else;
};
```

So now, we can do branching like the following.

```cpp
typename select<std::bool_constant<42 == 6 * 7>, Char<'t'>, Char<'f'>>
```

> [!NOTE]
>
> Note that the name `select` is deliberately distinct from `if`. The bodies of an `if` statement is lazily evaluated, i.e. only evaluated after the condition is checked and branching has occured, while the "body" of our `select` function is evaluated before the condition is checked.
>
> To define the equivalent of an `if` statement for lazily evaluated branch bodies, we can define the following.
>
> ```cpp
> template <typename Cond, typename Then, typename Else> struct if_ {};
> 
> template <typename Then, typename Else> struct if_<std::true_type, Then, Else> {
>     using value = typename Then::value;
> };
> 
> template <typename Then, typename Else> struct if_<std::false_type, Then, Else> {
>     using value = typename Else::value;
> };
> ```
>
> Notice the `::value` in the return value, this means we don't need to write `::value` when passing in the arguments, but this also means we cannot pass in any value as an argument due to the way we construct values, i.e. constructing values like `Cons<Int<1>, Nil>` does not require and cannot use `::value`.
>
> To remove this limitation, we can define "constructor" functions for struct/class values.
>
> ```cpp
> template <typename X, typename Xs> struct Cons {
>     using value = Cons<X, Xs>;
> };
>
> struct Nil {
>     using value = Nil;
> };
> ```
>
> This way, we can use `if_` with values as the bodies.
>
> ```cpp
> if_<
>     std::bool_constant<42 != 6 * 7>,
>     Cons<typename Int<1>::value, typename Nil::value>,
>     Nil
> >
> ```

### The calculator

Finally let's build the calculator.

We will first build an `evaluator` function which determines whether to call `eval` or store the value or operator into the accumulator.

```cpp
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

    // ...
};
```

We would also need a `get_result` function to extract the reuslt, i.e. the integer, from the tuple of integer and operator.

```cpp
template <typename Input> struct Calculator {
    // ...

    template <typename R> struct get_result {};

    template <int Value, char Op> struct get_result<std::tuple<Int<Value>, Char<Op>>> {
        using value = Int<Value>;
    };

    // ...
};
```

Finally we chain everything together: from `Str` to `Cons`, from `Cons` to `Folded`, from `Folded` to `value`.

```cpp
template <typename Input> struct Calculator {
    // ...

    using Cons = typename to_cons<Input>::value;
    using Folded = typename foldl<
        std::tuple<Int<0>, Char<'+'>>,
        evaluator,
        Cons
    >::value;
    using value = typename get_result<Folded>::value;
};
```

Let's see the result!

```cpp
static constexpr char inp[] = "1 + 2 * 3 / 4 - 5";
using InputStr = Str<inp>;
using Result = typename Calculator<InputStr>::value;
std::println("Result of '{}' = {}", inp, show<Result>::value());
// Result of '1 + 2 * 3 / 4 - 5' = -3
```

### Is there more?

And on top of what I shared in this blog post, there is so much more that can be done. To push my limitation of what to build in a "language" like this, the ultimate program I built is [an arithmetic expression evaluator with parser combinators and variable substituion](https://github.com/LioQing/pdwb/tree/main/ftcpp_calc).

An example usage extracted from the source code.

```cpp
// Inputs
constexpr char inp[] = "pi * r * r / 2 + (a + b) * h / 2";
constexpr char pi[] = "pi";
using InpStr = typename Literal<inp>::Eval;
using InpEnv = typename Env<
    Tuple<typename Literal<pi>::Eval, Int<3>>,
    typename Env<
        Tuple<typename String<Char<'r'>, Nil>::Eval, Int<2>>,
        typename Env<
            Tuple<typename String<Char<'a'>, Nil>::Eval, Int<1>>,
            typename Env<
                Tuple<typename String<Char<'b'>, Nil>::Eval, Int<5>>,
                typename Env<
                    Tuple<typename String<Char<'h'>, Nil>::Eval, Int<4>>,
                    Nil::Eval
                >::Eval
            >::Eval
        >::Eval
    >::Eval
>::Eval;

// Outputs
using OutParse = typename parse<pExpr::Eval, InpStr>::Eval;
using OutEval = typename eval<
    InpEnv,
    typename fst<typename unwrap<OutParse>::Eval>::Eval
>::Eval;

// Main
int main() {
    std::cout << "  InpStr: " << show<InpStr>::to_string() << std::endl;
    std::cout << "  InpEnv: " << show<InpEnv>::to_string() << std::endl;

    std::cout << "OutParse: " << show<OutParse>::to_string() << std::endl;
    std::cout << " OutEval: " << show<OutEval>::to_string() << std::endl;

    return 0;
}
```

## Conclusion

This has been a topic I wanted to share for a long time, it is super fascinating to me how the C++ template system can be so flexible, and how it is basically the opposite of procedural programming.

This has been an interesting journey, and I hope you enjoyed reading it as much as I enjoyed writing it!

### Appendix

<details>
<summary>
Full C++ source codeo of everything mentioned in this blog post.
</summary>

```cpp
{{#include main.cpp}}
```
</details>

<details>
<summary>
Full Haskell source code of everything mentioned in this blog post.
</summary>

```haskell
{{#include main.hs}}
```
</details>