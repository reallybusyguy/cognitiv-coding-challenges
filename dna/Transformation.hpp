#pragma once

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::ostream;

namespace dna
{
    enum TransformType
    {
        INSERTION,
        DELETION,
        SUBSTITUTION
    };

    struct Transformation
    {
        size_t index;
        TransformType type;
        string s1;            // For insertions, deletions, substitutions
        string s2;            // For substitutions

        Transformation(size_t ind, TransformType t, const string& str1);
        Transformation(size_t ind, TransformType t, const string& str1, const string& str2);
    };

    ostream& operator << (ostream& ostr, const Transformation& t);

    // Convert the given string to the output string via the given transformations.
    string applyTransformations(const string& str, const vector<Transformation>& transformations);
}
