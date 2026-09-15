/*
 * @copyright 한국기술교육대학교 컴퓨터공학부 자료구조및실습
 * @version 2026년도 2학기
 * @author 김상진
 * @file QueueTests.cpp
 * 테스트 프로그램
 */

#include <cstddef>
#include <stdexcept>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <utility>
#include "Queue.h"
#include "QueueIterator.h"
#include "gtest/gtest.h"

std::random_device randDevice{};
std::mt19937 randGen{randDevice()};


int generateRandomInt() {
	static std::uniform_int_distribution<> dist100(1, 100);
	return dist100(randGen);
}

std::vector<int> generateRandomInts(size_t capacity) {

	std::vector<int> ret(capacity);
	std::generate(ret.begin(), ret.end(),
		[&]() { return generateRandomInt(); });
	return ret;
}

std::vector<int> generateUniqueInts(size_t capacity) {
	std::unordered_set<int> set{};
	do {
		set.insert(generateRandomInt());
	}
	while(set.size() < capacity);
	return std::vector<int>{set.begin(), set.end()};
}


TEST(Queue, contructorTest)
{
	Queue queue1{};
	ASSERT_EQ(queue1.size(), 0);
	ASSERT_EQ(queue1.capacity(), 10);
	ASSERT_TRUE(queue1.isEmpty());
	Queue queue2(5);
	ASSERT_EQ(queue2.size(), 0);
	ASSERT_EQ(queue2.capacity(), 5);
	ASSERT_TRUE(queue2.isEmpty());
	Queue queue3(0);
	ASSERT_EQ(queue3.size(), 0);
	ASSERT_EQ(queue3.capacity(), 10);
	ASSERT_TRUE(queue3.isEmpty());
	Queue queue4(5, 5);
	ASSERT_EQ(queue4.size(), 5);
	ASSERT_EQ(queue4.capacity(), 5);
	ASSERT_FALSE(queue4.isEmpty());
}

TEST(Queue, PushAndPopTest)
{
	for(int i{0}; i < 10; ++i) {
		Queue queue{};
		ASSERT_THROW(queue.peek(), std::logic_error);
		ASSERT_THROW(queue.pop(), std::logic_error);
		std::vector<int> testData{generateRandomInts(50)};
		for(int j{0}; j < testData.size(); ++j) {
			queue.push(testData[j]);
			ASSERT_EQ(j + 1, queue.size());
			ASSERT_EQ(testData[0], queue.peek());
		}
		size_t size{testData.size()};
		for(auto it{testData.begin()}; it != testData.end(); ++it) {
			ASSERT_EQ(*it, queue.pop());
			ASSERT_EQ(--size, queue.size());
		}
	}
}

TEST(Queue, increaseCapacityTest)
{
	std::vector<int> testData{generateRandomInts(50)};
	Queue queue1{};
	ASSERT_EQ(queue1.size(), 0);
	ASSERT_EQ(queue1.capacity(), 10);
	for(size_t i{0}; i < testData.size(); ++i) {
		queue1.push(testData[i]);
		ASSERT_EQ(i + 1, queue1.size());
		if(i == 10 || i == 20 || i == 40) {
			ASSERT_EQ(i * 2, queue1.capacity());
		}
	}

	Queue queue2(1);
	ASSERT_EQ(queue2.size(), 0);
	ASSERT_EQ(queue2.capacity(), 1);
	for(size_t i{0}; i < testData.size(); ++i) {
		queue2.push(testData[i]);
		ASSERT_EQ(i + 1, queue2.size());
		if(i == 1 || i == 2 || i == 4 || i == 8 || i == 16 || i == 32) {
			ASSERT_EQ(i * 2, queue2.capacity());
		}
	}
}

TEST(Queue, initializer_list_test)
{
	std::vector<int> testData{9, 8, 7, 6, 5, 4, 3};
	Queue queue1{9, 8, 7, 6, 5, 4, 3};
	size_t size{testData.size()};
	ASSERT_EQ(size, queue1.size());
	for(auto it{testData.begin()}; it != testData.end(); ++it) {
		ASSERT_EQ(*it, queue1.pop());
		ASSERT_EQ(--size, queue1.size());
	}

	Queue queue2({});
	ASSERT_EQ(queue2.size(), 0);
	ASSERT_EQ(queue2.capacity(), 10);
	ASSERT_TRUE(queue2.isEmpty());
}

TEST(Queue, clear_test)
{
	Queue queue(20);
	queue.push(10);
	queue.push(20);
	queue.push(30);
	ASSERT_EQ(3, queue.size());
	ASSERT_EQ(20, queue.capacity());
	queue.clear();
	ASSERT_EQ(0, queue.size());
	ASSERT_EQ(20, queue.capacity());
	ASSERT_THROW(queue.peek(), std::logic_error);
	ASSERT_THROW(queue.pop(), std::logic_error);
	queue.push(40);
	ASSERT_EQ(40, queue.peek());
	ASSERT_EQ(1, queue.size());
}

TEST(Queue, iterator_test)
{
	Queue queue{};
	ASSERT_EQ(queue.begin(), queue.end());

	std::vector<int> testData{generateRandomInts(50)};
	for(const auto& item: testData) queue.push(item);
	auto itA{queue.begin()};
	auto itB{testData.begin()};
	while(itA != queue.end() && itB != testData.end()) {
		ASSERT_EQ(*itA, *itB);
		++itA;
		++itB;
	}
	ASSERT_EQ(itA, queue.end());
	ASSERT_EQ(itB, testData.end());
}


bool equals(const Queue& A, const Queue& B) {
	auto itA{A.begin()};
	auto itB{B.begin()};
	while(itA != A.end() && itB != B.end()) {
		if(*itA != *itB) return false;
		++itA;
		++itB;
	}
	return itA == A.end() && itB == B.end();
}

TEST(Queue, Big5)
{
	std::vector<int> testData1{1, 2, 3, 4, 5};
	std::vector<int> testData2{6, 7, 8, 9, 10};
	Queue queue1{1, 2, 3, 4, 5};
	Queue queue2{6, 7, 8, 9, 10};
	Queue queue3;
	Queue queue4(queue2);

	// copy constructor test
	ASSERT_TRUE(equals(queue2, queue4));
	queue2.pop();
	ASSERT_FALSE(equals(queue2, queue4));

	queue3 = queue1;
	// copy assignment test
	ASSERT_TRUE(equals(queue3, queue1));
	queue1.pop();
	ASSERT_FALSE(equals(queue3, queue1));

	// move constructor test
	Queue queue5{std::move(queue3)};
	size_t size{testData1.size()};
	for(auto it{testData1.begin()}; it != testData1.end(); ++it) {
		ASSERT_EQ(*it, queue5.pop());
		ASSERT_EQ(--size, queue5.size());
	}
	ASSERT_TRUE(queue3.isEmpty());

	Queue queue6{1, 2, 3, 4, 5};
	queue3 = queue6;
	// move assignment test
	queue3 = std::move(queue4);
	size = testData2.size();
	for(auto it{testData2.begin()}; it != testData2.end(); ++it) {
		ASSERT_EQ(*it, queue3.pop());
		ASSERT_EQ(--size, queue3.size());
	}
	size = testData1.size();
	for(auto it{testData1.begin()}; it != testData1.end(); ++it) {
		ASSERT_EQ(*it, queue4.pop());
		ASSERT_EQ(--size, queue4.size());
	}
}

