#pragma once
#include <chrono>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <vector>

namespace aux {
template <typename Number>
Number Random(Number sub = 0, Number upper = 100) {
  static std::random_device rd;
  static std::default_random_engine generator(rd());

  std::uniform_int_distribution<Number> distribution(sub, upper);
  return distribution(generator);
}

template <typename Number,
          typename std::enable_if<std::is_integral<Number>::value, Number>::type
              * = nullptr>
std::vector<Number> Randoms(size_t quantity, Number sub = 0,
                            Number upper = 100) {
  std::random_device rd;
  std::default_random_engine generator(rd());
  std::uniform_int_distribution<Number> distribution(sub, upper);

  std::vector<Number> values;
  values.reserve(quantity);
  for (size_t i = 0; i < quantity; i++) {
    values.push_back(distribution(generator));
  }
  return values;
}

template <typename Number,
          typename std::enable_if<std::is_floating_point<Number>::value,
                                  Number>::type * = nullptr>
std::vector<Number> Randoms(size_t quantity, Number sub = 0.0,
                            Number upper = 100.0) {
  std::random_device rd;
  std::default_random_engine generator(rd());
  std::uniform_real_distribution<Number> distribution(sub, upper);

  std::vector<Number> values;
  values.reserve(quantity);
  for (size_t i = 0; i < quantity; i++) {
    values.push_back(distribution(generator));
  }
  return values;
}

int RandomInt(size_t sub = 0, size_t upper = 100) {
  static std::random_device rd;
  static std::default_random_engine generator(rd());
  static std::uniform_int_distribution<int> distribution(sub, upper);
  return std::move(distribution(generator));
}

std::vector<int> RandomInts(size_t quantity, size_t sub = 0,
                            size_t upper = 100) {
  std::vector<int> values;
  for (size_t i = 0; i < quantity; i++) {
    values.emplace_back(RandomInt(sub, upper));
  }
  return std::move(values);
}

bool EvenlyDivisible(double a, double b, double tolerance = 1e-6) {
  // double quotient = a / b;

  // // 计算四舍五入后的整数商
  // double rounded_quotient = std::round(quotient);

  // // 计算整数商与原始a之间的差值
  // double difference = std::abs(a - rounded_quotient * b);

  // return difference < tolerance;
  double c = a / b;
  double d = std::remainder(a, b);
  bool is_exact = std::isnormal(c);
  bool is_approximately_exact = std::abs(d) < tolerance;
  return is_exact || is_approximately_exact;
}

std::vector<std::string> ReadLines(std::string pwd) {
  std::vector<std::string> content;
  if (!std::filesystem::exists(pwd)) {
    return content;
  }
  std::filesystem::directory_entry entry(pwd);
  if (entry.status().type() == std::filesystem::file_type::directory) {
    return content;
  }

  std::fstream file(pwd, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    return content;
  }

  std::string line;
  while (std::getline(file, line)) {
    content.emplace_back(std::move(line));
  }
  file.close();
  return std::move(content);
}

int WriteLines(std::vector<std::string> &content, std::string pwd) {
  std::filesystem::path dir = pwd;
  if (!std::filesystem::exists(dir.parent_path())) {
    std::filesystem::create_directory(dir.parent_path());
  }

  std::ofstream file(
      pwd, std::ios::out | std::ios::binary | std::ios::app | std::ios::ate);
  if (!file.is_open()) {
    return -1;
  }
  for (std::string &line : content) {
    file << line << std::endl;
  }
  file.close();
  return 0;
}

std::vector<std::string> SplitDelim(std::string &src, std::string delim) {
  std::vector<std::string> content;
  std::regex reg(delim);
  std::sregex_token_iterator tokens(src.cbegin(), src.cend(), reg, -1);
  std::sregex_token_iterator end;
  for (; tokens != end; ++tokens) {
    content.emplace_back(*tokens);
  }
  return std::move(content);
}

std::vector<std::string> Split(const std::string &src,
                               const std::string &delim) {
  std::vector<std::string> sub;
  // for (const auto word :
  //      std::views::split(std::string_view(src), std::string_view(delim))) {
  //   sub.emplace_back(word.begin(), word.end());
  // }

  size_t quick = 0;
  size_t slow = 0;
  std::string token;
  while ((quick = src.find(delim, slow)) != std::string::npos) {
    token = src.substr(slow, quick - slow);
    sub.emplace_back(token);
    slow = quick + delim.size();
  }

  return sub;
}

std::string FileToString(std::string pwd) {
  if (!std::filesystem::exists(pwd)) {
    return "";
  }
  std::ifstream ifs(pwd);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      std::istreambuf_iterator<char>());
  return std::move(content);
}

std::vector<std::byte> FileToByte(const std::string &pwd) {
  std::vector<std::byte> result;
  std::ifstream ifs(pwd, std::ios::binary);
  if (ifs.is_open()) {
    auto file_size = ifs.seekg(0, std::ios_base::end).tellg();
    ifs.seekg(0, std::ios_base::beg);
    result.resize(file_size);
    ifs.read((char *)result.data(), file_size);
    ifs.close();
  }
  return result;
}

void ByteToFile(const std::string &pwd, std::vector<std::byte> &bytes) {
  std::ofstream ofs(pwd, std::ios::binary);
  if (ofs.is_open()) {
    ofs.write((char *)bytes.data(), bytes.size());
    ofs.close();
  }
}

bool IsAlphabet(const char a) {
  return ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z');
}

// std::string w2s(const std::wstring &wide) {
//   // std::string str;
//   // std::transform(wide.begin(), wide.end(), std::back_inserter(str),
//   //                [](unsigned short c) { return char(c); });
//   // return str;
//   static std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
//   return std::move(cvt.to_bytes(wide));
// }

// std::wstring s2w(const std::string &str) {
//   static std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
//   return cvt.from_bytes(str);
// }

class IO {
 public:
  static std::vector<std::string> ReadLines(std::string pwd) {
    std::vector<std::string> content;
    if (!std::filesystem::exists(pwd)) {
      return content;
    }
    std::filesystem::directory_entry entry(pwd);
    if (entry.status().type() == std::filesystem::file_type::directory) {
      return content;
    }

    std::fstream file(pwd, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      return content;
    }

    std::string line;
    while (std::getline(file, line)) {
      content.emplace_back(std::move(line));
    }
    file.close();
    return std::move(content);
  }
  static int WriteLines(std::vector<std::string> &content, std::string pwd) {
    // if (!std::filesystem::exists(pwd))
    // {
    //     std::filesystem::create_directory(pwd);
    // }

    std::ofstream file(
        pwd, std::ios::out | std::ios::binary | std::ios::app | std::ios::ate);
    if (!file.is_open()) {
      return -1;
    }
    for (std::string &line : content) {
      file << line << std::endl;
    }
    file.close();
    return 0;
  }
};
}  // namespace aux

namespace aux {
class StopWatch {
 public:
  std::chrono::steady_clock::time_point Start() {
    start_ = std::chrono::steady_clock::now();
    return start_;
  }
  std::chrono::steady_clock::time_point Stop() {
    end_ = std::chrono::steady_clock::now();
    return end_;
  }
  long long ElapsedMilliseconds() {
    long long elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_)
            .count();
    return elapsed;
  }
  long long ElapsedMicroseconds() {
    long long elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_)
            .count();
    return elapsed;
  }

 private:
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
};

namespace calendar {
using namespace std::chrono;

int Year(const time_point<system_clock> now = system_clock::now()) {
  const year_month_day ymd{floor<days>(now)};
  return static_cast<int>(ymd.year());
}

unsigned Month(const time_point<system_clock> now = system_clock::now()) {
  const year_month_day ymd{floor<days>(now)};
  return static_cast<unsigned>(ymd.month());
}

unsigned Day(const time_point<system_clock> now = system_clock::now()) {
  const year_month_day ymd{floor<days>(now)};
  return static_cast<unsigned>(ymd.day());
}

int64_t Hour(const time_point<system_clock> now = system_clock::now()) {
  std::chrono::duration<int64_t, std::nano> ss = now.time_since_epoch();
  auto hh = std::chrono::floor<std::chrono::days>(ss);
  std::chrono::hh_mm_ss<std::chrono::milliseconds> tod{
      std::chrono::duration_cast<std::chrono::milliseconds>(ss - hh)};
  return tod.hours().count();
}

int64_t Minute(const time_point<system_clock> now = system_clock::now()) {
  std::chrono::duration<int64_t, std::nano> ss = now.time_since_epoch();
  auto hh = std::chrono::floor<std::chrono::days>(ss);
  std::chrono::hh_mm_ss<std::chrono::milliseconds> tod{
      std::chrono::duration_cast<std::chrono::milliseconds>(ss - hh)};
  return tod.minutes().count();
}

int64_t Second(const time_point<system_clock> now = system_clock::now()) {
  std::chrono::duration<int64_t, std::nano> ss = now.time_since_epoch();
  auto hh = std::chrono::floor<std::chrono::days>(ss);
  std::chrono::hh_mm_ss<std::chrono::milliseconds> tod{
      std::chrono::duration_cast<std::chrono::milliseconds>(ss - hh)};
  return tod.seconds().count();
}

int64_t Subsecond(const time_point<system_clock> now = system_clock::now()) {
  std::chrono::duration<int64_t, std::nano> ss = now.time_since_epoch();
  auto hh = std::chrono::floor<std::chrono::days>(ss);
  std::chrono::hh_mm_ss<std::chrono::milliseconds> tod{
      std::chrono::duration_cast<std::chrono::milliseconds>(ss - hh)};
  return tod.subseconds().count();
}

}  // namespace calendar
}  // namespace aux
