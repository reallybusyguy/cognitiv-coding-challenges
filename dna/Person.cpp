#include "Person.hpp"
#include "Chromosome_Comparer.hpp"

#include <thread>

namespace dna
{
    static const std::size_t MALE_MAXIMUM_LENGTH = 100 * 1000 * 1000;

    Person::Person(const array<DNA_Stream, NUM_CHROMS>& chromosomeData, std::size_t chunkSize)
    {
        if (chromosomeData.size() != chroms_.size())
            throw std::invalid_argument("chromosome data does not match expected size");

        std::size_t index = 0;
        auto it = chromosomeData.begin();
        for (; index < chromosomeData.size() && it != chromosomeData.end(); ++index, ++it)
            chroms_[index] = DNA_Stream(*it);
    }

    DNA_Stream& Person::chromosome(std::size_t chromosomeIndex)
    {
        if (chromosomeIndex >= chroms_.size())
            throw std::invalid_argument("index is out of range for the number of chromosomes available");

        return chroms_[chromosomeIndex];
    }

    std::size_t Person::chromosomes() const
    {
        return chroms_.size();
    }

    static void CompareChromosomes(int number, DNA_Stream& c1, DNA_Stream& c2, Chromosome_Comparison& result)
    {
        Chromosome_Comparer comparer(number, c1, c2);
        result = comparer.Compare();
    }

    vector<Chromosome_Comparison> Person::Compare(Person& other)
    {
        vector<Chromosome_Comparison> comparisons;

        // We want to compare the sex chromosomes only if the two Persons are of the
        // same sex.  If they are not of the same sex, then ignore the sex chromosomes.
        int numChromosomes = IsSameSexAs(other) ? NUM_CHROMS : NUM_CHROMS-1;
        comparisons.resize(numChromosomes);

        // Compare each corresponding pair of chromosomes, each comparison in a separate thread.
        vector<std::thread> threads;
        threads.reserve(numChromosomes);
        for (int i = 0; i < numChromosomes; i++)
        {
            std::thread th(CompareChromosomes, i,
                            std::ref(chromosome(i)),
                            std::ref(other.chromosome(i)),
                            std::ref(comparisons[i]));
            threads.push_back(std::move(th));
        }

        // Now wait for the completion of the computations.
        for (int i = 0; i < numChromosomes; i++)
        {
            threads[i].join();
        }

        return comparisons;
    }

    bool Person::IsSameSexAs(Person& other)
    {
        // The last chromosome is a sex chromosome.  It is either a male (ie, Y) chromosome,
        // or it is a female (ie, X) chromosome.  They notably differ by their length.
        const std::size_t SEX_CHROM = NUM_CHROMS-1;

        bool thisIsMale = IsMale(chromosome(SEX_CHROM));
        bool otherIsMale = IsMale(other.chromosome(SEX_CHROM));

        return (thisIsMale && otherIsMale) || (!thisIsMale && !otherIsMale);
    }

    bool Person::IsMale(DNA_Stream& chrom)
    {
        // The male chromosome is roughly 57 million pairs long, and the female chromosome
        // is roughly 156 million pairs long.  The length should give an indication as to
        // whether the two people are the same sex.  We establish a length threshold somewhere
        // between those estimated typical lengths.
        return chrom.size() <= MALE_MAXIMUM_LENGTH;
    }
}
