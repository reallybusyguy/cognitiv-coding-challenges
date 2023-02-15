#pragma once

#include <string>
#include <vector>
#include "Transformation.hpp"

using std::string;
using std::vector;

namespace dna
{
    class String_Comparer
    {
    public:
        String_Comparer() = default;

        // Return a vector of transformations needed to convert s1 to s2.
        // Note that the transformations are cumulative, from the start to the end.
        vector<Transformation> Compare(const string& s1, const string& s2) const;

    private:
        vector<vector<int>> buildLevenshteinTable(const string& s1, const string& s2) const;
        int getLevenshteinValue(int i, int j,
                                const string& s1, const string& s2,
                                const vector<vector<int>>& table) const;
        void reviseTransformations(vector<Transformation>& transformations) const;
        bool canBeMerged(const vector<Transformation>& transformations, size_t i) const;
    };
}