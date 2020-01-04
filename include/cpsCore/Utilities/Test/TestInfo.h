/**
 * @file TestInfo.h
 * @date Oct 2, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef TEST_TESTINFO_H_
#define TEST_TESTINFO_H_
#include <unistd.h>
#include <string>

#ifdef CHECK
#undef CHECK
#endif
#include <catch2/catch.hpp>

class TestInfo
{
public:

	static TestInfo&
	getInstance()
	{
		static TestInfo instance;
		return instance;
	}

	void
	initialize(int argc, char** argv);

	std::string testDir;

private:
	TestInfo() = default;

};

namespace test_info
{
std::string
test_dir();
}



#endif /* TEST_TESTINFO_H_ */
