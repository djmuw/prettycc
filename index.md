---
title: prettycpp
layout: post
---

# 0. Preface and wording

A **'decoration'** in terms of this project is a set of prefix-string, delimiter-string and a postfix-string.
Where the prefix string is inserted into a stream before and the postfix string after the values of a container (see 2. Target containers).
The delimiter string is inserted between the values of the respective container.

*Note: Actually, this answer does not address the question to 100% since the decoration is not strictly compile time constant because runtime checks are required to check whether a custom decoration has been applied to the current stream.*
Nevertheless, I think it has some decent features.

*Note2: May have minor bugs since it is not yet well tested.*

# 1. General idea / usage

### Zero additional code required for usage

It is to be kept as easy as

    #include <vector>
    #include "pretty.h"

    int main()
    {
      std::cout << std::vector<int>{1,2,3,4,5}; // prints 1, 2, 3, 4, 5
      return 0;
    }

### Easy customization ...

... with respect to specific stream object

    #include <vector>
    #include "pretty.h"

    int main()
    {
      // set decoration for std::vector<int> for cout object
      std::cout << pretty::decoration<std::vector<int>>("(", ",", ")");
      std::cout << std::vector<int>{1,2,3,4,5}; // prints (1,2,3,4,5)
      return 0;
    }

or with respect to all streams:

    #include <vector>
    #include "pretty.h"

    // set decoration for std::vector<int> for all ostream objects
    PRETTY_DEFAULT_DECORATION(std::vector<int>, "{", ", ", "}")

    int main()
    {
      std::cout << std::vector<int>{1,2,3,4,5}; // prints {1, 2, 3, 4, 5}
      std::cout << pretty::decoration<std::vector<int>>("(", ",", ")");
      std::cout << std::vector<int>{1,2,3,4,5}; // prints (1,2,3,4,5)
      return 0;
    }

### Rough description

- The code includes a class template providing a default decoration for any type
- which can be specialized to change the default decoration for (a) certain type(s) and it is
- using the private storage provided by `ios_base` using `xalloc`/`pword` in order to save a pointer to a `pretty::decor` object specifically decorating a certain type on a certain stream.

If no `pretty::decor<T>` object for this stream has been set up explicitly `pretty::defaulted<T, charT, chartraitT>::decoration()` is called to obtain the default decoration for the given type.
The class `pretty::defaulted` is to be specialized to customize default decorations.
 
# 2. Target objects / containers

Target objects `obj` for the *'pretty decoration'* of this code are objects having either

- overloads `std::begin` and `std::end` defined (includes C-Style arrays),
- having `begin(obj)` and `end(obj)` available via ADL,
- are of type `std::tuple` 
- or of type `std::pair`.

The code includes a trait for identification of classes with range features (`begin`/`end`).
(There's no check included, whether `begin(obj) == end(obj)` is a valid expression, though.)
 
The code provides `operator<<`s in the global namespace that only apply for classes not having a more specialized version of `operator<<` available.
Therefore, in example `std::string` is not printed using the operator in this code although having a valid `begin`/`end` pair.

# 3. Utilization and customization

Decorations can be imposed seperately for every type (except different `tuple`s) and stream (not stream type!).
(I.e. a `std::vector<int>` can have a different decorations for different stream objects.)

## A) Default decoration

The default prefix is `""` (nothing) as is the default postfix, while the default delimiter is `", "` (comma+space).

## B) Customized default decoration of a type by specializing the `pretty::defaulted` class template

The `struct defaulted` has a static member function `decoration()` returning a `decor` object which includes the default values for the given type.

### Example using array:

Cutomize default array printing:

    namespace pretty
    {
      template<class T, std::size_t N>
      struct defaulted<T[N]>
      {
        static decor<T[N]> decoration()
        {
          return{ { "(" }, { ":" }, { ")" } };
        }
      };
    }

Print an arry array:

    float e[5] = { 3.4f, 4.3f, 5.2f, 1.1f, 22.2f };
    std::cout << e << '\n'; // prints (3.4:4.3:5.2:1.1:22.2)

### Using the `PRETTY_DEFAULT_DECORATION(TYPE, PREFIX, DELIM, POSTFIX, ...)` macro for `char` streams

The macro expands to

    namespace pretty { 
      template< __VA_ARGS__ >
      struct defaulted< TYPE > {
        static decor< TYPE > decoration() {
          return { PREFIX, DELIM, POSTFIX };
        } 
      }; 
    } 

enabling the above partial specialization to be rewritten to

    PRETTY_DEFAULT_DECORATION(T[N], "", ";", "", class T, std::size_t N)

or inserting a full specialization like

    PRETTY_DEFAULT_DECORATION(std::vector<int>, "(", ", ", ")")

Another macro for `wchar_t` streams is included: `PRETTY_DEFAULT_WDECORATION`.

## C) Impose decoration on streams

The function `pretty::decoration` is used to impose a decoration on a certain stream.
There are overloads taking either 
- one string argument being the delimiter (adopting prefix and postfix from the defaulted class)
- or three string arguments assembling the complete decoration

### Complete decoration for given type and stream

    float e[3] = { 3.4f, 4.3f, 5.2f };
    std::stringstream u;
    // add { ; } decoration to u
    u << pretty::decoration<float[3]>("{", "; ", "}");
 
    // use { ; } decoration
    u << e << '\n'; // prints {3.4; 4.3; 5.2}

    // uses decoration returned by defaulted<float[3]>::decoration()
    std::cout << e; // prints 3.4, 4.3, 5.2

### Customization of delimiter for given stream

    PRETTY_DEFAULT_DECORATION(float[3], "{{{", ",", "}}}")

    std::stringstream v;
    v << e; // prints {{{3.4,4.3,5.2}}}

    v << pretty::decoration<float[3]>(":");
    v << e; // prints {{{3.4:4.3:5.2}}}

    v << pretty::decoration<float[3]>("((", "=", "))");
    v << e; // prints ((3.4=4.3=5.2))


# 4. Special handling of `std::tuple`

Instead of allowing a specialization for every possible tuple type, this code applies any decoration available for `std::tuple<void*>` to all kind of `std::tuple<...>`s.

# 5. Remove custom decoration from stream

To go back to the defaulted decoration for a given type use `pretty::clear` function template on the stream `s`.

    s << pretty::clear<std::vector<int>>();

# 5. Further examples

Printing "matrix-like" with newline delimiter

    std::vector<std::vector<int>> m{ {1,2,3}, {4,5,6}, {7,8,9} };
    std::cout << pretty::decoration<std::vector<std::vector<int>>>("\n");
    std::cout << m;

Prints

    1, 2, 3
    4, 5, 6
    7, 8, 9
