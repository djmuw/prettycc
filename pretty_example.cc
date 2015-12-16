#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <sstream>

#include "pretty.hpp"

PRETTY_DEFAULT_DECORATION(std::vector<int>, "[[", "||", ">")

int main()
{

  // Global default decoration
  {
    std::vector<double> v = { 1.1, 2.1, 3.1, 1.2 };
    std::cout << "Vector, default decoration: " << v << '\n' << '\n';
  }
  // global custom decoration
  {
    std::vector<int> v = { 1, 5, 4, 4 };
    std::stringstream ss;
    ss << v;
    std::cout << "Vector, globally customized decoration: " << v << '\n';
    std::cout << "From stringstream: " << ss.str() << '\n' << '\n';
  }
  // stream-local custom decoration
  {
    std::vector<int> v = { 7, 5, 6, 4, 2 };
    std::cout << pretty::decoration<std::vector<int>>("#", " : ", "#");
    std::stringstream ss;
    ss << v;
    std::cout << "Vector, locally customized decoration: " << v << '\n';
    std::cout << "From stringstream: " << ss.str() << '\n' << '\n';
  }
  // string output
  {
    std::array<std::string, 4u> v = { "Hey", "folks", "whazzapp", "?" };
    std::cout << pretty::decoration<decltype(v)>("\"", " ", "\"");
    std::stringstream ss;
    ss << v;
    std::cout << "Array of strings, locally decorated: " << v << '\n';
    std::cout << "Array of strings, globally decorated: " << ss.str() << '\n' << '\n';
  }
  // two streams of same type with different decorations
  {
    std::map<int, std::string> m;
    m.insert({ 1, "Hello" });
    m.insert({ 7, "World!" });
    std::stringstream a, b;

    a << pretty::decoration<std::pair<int const, std::string>>("[", " <- '", "']");
    b << pretty::decoration<std::pair<int const, std::string>>("((", " << ", "))");
    a << m;
    b << m;
    std::cout << "stringstream a << map<int, string>: " << a.str() << '\n';
    std::cout << "stringstream b << map<int, string>: " << b.str() << '\n' << '\n';
  }

  // TODO: add more ;)

  return 0;

}