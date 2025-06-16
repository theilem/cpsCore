/*
 * DataPresentationTest.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>

#include <fstream>

#include <cpsCore/Utilities/DataPresentation/BinarySerialization.hpp>
#include <cpsCore/Utilities/DataPresentation/DataPresentation.h>

#include "cpsCore/Utilities/DataPresentation/hamming74.h"

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

	ArchiveOptions opts;
	opts.compressDouble = false;
	std::string packet;
	BinaryToArchive toArchive(packet, opts);

	toArchive << test1;
	toArchive << test2;

	std::string packet2;
	BinaryToArchive toArchive2(packet2, opts);

	toArchive2 << test1;

	std::string packet3;
	BinaryToArchive toArchive3(packet3, opts);

	toArchive3 << test2;

	CHECK(packet.size() == 16);

	BinaryFromArchive fromArchive(packet, opts);
	BinaryFromArchive fromArchive3(packet3, opts);

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

TEST_CASE("Test ordered maps")
{
	std::map<std::string, std::string> data;
	data["cde"] = "fgh";
	data["bcd"] = "efg";
	data["abc"] = "def";

	DataPresentation dp;

	Packet p = dp.serialize(data);

	auto reconstruct = dp.deserialize<std::map<std::string, std::string>>(p);

	CHECK(reconstruct["cde"] == "fgh");
	CHECK(reconstruct["bcd"] == "efg");
	CHECK(reconstruct["abc"] == "def");
	CHECK(reconstruct.size() == 3);

	// std::map needs to preserve order
	std::vector<std::string> expectedKey = {"abc", "bcd", "cde"};
	for (auto [idx, it] = std::tuple{unsigned{0}, reconstruct.begin()}; idx < reconstruct.size(); it++, idx++)
	{
		CHECK(it->first == expectedKey[idx]);
	}
}

TEST_CASE("Test unordered maps")
{
	std::unordered_map<std::string, std::string> data;
	data["abc"] = "def";
	data["bcd"] = "efg";
	data["cde"] = "fgh";

	DataPresentation dp;

	Packet p = dp.serialize(data);

	auto reconstruct = dp.deserialize<std::unordered_map<std::string, std::string>>(p);

	CHECK(reconstruct["abc"] == "def");
	CHECK(reconstruct["bcd"] == "efg");
	CHECK(reconstruct["cde"] == "fgh");
	CHECK(reconstruct.size() == 3);
}

TEST_CASE("Test Configuration")
{
	Configuration c;
	c["asd"] = "abc";
	c["a1s"] = 123;
	c["a2d"] = 3.45;

	Configuration d;
	d["az"] = 1;
	d["ay"] = 1.2;
	d["ad"] = "ab";

	c["d"] = d;

	DataPresentation dp;
	Packet p = dp.serialize(c);
	auto reconstruct = dp.deserialize<Configuration>(p);

	CHECK(reconstruct.size() == 4);
	CHECK(reconstruct["asd"].get<std::string>() == "abc");
	CHECK(reconstruct["a1s"].get<int>() == 123);
	CHECK(reconstruct["a2d"].get<double>() == 3.45);

	auto reconstructChild = reconstruct["d"];

	CHECK(reconstructChild.size() == 3);
	CHECK(reconstructChild["az"].get<int>() == 1);
	CHECK(reconstructChild["ay"].get<double>() == 1.2);
	CHECK(reconstructChild["ad"].get<std::string>() == "ab");
}

TEST_CASE("Test Hamming74")
{
	std::string data = "Random test string for Hamming74 encoding and decoding.";
	std::string encoded = Hamming74::encode(data);
	std::vector<bool> corrected;
	std::string decoded = Hamming74::decode(encoded, corrected);
	CHECK(data == decoded);

	Packet packet(data);
	auto encodedPacket = packet.hamming74Encode();
	auto decodedPacket = encodedPacket.hamming74Decode();
	CHECK(packet.getBuffer() == decodedPacket.getBuffer());
}

TEST_CASE("Multi serialization")
{
	DataPresentation dp;
	Packet packet = dp.serialize(1, 2.0, std::string("test"), std::vector<int>{1, 2, 3});

	auto [i, d, s, v] = dp.extract<int, double, std::string, std::vector<int>>(packet);

	CHECK(i == 1);
	CHECK(d == 2.0);
	CHECK(s == "test");
	CHECK(v.size() == 3);
	CHECK(v[0] == 1);
	CHECK(v[1] == 2);
	CHECK(v[2] == 3);

	Packet packet2 = dp.serialize(Vector3(1.0, 2.0, 3.0), std::string("test2"), 42, Vector3(4.0, 5.0, 6.0));
	auto [v1, s2, i2, v2] = dp.extract<Vector3, std::string, int, Vector3>(packet2);
	CHECK(v1 == Vector3(1.0, 2.0, 3.0));
	CHECK(s2 == "test2");
	CHECK(i2 == 42);
	CHECK(v2 == Vector3(4.0, 5.0, 6.0));

}

TEST_CASE("Eigen double compression")
{
	Eigen::Matrix<double, -1, -1> mat = Eigen::Matrix<double, -1, -1>::Random(7, 3);

	DataPresentation dp;
	ArchiveOptions opts;
	opts.compressDouble = false;
	dp.setParams(opts);
	Packet packet = dp.serialize(mat);
	CHECK(packet.getSize() == 7 * 3 * sizeof(double) + 2 * sizeof(Eigen::Index));

	auto matRes = dp.deserialize<Eigen::Matrix<double, -1, -1>>(packet);
	CHECK(matRes == mat);

	opts.compressDouble = true;
	dp.setParams(opts);
	Packet packet2 = dp.serialize(mat);
	CHECK(packet2.getSize() == 7 * 3 * sizeof(float) + 2 * sizeof(Eigen::Index));
	auto matRes2 = dp.deserialize<Eigen::Matrix<double, -1, -1>>(packet2);
	CHECK(matRes2.cast<float>() == mat.cast<float>());

	Eigen::Matrix<double, 8, 6> mat2 = Eigen::Matrix<double, 8, 6>::Random();
	Packet packet3 = dp.serialize(mat2);
	CHECK(packet3.getSize() == 8 * 6 * sizeof(float));
	auto matRes3 = dp.deserialize<Eigen::Matrix<double, 8, 6>>(packet3);
	CHECK(matRes3.cast<float>() == mat2.cast<float>());
}

TEST_CASE("std::array serialization")
{
	std::array<int, 3> arr = {1, 2, 3};
	DataPresentation dp;
	Packet packet = dp.serialize(arr);
	auto arrRes = dp.deserialize<std::array<int, 3>>(packet);
	CHECK(arrRes.size() == 3);
	CHECK(arrRes[0] == 1);
	CHECK(arrRes[1] == 2);
	CHECK(arrRes[2] == 3);
	std::array<std::string, 2> arrStr = {"Hello", "World"};
	Packet packetStr = dp.serialize(arrStr);
	auto arrStrRes = dp.deserialize<std::array<std::string, 2>>(packetStr);
	CHECK(arrStrRes.size() == 2);
	CHECK(arrStrRes[0] == "Hello");
	CHECK(arrStrRes[1] == "World");

}