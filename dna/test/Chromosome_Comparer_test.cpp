#include "catch.hpp"
#include "base.hpp"
#include "DNA_Stream.hpp"
#include "Chromosome_Comparer.hpp"

#include <cstddef>
#include <vector>

using std::byte;
using std::vector;

TEST_CASE("Chromosomes are identical", "[chromosomes]")
{
    vector<byte> data = dna::ConvertToData("CACGTAACGCAT");

    dna::DNA_Stream stream1(data, 10);
    dna::DNA_Stream stream2(data, 10);

    dna::Chromosome_Comparer comparer(0, stream1, stream2);
    dna::Chromosome_Comparison comparison = comparer.Compare();

    REQUIRE(comparison.chromosome == 0);
    REQUIRE(comparison.transformations.empty());
}

TEST_CASE("No telomeres", "[chromosomes]")
{
    vector<byte> data1 = dna::ConvertToData("CACGTAACGCAT");
    vector<byte> data2 = dna::ConvertToData("CACGTCCCGCAT");

    dna::DNA_Stream stream1(data1, 10);
    dna::DNA_Stream stream2(data2, 10);

    dna::Chromosome_Comparer comparer(0, stream1, stream2);
    dna::Chromosome_Comparison comparison = comparer.Compare();

    REQUIRE(comparison.chromosome == 0);
    REQUIRE(comparison.transformations.size() == 1);

    REQUIRE(comparison.transformations[0].index == 5);
    REQUIRE(comparison.transformations[0].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[0].s1 == "AA");
    REQUIRE(comparison.transformations[0].s2 == "CC");
}

TEST_CASE("Different telomere fragments", "[chromosomes]")
{
    vector<byte> data1 = dna::ConvertToData("AGGGTAACGCAT");
    vector<byte> data2 = dna::ConvertToData("GGGATAACGCAT");

    dna::DNA_Stream stream1(data1, 10);
    dna::DNA_Stream stream2(data2, 10);

    dna::Chromosome_Comparer comparer(0, stream1, stream2);
    dna::Chromosome_Comparison comparison = comparer.Compare();

    REQUIRE(comparison.chromosome == 0);
    REQUIRE(comparison.transformations.size() == 1);

    REQUIRE(comparison.transformations[0].index == 4);
    REQUIRE(comparison.transformations[0].type == dna::INSERTION);
    REQUIRE(comparison.transformations[0].s1 == "A");
}

TEST_CASE("Telomeres cross chunk boundaries", "[chromosomes]")
{
    string s1 = "GGGTTAGGGTTAGGGTTAGGGTAACGCATAAC";
    string s2 = "GGGTTAGGGTTAGGGTTAGGGTAATTTACGCATAAC";
    vector<byte> data1 = dna::ConvertToData(s1);
    vector<byte> data2 = dna::ConvertToData(s2);

    // Make the chunks small so that we can see that the telomeres span multiple chunks.
    dna::DNA_Stream stream1(data1, 3);
    dna::DNA_Stream stream2(data2, 3);

    dna::Chromosome_Comparer comparer(0, stream1, stream2);
    dna::Chromosome_Comparison comparison = comparer.Compare();

    REQUIRE(comparison.chromosome == 0);
    REQUIRE(comparison.transformations.size() == 3);

    REQUIRE(comparison.transformations[0].index == 24);
    REQUIRE(comparison.transformations[0].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[0].s1 == "CGC");
    REQUIRE(comparison.transformations[0].s2 == "TTT");

    REQUIRE(comparison.transformations[1].index == 28);
    REQUIRE(comparison.transformations[1].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[1].s1 == "TAAC");
    REQUIRE(comparison.transformations[1].s2 == "CGCA");

    REQUIRE(comparison.transformations[2].index == 32);
    REQUIRE(comparison.transformations[2].type == dna::INSERTION);
    REQUIRE(comparison.transformations[2].s1 == "TAAC");

    // Verify that if we apply the transformations to s1, we get s2.
    string transformedS1 = dna::applyTransformations(s1, comparison.transformations);
    REQUIRE(transformedS1 == s2);
}

TEST_CASE("Tailing telomeres too", "[chromosomes]")
{
    string s1 = "GGGTTAGGGTTAGGGTTAGGGTAGCGAATATATTTAGGGTTAGGGTTAGGGTTAGGG";
    string s2 = "GGGTTAGGGTTAGGGTTAGGGTAACGACTGTATTTAGGGTTAGGGTTAGGGTTA";
    vector<byte> data1 = dna::ConvertToData(s1);
    vector<byte> data2 = dna::ConvertToData(s2);

    // Make the chunks small so that we can see that the telomeres span multiple chunks.
    dna::DNA_Stream stream1(data1, 4);
    dna::DNA_Stream stream2(data2, 4);

    dna::Chromosome_Comparer comparer(0, stream1, stream2);
    dna::Chromosome_Comparison comparison = comparer.Compare();

    REQUIRE(comparison.chromosome == 0);
    REQUIRE(comparison.transformations.size() == 3);

    REQUIRE(comparison.transformations[0].index == 23);
    REQUIRE(comparison.transformations[0].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[0].s1 == "G");
    REQUIRE(comparison.transformations[0].s2 == "A");

    REQUIRE(comparison.transformations[1].index == 27);
    REQUIRE(comparison.transformations[1].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[1].s1 == "A");
    REQUIRE(comparison.transformations[1].s2 == "C");

    REQUIRE(comparison.transformations[2].index == 29);
    REQUIRE(comparison.transformations[2].type == dna::SUBSTITUTION);
    REQUIRE(comparison.transformations[2].s1 == "A");
    REQUIRE(comparison.transformations[2].s2 == "G");

    // Verify that if we apply the transformations to s1, we get s2.
    // Note that we need to ignore the last three additional telomere characters in s1.
    string transformedS1 = dna::applyTransformations(s1.substr(0, s1.length()-3), comparison.transformations);
    REQUIRE(transformedS1 == s2);
}
