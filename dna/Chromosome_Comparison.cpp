#include "Chromosome_Comparison.hpp"

namespace dna
{
    Chromosome_Comparison::Chromosome_Comparison() : chromosome(0)
    {
    }
    
    Chromosome_Comparison::Chromosome_Comparison(const Chromosome_Comparison& other) : chromosome(other.chromosome), transformations(other.transformations)
    {
    }
    
    Chromosome_Comparison::Chromosome_Comparison(Chromosome_Comparison&& other) noexcept
    {
        chromosome = other.chromosome;
        transformations = std::move(other.transformations);
    }

    Chromosome_Comparison& Chromosome_Comparison::operator= (const Chromosome_Comparison& other)
    {
        if (this != &other)
        {
            chromosome = other.chromosome;
            transformations = other.transformations;
        }
        return *this;
    }

    Chromosome_Comparison& Chromosome_Comparison::operator= (Chromosome_Comparison&& other) noexcept
    {
        if (this != &other)
        {
            chromosome = other.chromosome;
            transformations = std::move(other.transformations);
        }
        return *this;
    }
        
}