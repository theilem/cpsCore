/*
 * JsonPopulatorTest.cpp
 *
 *  Created on: Aug 27, 2019
 *      Author: mirco
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include <cpsCore/Configuration/JsonPopulator.h>
#include <fstream>
#include <cpsCore/Utilities/Scheduler/IScheduler.h>
#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>
#include <cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h>

namespace
{
    struct ParamsNested
    {
        Parameter<float> p1 =
            {2.5, "p1", true};
        Parameter<std::string> p2 =
            {"test", "p2", false};
        Parameter<uint64_t> p3 =
            {100, "p3", true};

        template <class Configurator>
        void
        configure(Configurator& c)
        {
            c & p1;
            c & p2;
            c & p3;
        }
    };

    struct Params
    {
        Parameter<float> p1 =
            {1.0, "p1", true};
        Parameter<std::vector<int>> p2 =
            {{7, 8, 9}, "p2", true};
        Parameter<ParamsNested> p3 =
        {
            {}, "p3", false
        };

        template <class Configurator>
        inline void
        configure(Configurator& c)
        {
            c & p1;
            c & p2;
            c & p3;
        }
    };

    class Test : public ConfigurableObject<Params>
    {
    public:
        static constexpr const char* const typeId = "test";
    };

    class Test2 : public ConfigurableObject<Params>
    {
    public:
        static constexpr const char* const typeId = "test2";

        bool
        configure(const Configuration& config)
        {
            PropertyMapper pm(config);

            configureParams(pm);

            return pm.map();
        }

        template <typename Config>
        void
        configureParams(Config& c)
        {
            params.configure(c);

            ParameterRef<Test> ref(test,
                                   {}, "sub_test", true);
            c & ref;
        }

    private:
        Test test;
    };

    using TestHelper = StaticHelper<Test, Test2>;


    struct AngleParams
    {
        Parameter<Angle<FloatingType>> angle = {Angle<FloatingType>(45.0), "angle", true};

        template <class Configurator>
        void
        configure(Configurator& c)
        {
            c & angle;
        }
    };
    class AngleTest : public ConfigurableObject<AngleParams>
    {
    public:
        static constexpr const char* const typeId = "angle_test";
    };
}

TEST_CASE("Json Populator Test 1")
{
    JsonPopulator pop;
    pop.populate<Test>();

    std::string correct =
        "{\n"
        "    \"test\": {\n"
        "        \"p1\": 1.0,\n"
        "        \"p2\": [\n"
        "            7,\n"
        "            8,\n"
        "            9\n"
        "        ],\n"
        "        \"p3\": {\n"
        "            \"p1\": 2.5,\n"
        "            \"p2\": \"test\",\n"
        "            \"p3\": 100\n"
        "        }\n"
        "    }\n"
        "}";

    CHECK(correct == pop.getString());
}

TEST_CASE("Json Populator Test 2")
{
    REQUIRE(has_configure_params<Test2>::value);
    JsonPopulator pop;
    pop.populate<Test2>();

    std::string correct =
        "{\n"
        "    \"test2\": {\n"
        "        \"p1\": 1.0,\n"
        "        \"p2\": [\n"
        "            7,\n"
        "            8,\n"
        "            9\n"
        "        ],\n"
        "        \"p3\": {\n"
        "            \"p1\": 2.5,\n"
        "            \"p2\": \"test\",\n"
        "            \"p3\": 100\n"
        "        },\n"
        "        \"sub_test\": {\n"
        "            \"p1\": 1.0,\n"
        "            \"p2\": [\n"
        "                7,\n"
        "                8,\n"
        "                9\n"
        "            ],\n"
        "            \"p3\": {\n"
        "                \"p1\": 2.5,\n"
        "                \"p2\": \"test\",\n"
        "                \"p3\": 100\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";

    CHECK(correct == pop.getString());
}

TEST_CASE("Json Populator Test 3")
{
    auto pop = JsonPopulator::populateContainer<TestHelper>();

    std::string correct =
        "{\n"
        "    \"test\": {\n"
        "        \"p1\": 1.0,\n"
        "        \"p2\": [\n"
        "            7,\n"
        "            8,\n"
        "            9\n"
        "        ],\n"
        "        \"p3\": {\n"
        "            \"p1\": 2.5,\n"
        "            \"p2\": \"test\",\n"
        "            \"p3\": 100\n"
        "        }\n"
        "    },\n"
        "    \"test2\": {\n"
        "        \"p1\": 1.0,\n"
        "        \"p2\": [\n"
        "            7,\n"
        "            8,\n"
        "            9\n"
        "        ],\n"
        "        \"p3\": {\n"
        "            \"p1\": 2.5,\n"
        "            \"p2\": \"test\",\n"
        "            \"p3\": 100\n"
        "        },\n"
        "        \"sub_test\": {\n"
        "            \"p1\": 1.0,\n"
        "            \"p2\": [\n"
        "                7,\n"
        "                8,\n"
        "                9\n"
        "            ],\n"
        "            \"p3\": {\n"
        "                \"p1\": 2.5,\n"
        "                \"p2\": \"test\",\n"
        "                \"p3\": 100\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}";

    CHECK(correct == pop.getString());
}

TEST_CASE("Json Populator Test 4")
{
    JsonPopulator pop;
    pop.populate<AngleTest>();
    std::string correct =
        "{\n"
        "    \"angle_test\": {\n"
        "        \"angle\": 45.0\n"
        "    }\n"
        "}";
    CHECK(correct == pop.getString());
}
