//
// Created by mirco on 30.08.20.
//
#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Utilities/EnumMap.hpp>

namespace
{

enum class TestEnum
{
	TEST1,
	TEST2,
	TEST3
};

ENUMMAP_INIT(TestEnum,
			 {
				 { TestEnum::TEST1, "test1" },
				 { TestEnum::TEST2, "test2" },
				 { TestEnum::TEST3, "test3" }
			 });
}


TEST_CASE("EnumMap Range Test")
{
	int k = 0;
	for (const auto&[e, string]:EnumMap<TestEnum>::getInstance())
	{
		if (k == 0)
		{
			CHECK(e == TestEnum::TEST1);
			CHECK(string == "test1");
		}
		else if (k == 1)
		{
			CHECK(e == TestEnum::TEST2);
			CHECK(string == "test2");
		}
		else if (k == 2)
		{
			CHECK(e == TestEnum::TEST3);
			CHECK(string == "test3");
		}
		++k;
	}
}