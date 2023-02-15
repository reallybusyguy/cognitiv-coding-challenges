#pragma once

#include <string>
#include <vector>
#include "DNA_Stream.hpp"
#include "Transformation.hpp"
#include "Chromosome_Comparison.hpp"

using std::string;
using std::vector;

namespace dna
{
    class Chromosome_Comparer
    {
        int num_;
        DNA_Stream& c1_;
        DNA_Stream& c2_;
        int trailingNonTelomereCharsOnC1_ = 0;
        int trailingNonTelomereCharsOnC2_ = 0;
        size_t bytesReadFromC1_ = 0;

    public:
        Chromosome_Comparer(int number, DNA_Stream& c1, DNA_Stream& c2);
        Chromosome_Comparison Compare();

    private:
        string unpackChunk(const sequence_buffer<byte_view>& bytes);
        int initializeStream(DNA_Stream& stream, bool trackBytesRead);
        bool findFullTelomeresInChars(const string& chars,
            size_t startPoint,
            const string& previous_chars,
            string& nextPrefix);
        string getNextChunkOfChars(DNA_Stream& stream, int& trailingTelomereChars);
        bool shouldSpliceAt(const vector<Transformation>& transforms, size_t i);
    };
}
