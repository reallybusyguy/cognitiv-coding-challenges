#include "catch.hpp"
#include "Person.hpp"
#include "base.hpp"
#include "DNA_Stream.hpp"
#include "Chromosome_Comparison.hpp"

#include <cstddef>
#include <vector>
#include <array>

using std::byte;
using std::vector;
using std::array;

TEST_CASE("Compare two people", "[person]")
{
    string s1 = "GGGTTAGGGTTAGGGTTAGGGTAGCGAATATATTTAGGGTTAGGGTTAGGGTTAGGG";
    string s2 = "GGGTTAGGGTTAGGGTTAGGGTAACGACTGTATTTAGGGTTAGGGTTAGGGTTA";
    vector<byte> data1 = dna::ConvertToData(s1);
    vector<byte> data2 = dna::ConvertToData(s2);

    dna::DNA_Stream stream1(data1);
    dna::DNA_Stream stream2(data2);

    array<dna::DNA_Stream, 23> chroms1 = { stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1,
                                           stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1, stream1,
                                           stream1, stream1, stream1 };
    array<dna::DNA_Stream, 23> chroms2 = { stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2,
                                           stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2, stream2,
                                           stream2, stream2, stream2 };

    dna::Person person1(chroms1);
    dna::Person person2(chroms2);

    REQUIRE(person1.chromosomes() == 23);
    REQUIRE(person2.chromosomes() == 23);
    REQUIRE(person1.IsSameSexAs(person2));

    vector<dna::Chromosome_Comparison> comparisons = person1.Compare(person2);

    REQUIRE(comparisons.size() == 23);

    for (int i=0; i<23; i++)
    {
        REQUIRE(comparisons[i].chromosome == i);
        
        REQUIRE(comparisons[i].transformations.size() == 3);

        REQUIRE(comparisons[i].transformations[0].index == 23);
        REQUIRE(comparisons[i].transformations[0].type == dna::SUBSTITUTION);
        REQUIRE(comparisons[i].transformations[0].s1 == "G");
        REQUIRE(comparisons[i].transformations[0].s2 == "A");

        REQUIRE(comparisons[i].transformations[1].index == 27);
        REQUIRE(comparisons[i].transformations[1].type == dna::SUBSTITUTION);
        REQUIRE(comparisons[i].transformations[1].s1 == "A");
        REQUIRE(comparisons[i].transformations[1].s2 == "C");

        REQUIRE(comparisons[i].transformations[2].index == 29);
        REQUIRE(comparisons[i].transformations[2].type == dna::SUBSTITUTION);
        REQUIRE(comparisons[i].transformations[2].s1 == "A");
        REQUIRE(comparisons[i].transformations[2].s2 == "G");

        // Verify that if we apply the transformations to s1, we get s2.
        // Note that we need to ignore the last three additional telomere characters in s1.
        string transformedS1 = dna::applyTransformations(s1.substr(0, s1.length() - 3), comparisons[i].transformations);
        REQUIRE(transformedS1 == s2);
    }
}
