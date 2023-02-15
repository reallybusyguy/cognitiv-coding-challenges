#pragma once

#include "sequence_buffer.hpp"
#include "DNA_Stream.hpp"
#include "Chromosome_Comparison.hpp"

#include <array>

#define NUM_CHROMS 23

using std::array;

namespace dna
{

template<typename T>
concept HelixStream = requires(T a) {
    { a.seek(1000L) };
    { a.read() }; // -> std::convertible_to<sequence_buffer<ByteBuffer>>;
    { a.size() } -> std::convertible_to<std::size_t>;
};

template<typename T>
concept Organism = requires(T a) {
    { a.chromosome(1) };
    { a.chromosomes() } -> std::convertible_to<std::size_t>;
};

class Person
{
    array<DNA_Stream, NUM_CHROMS> chroms_;
public:

    Person(const array<DNA_Stream, NUM_CHROMS>& chromosomeData, std::size_t chunkSize = 512);

    DNA_Stream& chromosome(std::size_t chromosomeIndex);

    std::size_t chromosomes() const;

    vector<Chromosome_Comparison> Compare(Person& other);
    bool IsSameSexAs(Person& other);

private:
    bool IsMale(DNA_Stream& chrom);
};

}

