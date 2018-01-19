// Copyright 2017 Husky Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "husky-base/bin_stream.h"

#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace husky {
namespace {

using ::husky::base::BinStream;

class TestBinStream : public testing::Test {
 public:
  TestBinStream() {}
  ~TestBinStream() {}

 protected:
  void SetUp() {}
  void TearDown() {}
};

class TestBinStreamWithInheritance {
 public:
  explicit TestBinStreamWithInheritance(int y) : x(y) {}

  BinStream& serialize(BinStream& stream) const { return stream << x; }

  BinStream& deserialize(BinStream& stream) { return stream >> x; }

  bool operator==(const TestBinStreamWithInheritance& rhs) const { return x == rhs.x; }

 protected:
  int x;
};

class TestBinStreamWithInheritanceSubclass : public TestBinStreamWithInheritance {
 public:
  explicit TestBinStreamWithInheritanceSubclass(int y) : TestBinStreamWithInheritance(y) {}
};

TEST_F(TestBinStream, InitAndDelete) {
  BinStream* stream = new BinStream();
  ASSERT_TRUE(stream != NULL);
  delete stream;
}

TEST_F(TestBinStream, BoolType) {
  bool input = true;
  BinStream stream;
  stream << input;
  EXPECT_EQ(stream.size(), 1);
  bool output = false;
  stream >> output;
  EXPECT_EQ(output, input);
}

TEST_F(TestBinStream, CharType) {
  char input = 'a';
  BinStream stream;
  stream << input;
  EXPECT_EQ(stream.size(), 1);
  char output = 'b';
  stream >> output;
  EXPECT_EQ(output, input);
}

TEST_F(TestBinStream, IntegerType) {
  int input = 1001;
  BinStream stream;
  stream << input;
  EXPECT_EQ(stream.size(), 4);
  int output = 0;
  stream >> output;
  EXPECT_EQ(output, input);
}

TEST_F(TestBinStream, FloatType) {
  float input = .123;
  BinStream stream;
  stream << input;
  EXPECT_EQ(stream.size(), 4);
  float output = .0;
  stream >> output;
  EXPECT_FLOAT_EQ(output, input);
}

TEST_F(TestBinStream, DoubleType) {
  double input = .123456;
  BinStream stream;
  stream << input;
  EXPECT_EQ(stream.size(), 8);
  double output = .0;
  stream >> output;
  EXPECT_DOUBLE_EQ(output, input);
}

TEST_F(TestBinStream, EmptyString) {
  std::string input, output;
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_STREQ(output.c_str(), input.c_str());
}

TEST_F(TestBinStream, String) {
  std::string input = "123abc";
  BinStream stream;
  stream << input;
  std::string output;
  stream >> output;
  EXPECT_STREQ(output.c_str(), input.c_str());
}

TEST_F(TestBinStream, EmptyVector) {
  std::vector<int> input, output;
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(output.size(), input.size());
}

TEST_F(TestBinStream, Vector) {
  std::vector<int> input{1, 0, -2, 3};
  BinStream stream;
  stream << input;
  std::vector<int> output;
  stream >> output;
  EXPECT_EQ(output.size(), input.size());
  for (int i = 0; i < output.size(); i++)
    EXPECT_EQ(output[i], input[i]);
}

TEST_F(TestBinStream, VectorBool) {
  std::vector<bool> input{1, 0, 1};
  BinStream stream;
  stream << input;
  std::vector<bool> output;
  stream >> output;
  EXPECT_EQ(output.size(), input.size());
  for (int i = 0; i < output.size(); i++)
    EXPECT_EQ(output[i], input[i]);
}

TEST_F(TestBinStream, Pair) {
  std::vector<int> itemVector{2, 2, 2, 2};
  BinStream stream;
  std::pair<int, std::vector<int>> input(1, itemVector);
  stream << input;
  std::pair<int, std::vector<int>> output;
  stream >> output;
  EXPECT_EQ(output.first, input.first);
  for (int i = 0; i < output.second.size(); i++)
    EXPECT_EQ(output.second[i], input.second[i]);
}

TEST_F(TestBinStream, PairVector) {
  std::vector<int> itemVector{2, 2, 2, 2};
  std::pair<int, std::vector<int>> p1(1, itemVector);
  std::pair<int, std::vector<int>> p2(2, itemVector);
  std::vector<std::pair<int, std::vector<int>>> input;
  input.push_back(p1);
  input.push_back(p2);

  BinStream stream;
  stream << input;
  std::vector<std::pair<int, std::vector<int>>> output;
  stream >> output;

  EXPECT_EQ(output.size(), input.size());
  for (int i = 0; i < output.size(); ++i)
    EXPECT_EQ(output[i], input[i]);
}

TEST_F(TestBinStream, TestSharedPtr) {
  auto input = std::make_shared<int>(100);
  std::shared_ptr<int> output(new int);
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(*input, *output);
}

TEST_F(TestBinStream, TestUniquePtr) {
  auto input = std::make_unique<int>(100);
  std::unique_ptr<int> output(new int);
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(*input, *output);
}

TEST_F(TestBinStream, EmptyMap) {
  std::map<int, int> input, output;
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(output.size(), input.size());
}

TEST_F(TestBinStream, Map) {
  std::map<int, int> input, output;
  input.insert(std::pair<int, int>(1, 2));
  input.insert(std::pair<int, int>(3, 4));
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(input.size(), output.size());
  std::map<int, int>::iterator iter;
  iter = output.find(1);
  EXPECT_EQ(iter->second, 2);
  iter = output.find(3);
  EXPECT_EQ(iter->second, 4);
}

TEST_F(TestBinStream, Set) {
  std::set<int> input, output;
  input.insert(1);
  input.insert(100);
  input.insert(10);
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(input.size(), output.size());
  EXPECT_EQ(1, output.count(1));
  EXPECT_EQ(1, output.count(10));
  EXPECT_EQ(1, output.count(100));
}

TEST_F(TestBinStream, UnorderedSet) {
  std::unordered_set<int> input, output;
  input.insert(1);
  input.insert(100);
  input.insert(10);
  BinStream stream;
  stream << input;
  stream >> output;
  EXPECT_EQ(input.size(), output.size());
  EXPECT_EQ(1, output.count(1));
  EXPECT_EQ(1, output.count(10));
  EXPECT_EQ(1, output.count(100));
}

TEST_F(TestBinStream, OtherBinStream) {
  std::string s = "123abc";
  BinStream stream;
  stream << s;
  BinStream input, output;
  input << stream;
  input >> output;
  EXPECT_EQ(stream.size(), output.size());
  std::string ss;
  output >> ss;
  EXPECT_STREQ(s.c_str(), ss.c_str());
}

TEST_F(TestBinStream, Mixture) {
  bool a = true;
  std::string b = "husky";
  double c = .343412;
  int d = 65535;
  BinStream stream;
  stream << a << b << c << d;
  bool aa;
  std::string bb;
  double cc;
  int dd;
  stream >> aa >> bb >> cc >> dd;
  EXPECT_EQ(a, aa);
  EXPECT_STREQ(b.c_str(), bb.c_str());
  EXPECT_EQ(c, cc);
  EXPECT_EQ(d, dd);
}

TEST_F(TestBinStream, UsingMemberFunction) {
  BinStream stream;
  TestBinStreamWithInheritance a(123);
  stream << a;
  TestBinStreamWithInheritance b(0);
  stream >> b;
  EXPECT_EQ(a, b);
}

TEST_F(TestBinStream, WithInheritance) {
  BinStream stream;
  TestBinStreamWithInheritanceSubclass a(123);
  stream << a;
  TestBinStreamWithInheritanceSubclass b(0);
  stream >> b;
  EXPECT_EQ(a, b);
}

}  // namespace
}  // namespace husky
