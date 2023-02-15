#include "catch.hpp"
#include <string>
#include "String_Comparer.hpp"

using std::string;


TEST_CASE("Compare two empty strings", "[strings]")
{
    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare("", "");

    REQUIRE(transformations.empty());
}

TEST_CASE("First string is empty", "[strings]")
{
    string s1;
    string s2 = "Happy Jack";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::INSERTION);
    REQUIRE(transformations[0].s1 == s2);

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Second string is empty", "[strings]")
{
    string s1 = "Happy Jack";
    string s2;

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::DELETION);
    REQUIRE(transformations[0].s1 == s1);

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Single insertion in front", "[strings]")
{
    string s1 = "Red";
    string s2 = "Big Red";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::INSERTION);
    REQUIRE(transformations[0].s1 == "Big ");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Multiple insertions", "[strings]")
{
    string s1 = "Johnny eats the red apple";
    string s2 = "Little Johnny eats the big red apple";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 2);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::INSERTION);
    REQUIRE(transformations[0].s1 == "Little ");

    REQUIRE(transformations[1].index == 23);
    REQUIRE(transformations[1].type == dna::INSERTION);
    REQUIRE(transformations[1].s1 == "big ");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Single deletion in front", "[strings]")
{
    string s1 = "Big Red";
    string s2 = "Red";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::DELETION);
    REQUIRE(transformations[0].s1 == "Big ");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Multiple deletions", "[strings]")
{
    string s1 = "canXXXtelYYYoupe";
    string s2 = "canteloupe";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 2);

    REQUIRE(transformations[0].index == 3);
    REQUIRE(transformations[0].type == dna::DELETION);
    REQUIRE(transformations[0].s1 == "XXX");

    REQUIRE(transformations[1].index == 6);
    REQUIRE(transformations[1].type == dna::DELETION);
    REQUIRE(transformations[1].s1 == "YYY");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Substitution in front", "[strings]")
{
    string s1 = "back";
    string s2 = "pack";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::SUBSTITUTION);
    REQUIRE(transformations[0].s1 == "b");
    REQUIRE(transformations[0].s2 == "p");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Substitution at end", "[strings]")
{
    string s1 = "pass";
    string s2 = "past";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 1);

    REQUIRE(transformations[0].index == 3);
    REQUIRE(transformations[0].type == dna::SUBSTITUTION);
    REQUIRE(transformations[0].s1 == "s");
    REQUIRE(transformations[0].s2 == "t");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("All types of transformations", "[strings]")
{
    string s1 = "watermelody";
    string s2 = "ripe watermelon";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 3);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::INSERTION);
    REQUIRE(transformations[0].s1 == "ripe ");

    REQUIRE(transformations[1].index == 14);
    REQUIRE(transformations[1].type == dna::DELETION);
    REQUIRE(transformations[1].s1 == "d");

    REQUIRE(transformations[2].index == 14);
    REQUIRE(transformations[2].type == dna::SUBSTITUTION);
    REQUIRE(transformations[2].s1 == "y");
    REQUIRE(transformations[2].s2 == "n");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Strings are same", "[strings]")
{
    string s1 = "Identical";
    string s2 = s1;

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.empty());
}

TEST_CASE("First string contains the second string", "[strings]")
{
    string s1 = "Your happy clown";
    string s2 = "happy";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 2);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::DELETION);
    REQUIRE(transformations[0].s1 == "Your ");

    REQUIRE(transformations[1].index == 5);
    REQUIRE(transformations[1].type == dna::DELETION);
    REQUIRE(transformations[1].s1 == " clown");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Second string contains the first string", "[strings]")
{
    string s1 = "happy";
    string s2 = "Your happy clown";

    dna::String_Comparer comparer;
    vector<dna::Transformation> transformations = comparer.Compare(s1, s2);

    REQUIRE(transformations.size() == 2);

    REQUIRE(transformations[0].index == 0);
    REQUIRE(transformations[0].type == dna::INSERTION);
    REQUIRE(transformations[0].s1 == "Your ");

    REQUIRE(transformations[1].index == 10);
    REQUIRE(transformations[1].type == dna::INSERTION);
    REQUIRE(transformations[1].s1 == " clown");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, transformations);
    REQUIRE(transformedS1 == s2);
}
