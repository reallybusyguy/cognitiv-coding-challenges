#pragma once

#include <vector>
#include "Transformation.hpp"

using std::vector;

namespace dna
{

    struct Chromosome_Comparison
    {
        int chromosome = 0;
        vector<Transformation> transformations;

        Chromosome_Comparison();
        Chromosome_Comparison(const Chromosome_Comparison& other);
        Chromosome_Comparison(Chromosome_Comparison&& other) noexcept;
        Chromosome_Comparison& operator= (const Chromosome_Comparison& other);
        Chromosome_Comparison& operator= (Chromosome_Comparison&& other) noexcept;
    };

}
