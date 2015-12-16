#ifndef pretty_print_0x57547_sa4884X_0_1_h_guard_
#define pretty_print_0x57547_sa4884X_0_1_h_guard_
#pragma once

/*

//    Copyright Daniel Weber 2010 - 2014.
//    Distributed under The MIT License.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://opensource.org/licenses/mit-license.php)
//    --
//    A pretty printing library for C++ types
//    #include "pretty.hpp" and start printing

*/



#include <string>
#include <iostream>
#include <type_traits>
#include <iterator>
#include <utility>

#define PRETTY_DEFAULT_DECORATION(TYPE, PREFIX, DELIM, POSTFIX, ...) \
        namespace pretty { template< __VA_ARGS__ >\
        struct defaulted< TYPE > {\
        static decor< TYPE > decoration(){\
          return { PREFIX, DELIM, POSTFIX };\
        } /*decoration*/ }; /*defaulted*/} /*pretty*/

#define PRETTY_DEFAULT_WDECORATION(TYPE, PREFIX, DELIM, POSTFIX, ...) \
        namespace pretty { template< __VA_ARGS__ >\
        struct defaulted< TYPE, wchar_t, std::char_traits<wchar_t> > {\
        static decor< TYPE, wchar_t, std::char_traits<wchar_t> > decoration(){\
          return { PREFIX, DELIM, POSTFIX };\
        } /*decoration*/ }; /*defaulted*/} /*pretty*/

namespace pretty
{

  namespace detail
  {
    // drag in begin and end overloads
    using std::begin;
    using std::end;
    // helper template
    template <int I> using _ol = std::integral_constant<int, I>*;
    // SFINAE check whether T is a range with begin/end
    template<class T>
    class is_range
    {
      // helper function declarations using expression sfinae
      template <class U, _ol<0> = nullptr>
      static std::false_type b(...);
      template <class U, _ol<1> = nullptr>
      static auto b(U &v) -> decltype(begin(v), std::true_type());
      template <class U, _ol<0> = nullptr>
      static std::false_type e(...);
      template <class U, _ol<1> = nullptr>
      static auto e(U &v) -> decltype(end(v), std::true_type());
      // return types
      using b_return = decltype(b<T>(std::declval<T&>()));
      using e_return = decltype(e<T>(std::declval<T&>()));
    public:
      static const bool value = b_return::value && e_return::value;
    };
  }

  // holder class for data
  template<class T, class CharT = char, class TraitT = std::char_traits<CharT>>
  struct decor
  {
    static const int xindex;
    std::basic_string<CharT, TraitT> prefix, delimiter, postfix;
    decor(std::basic_string<CharT, TraitT> const & pre = "",
      std::basic_string<CharT, TraitT> const & delim = "",
      std::basic_string<CharT, TraitT> const & post = "")
      : prefix(pre), delimiter(delim), postfix(post) {}
  };

  template<class T, class charT, class traits>
  int const decor<T, charT, traits>::xindex = std::ios_base::xalloc();

  namespace detail
  {

    template<class T, class CharT, class TraitT>
    void manage_decor(std::ios_base::event evt, std::ios_base &s, int const idx)
    {
      using deco_type = decor<T, CharT, TraitT>;
      if (evt == std::ios_base::erase_event)
      { // erase deco
        void const * const p = s.pword(idx);
        if (p)
        {
          delete static_cast<deco_type const * const>(p);
          s.pword(idx) = nullptr;
        }
      }
      else if (evt == std::ios_base::copyfmt_event)
      { // copy deco
        void const * const p = s.pword(idx);
        if (p)
        {
          auto np = new deco_type{ *static_cast<deco_type const * const>(p) };
          s.pword(idx) = static_cast<void*>(np);
        }
      }
    }

    template<class T> struct clearer {};

    template<class T, class CharT, class TraitT>
    std::basic_ostream<CharT, TraitT>& operator<< (
      std::basic_ostream<CharT, TraitT> &s, clearer<T> const &)
    {
      using deco_type = decor<T, CharT, TraitT>;
      void const * const p = s.pword(deco_type::xindex);
      if (p)
      { // delete if set
        delete static_cast<deco_type const *>(p);
        s.pword(deco_type::xindex) = nullptr;
      }
      return s;
    }

    template <class CharT>
    struct default_data { static const CharT * decor[3]; };
    template <>
    const char * default_data<char>::decor[3] = { "", ", ", "" };
    template <>
    const wchar_t * default_data<wchar_t>::decor[3] = { L"", L", ", L"" };

  }

  // Clear decoration for T
  template<class T>
  detail::clearer<T> clear() { return{}; }
  template<class T, class CharT, class TraitT>
  void clear(std::basic_ostream<CharT, TraitT> &s) { s << detail::clearer<T>{}; }

  // impose decoration on ostream
  template<class T, class CharT, class TraitT>
  std::basic_ostream<CharT, TraitT>& operator<<(
    std::basic_ostream<CharT, TraitT> &s, decor<T, CharT, TraitT> && h)
  {
    using deco_type = decor<T, CharT, TraitT>;
    void const * const p = s.pword(deco_type::xindex);
    // delete if already set
    if (p) delete static_cast<deco_type const *>(p);
    s.pword(deco_type::xindex) = static_cast<void *>(new deco_type{ std::move(h) });
    // check whether we alread have a callback registered
    if (s.iword(deco_type::xindex) == 0)
    { // if this is not the case register callback and set iword
      s.register_callback(detail::manage_decor<T, CharT, TraitT>, deco_type::xindex);
      s.iword(deco_type::xindex) = 1;
    }
    return s;
  }

  template<class T, class CharT = char, class TraitT = std::char_traits<CharT>>
  struct defaulted
  {
    static inline decor<T, CharT, TraitT> decoration()
    {
      return{ detail::default_data<CharT>::decor[0],
        detail::default_data<CharT>::decor[1],
        detail::default_data<CharT>::decor[2] };
    }
  };

  template<class T, class CharT = char, class TraitT = std::char_traits<CharT>>
  decor<T, CharT, TraitT> decoration(
    std::basic_string<CharT, TraitT> const & prefix,
    std::basic_string<CharT, TraitT> const & delimiter,
    std::basic_string<CharT, TraitT> const & postfix)
  {
    return{ prefix, delimiter, postfix };
  }

  template<class T, class CharT = char,
  class TraitT = std::char_traits < CharT >>
    decor<T, CharT, TraitT> decoration(
      std::basic_string<CharT, TraitT> const & delimiter)
  {
    using str_type = std::basic_string<CharT, TraitT>;
    return{ defaulted<T, CharT, TraitT>::decoration().prefix,
      delimiter, defaulted<T, CharT, TraitT>::decoration().postfix };
  }

  template<class T, class CharT = char,
  class TraitT = std::char_traits < CharT >>
    decor<T, CharT, TraitT> decoration(CharT const * const prefix,
      CharT const * const delimiter, CharT const * const postfix)
  {
    using str_type = std::basic_string<CharT, TraitT>;
    return{ str_type{ prefix }, str_type{ delimiter }, str_type{ postfix } };
  }

  template<class T, class CharT = char,
  class TraitT = std::char_traits < CharT >>
    decor<T, CharT, TraitT> decoration(CharT const * const delimiter)
  {
    using str_type = std::basic_string<CharT, TraitT>;
    return{ defaulted<T, CharT, TraitT>::decoration().prefix,
      str_type{ delimiter }, defaulted<T, CharT, TraitT>::decoration().postfix };
  }

  template<typename T, std::size_t N, std::size_t L>
  struct tuple
  {
    template<class CharT, class TraitT>
    static void print(std::basic_ostream<CharT, TraitT>& s, T const & value,
      std::basic_string<CharT, TraitT> const &delimiter)
    {
      s << std::get<N>(value) << delimiter;
      tuple<T, N + 1, L>::print(s, value, delimiter);
    }
  };

  template<typename T, std::size_t N>
  struct tuple<T, N, N>
  {
    template<class CharT, class TraitT>
    static void print(std::basic_ostream<CharT, TraitT>& s, T const & value,
      std::basic_string<CharT, TraitT> const &) {
      s << std::get<N>(value);
    }
  };

}

template<class CharT, class TraitT>
std::basic_ostream<CharT, TraitT> & operator<< (
  std::basic_ostream<CharT, TraitT> &s, std::tuple<> const & v)
{
  using deco_type = pretty::decor<std::tuple<void*>, CharT, TraitT>;
  using defaulted_type = pretty::defaulted<std::tuple<void*>, CharT, TraitT>;
  void const * const p = s.pword(deco_type::xindex);
  auto const d = static_cast<deco_type const * const>(p);
  s << (d ? d->prefix : defaulted_type::decoration().prefix);
  s << (d ? d->postfix : defaulted_type::decoration().postfix);
  return s;
}

template<class CharT, class TraitT, class ... T>
std::basic_ostream<CharT, TraitT> & operator<< (
  std::basic_ostream<CharT, TraitT> &s, std::tuple<T...> const & v)
{
  using deco_type = pretty::decor<std::tuple<void*>, CharT, TraitT>;
  using defaulted_type = pretty::defaulted<std::tuple<void*>, CharT, TraitT>;
  using pretty_tuple = pretty::tuple<std::tuple<T...>, 0U, sizeof...(T)-1U>;
  void const * const p = s.pword(deco_type::xindex);
  auto const d = static_cast<deco_type const * const>(p);
  s << (d ? d->prefix : defaulted_type::decoration().prefix);
  pretty_tuple::print(s, v, d ? d->delimiter :
    defaulted_type::decoration().delimiter);
  s << (d ? d->postfix : defaulted_type::decoration().postfix);
  return s;
}

template<class T, class U, class CharT, class TraitT>
std::basic_ostream<CharT, TraitT> & operator<< (
  std::basic_ostream<CharT, TraitT> &s, std::pair<T, U> const & v)
{
  using deco_type = pretty::decor<std::pair<T, U>, CharT, TraitT>;
  using defaulted_type = pretty::defaulted<std::pair<T, U>, CharT, TraitT>;
  void const * const p = s.pword(deco_type::xindex);
  auto const d = static_cast<deco_type const * const>(p);
  s << (d ? d->prefix : defaulted_type::decoration().prefix);
  s << v.first;
  s << (d ? d->delimiter : defaulted_type::decoration().delimiter);
  s << v.second;
  s << (d ? d->postfix : defaulted_type::decoration().postfix);
  return s;
}


template<class T, class CharT = char,
class TraitT = std::char_traits < CharT >>
  typename std::enable_if < pretty::detail::is_range<T>::value,
  std::basic_ostream < CharT, TraitT >> ::type & operator<< (
    std::basic_ostream<CharT, TraitT> &s, T const & v)
{
  bool first(true);
  using deco_type = pretty::decor<T, CharT, TraitT>;
  using default_type = pretty::defaulted<T, CharT, TraitT>;
  void const * const p = s.pword(deco_type::xindex);
  auto d = static_cast<pretty::decor<T, CharT, TraitT> const * const>(p);
  s << (d ? d->prefix : default_type::decoration().prefix);
  for (auto const & e : v)
  { // v is range thus range based for works
    if (!first) s << (d ? d->delimiter : default_type::decoration().delimiter);
    s << e;
    first = false;
  }
  s << (d ? d->postfix : default_type::decoration().postfix);
  return s;
}

#endif // pretty_print_0x57547_sa4884X_0_1_h_guard_