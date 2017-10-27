#include "bisect/bicla.h"

#include <array>
#pragma warning(push)
#pragma warning(disable: 4996)
#include "catch.hpp"
#pragma warning(pop)
using namespace bisect::bicla;

//------------------------------------------------------------------------------

SCENARIO("argument parsing")
{
    GIVEN("a config with three string arguments")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::string s3;
        };

        WHEN("we provide three arguments")
        {
			const std::array<const char*, 4> argv = { "program name", "string 1", "string 2", "string 3" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(static_cast<int>(argv.size()), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::s2, "s 2"),
                    argument(&config::s3, "s 3")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(config.s3 == "string 3");
            }
        }

        WHEN("we only provide two arguments")
        {
			const std::array<const char*, 3> argv = { "program name", "string 1", "string 2" };

            THEN("parsing fails")
            {
                const auto[parse_result, _] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::s2, "s 2"),
                    argument(&config::s3, "s 3")
                );
				static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }

        WHEN("we provide four arguments")
        {
			const std::array<const char*, 5> argv = { "program name", "string 1", "string 2", "string 3", "extra" };

            THEN("parsing fails")
            {
                const auto[parse_result, _] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::s2, "s 2"),
                    argument(&config::s3, "s 3")
                );
				static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }
    }

    GIVEN("a config with three string arguments, one of which is optional")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::optional<std::string> s3;
        };

        WHEN("we provide three arguments")
        {
			const std::array<const char*, 4> argv = { "program name", "string 1", "string 2", "string 3" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::s2, "s 2"),
                    argument(&config::s3, "s 3")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(config.s3 == "string 3");
            }
        }

        WHEN("we provide only two arguments")
        {
			const std::array<const char*, 3> argv = { "program name", "string 1", "string 2" };

            THEN("the first two are correctly parsed and the last one is not set")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::s2, "s 2"),
                    argument(&config::s3, "s 3")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(!config.s3);
            }
        }
    }
}

SCENARIO("heterogenous argument parsing 2")
{
    GIVEN("a config with four arguments, two of which are optional")
    {
        struct config
        {
            std::string s1;
            int i1;
            std::optional<std::string> s3;
            std::optional<int> i2;
        };

        WHEN("we provide three arguments")
        {
			const std::array<const char*, 4> argv = { "program name", "string 1", "2", "string 3" };

            THEN("the first three are correctly parsed and the last one is not set")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::i1, "i 1"),
                    argument(&config::s3, "s 3")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i1 == 2);
                REQUIRE(config.s3 == "string 3");
                REQUIRE(!config.i2);
            }
        }

        WHEN("we provide four arguments")
        {
			const std::array<const char*, 5> argv = { "program name", "string 1", "2", "string 3", "42" };

            THEN("all of them are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    argument(&config::s1, "s 1"),
                    argument(&config::i1, "i 1"),
                    argument(&config::s3, "s 3"),
                    argument(&config::i2, "i 2")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i1 == 2);
                REQUIRE(config.s3 == "string 3");
                REQUIRE(config.i2 == 42);
            }
        }
    }
}

SCENARIO("messages")
{
    GIVEN("a config with three string arguments")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::string s3;
        };

        WHEN("we parse")
        {
			const std::array<const char*, 3> argv = { "program name", "string 1", "string 2" };

            const auto[parse_result, _] = parse(int(static_cast<int>(argv.size())), argv.data(),
                argument(&config::s1, "s 1", "a string named 1"),
                argument(&config::s2, "s 2", "a string named 2"),
                argument(&config::s3, "s 3", "a string named 3")
            );

			static_cast<void>(_);

            THEN("the usage message is correct")
            {
                const auto expected_usage_message = "<s 1> <s 2> <s 3>";
                REQUIRE(parse_result.usage_message == expected_usage_message);
            }

            THEN("the parameters description is correct")
            {
                const auto expected_parameters_description = detail::svector
                {
                    "s 1: a string named 1",
                    "s 2: a string named 2",
                    "s 3: a string named 3",
                };
                REQUIRE(parse_result.parameters_description == expected_parameters_description);
            }
        }
    }

    GIVEN("a config with three arguments, two of which are optional")
    {
        struct config
        {
            std::string s1;
            std::optional<std::string> s2;
            std::optional<std::string> s3;
        };

        WHEN("we parse")
        {
			const std::array<const char*, 1> argv = { "program name" };

            const auto[parse_result, _] = parse(int(static_cast<int>(argv.size())), argv.data(),
                argument(&config::s1, "s 1", "a string named 1"),
                argument(&config::s2, "s 2", "a string named 2"),
                argument(&config::s3, "s 3", "a string named 3")
            );
			static_cast<void>(_);

            THEN("the usage message is correct")
            {
                const auto expected_usage_message = "<s 1> [<s 2>] [<s 3>]";
                REQUIRE(parse_result.usage_message == expected_usage_message);
            }

            THEN("the parameters description is correct")
            {
                const auto expected_parameters_description = detail::svector
                {
                    "s 1: a string named 1",
                    "s 2: a string named 2",
                    "s 3: a string named 3"
                };
                REQUIRE(parse_result.parameters_description == expected_parameters_description);
            }
        }
    }
}

SCENARIO("option parsing")
{
    GIVEN("a config with one string option")
    {
        struct config
        {
            std::string s1;
        };

        WHEN("we provide suitable arguments")
        {
			const std::array<const char*, 3> argv = { "program name", "-s", "string 1" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    option(&config::s1, "s", "s 1")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
            }
        }

        WHEN("we provide extra arguments")
        {
			const std::array<const char*, 4> argv = { "program name", "-s", "string 1", "extra" };

            THEN("parsing fails")
            {
                const auto[parse_result, _] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    option(&config::s1, "s", "s 1")
                );
				static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }
    }

    GIVEN("a config with three options")
    {
        struct config
        {
            std::string s1;
            int i2;
            std::optional<float> f3;
        };

        WHEN("we provide suitable arguments")
        {
			const std::array<const char*, 7> argv = { "program name", "-s", "string 1", "-i", "123", "-f", "2.2" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    option(&config::s1, "s", "a string"),
                    option(&config::i2, "i", "an int"),
                    option(&config::f3, "f", "a float")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }
    }
}

SCENARIO("option and argument parsing")
{
    GIVEN("a config with three options")
    {
        struct config
        {
            std::string s1;
            int i2;
            std::optional<float> f3;
        };

        WHEN("we provide the arguments in the natural order")
        {
			const std::array<const char*, 5> argv = { "program name", "-s", "string 1", "123", "2.2" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    option(&config::s1, "s", "a string"),
                    argument(&config::i2, "an int"),
                    argument(&config::f3, "a float")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }

        WHEN("we change the order")
        {
			const std::array<const char*, 5> argv = { "program name", "123", "-s", "string 1", "2.2" };

            THEN("they are correctly parsed")
            {
                const auto[parse_result, config] = parse(int(static_cast<int>(argv.size())), argv.data(),
                    option(&config::s1, "s", "a string"),
                    argument(&config::i2, "an int"),
                    argument(&config::f3, "a float")
                );

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }
    }
}