/*
 * DataPresentationTest.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: mircot
 */

#include "TestInfo.h"

#include <fstream>

#include "cpsCore/Utilities/DataPresentation/BinarySerialization.hpp"
#include "cpsCore/Utilities/DataPresentation/DataPresentation.h"

namespace
{
enum TestContent {
	DUMMY
};

}
TEST_CASE("Data Presentation Test")
{
	double test1 = 5.312;
	double test2 = 1151351.21;

	std::string packet;
	BinaryToArchive toArchive(packet);

	toArchive << test1;
	toArchive << test2;

	std::string packet2;
	BinaryToArchive toArchive2(packet2);

	toArchive2 << test1;

	std::string packet3;
	BinaryToArchive toArchive3(packet3);

	toArchive3 << test2;

	CHECK(packet.size() == 16);

	BinaryFromArchive fromArchive(packet);
	BinaryFromArchive fromArchive3(packet3);

	double check1 = 0;
	double check2 = 0;
	fromArchive >> check1;
	fromArchive >> check2;

	CHECK(test1 == check1);
	CHECK(test2 == check2);

	fromArchive3 >> check2;

	CHECK(test2 == check2);
}

TEST_CASE("String Data Test")
{
	std::string s("Hallo ... yeah");

	DataPresentation dp;
	Packet packet = dp.serialize(s);
	dp.addHeader(packet, TestContent::DUMMY);

	TestContent content = dp.extractHeader<TestContent>(packet);
	auto check = dp.deserialize<std::string>(packet);

	CHECK(content == TestContent::DUMMY);
	CHECK_FALSE(s.compare(check));

}

TEST_CASE("Test Float Arrays")
{
	float test[] = {1.2, 4.3, 5.2, 6.1};
	std::ofstream fileOut("test_array", std::ofstream::out | std::ofstream::binary);

	FileToArchive to(fileOut);
	dp::serialize(to, reinterpret_cast<char*>(test), sizeof(test));
	fileOut.close();

	float testRead[4];
	std::ifstream fileIn("test_array", std::ifstream::in | std::ifstream::binary);

	FileFromArchive from(fileIn);
	dp::serialize(from, reinterpret_cast<char*>(testRead), sizeof(test));
	fileIn.close();

	CHECK(test[0] == testRead[0]);
	CHECK(test[1] == testRead[1]);
	CHECK(test[2] == testRead[2]);
	CHECK(test[3] == testRead[3]);
}

TEST_CASE("Test Int Arrays")
{
	int test[] = {12, 13, 5, 10};
	std::ofstream fileOut("test_array", std::ofstream::out | std::ofstream::binary);

	FileToArchive to(fileOut);
	dp::serialize(to, reinterpret_cast<char*>(test), sizeof(test));
	fileOut.close();

	int testRead[4];
	std::ifstream fileIn("test_array", std::ifstream::in | std::ifstream::binary);

	FileFromArchive from(fileIn);
	dp::serialize(from, reinterpret_cast<char*>(testRead), sizeof(test));
	fileIn.close();

	CHECK(test[0] == testRead[0]);
	CHECK(test[1] == testRead[1]);
	CHECK(test[2] == testRead[2]);
	CHECK(test[3] == testRead[3]);
}



TEST_CASE("Test Eigen Matrix")
{
	Eigen::Matrix<int, -1, -1> mat = Eigen::Matrix<int, -1, -1>::Random(7,3);

	DataPresentation dp;
	Packet packet = dp.serialize(mat);

	auto matRes = dp.deserialize<Eigen::Matrix<int, -1, -1>>(packet);

	CHECK(matRes == mat);

	Eigen::Matrix<float, -1, -1> matFloat = Eigen::Matrix<float, -1, -1>::Random(31,27);

	Packet packetFloat = dp.serialize(matFloat);

	auto matFloatRes = dp.deserialize<Eigen::Matrix<float, -1, -1>>(packetFloat);

	CHECK(matFloat == matFloatRes);
}

