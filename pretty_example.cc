#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <array>
#include <tuple>
#include <valarray>
#include <utility>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <random>

#include "pretty.hpp"

namespace
{
  std::string random_string(std::size_t length)
  {
    static const std::string alphanums =
      "0123456789"
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::mt19937 rg(std::random_device{}());
    std::uniform_int_distribution<> pick(0, alphanums.size() - 1);

    std::string s;
    s.reserve(length);
    while (length--)
    {
      s += alphanums[std::uniform_int_distribution<unsigned>
      {0, alphanums.size() - 1}(rg)];
    }

    return s;
  }
}

PRETTY_DEFAULT_DECORATION(std::vector<int>, "[[", "||", ">")

int main(int argc, char * argv[])
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


  std::system("pause");
}