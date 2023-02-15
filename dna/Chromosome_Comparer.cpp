#include "Chromosome_Comparer.hpp"
#include "sequence_buffer.hpp"
#include "base.hpp"
#include "String_Comparer.hpp"

#include <iostream>
#include <sstream>
#include <vector>

using std::vector;
using std::ostringstream;

namespace dna
{
    static string TELOMERE = "TTAGGG";
    static vector<string> LEADING_TELOMERE_FRAGMENTS{ "TAGGG", "AGGG", "GGG", "GG", "G" };
    static vector<string> TAILING_TELOMERE_FRAGMENTS{ "TTAGG", "TTAG", "TTA", "TT", "T" };

    Chromosome_Comparer::Chromosome_Comparer(int number, DNA_Stream& c1, DNA_Stream& c2) :
        num_(number), c1_(c1), c2_(c2)
    {
    }

    Chromosome_Comparison Chromosome_Comparer::Compare()
    {
        Chromosome_Comparison comparison;
        comparison.chromosome = num_;

        vector<Transformation> transforms;

        trailingNonTelomereCharsOnC1_ = initializeStream(c1_, true);
        trailingNonTelomereCharsOnC2_ = initializeStream(c2_, false);

        size_t c1BytesSoFar = bytesReadFromC1_;

        // Iterate through the chunks from each of the chromosomes.
        while (!c1_.atEnd() && !c2_.atEnd())
        {
            string c1String = getNextChunkOfChars(c1_, trailingNonTelomereCharsOnC1_);
            string c2String = getNextChunkOfChars(c2_, trailingNonTelomereCharsOnC2_);

            trailingNonTelomereCharsOnC1_ = 0;
            trailingNonTelomereCharsOnC2_ = 0;

            String_Comparer stringComparer;
            transforms = stringComparer.Compare(c1String, c2String);

            // Now we need to update the index for each of the transforms to offset them
            // by the bytes we've read so far from the first chromosome.  That way, all
            // indices will be relative to the start of the first chromosome.
            for (auto& t : transforms)
            {
                t.index += c1BytesSoFar;
            }

            // Append the transforms to those that have been discovered so far from earlier chunks.
            comparison.transformations.insert(comparison.transformations.end(), transforms.begin(), transforms.end());

            // Update the bytes read so far.
            c1BytesSoFar += c1String.size();
        }

        if (c1_.atEnd() && !c2_.atEnd())
        {
            // c1 is done.  Get the remaining chunks from c2 and mark them as deletions.
            string remainingChars;
            while (!c2_.atEnd())
            {
                remainingChars += getNextChunkOfChars(c2_, trailingNonTelomereCharsOnC2_);
            }

            // Put the remaining characters in an insertion transformation.
            // Append that insertion to the accumulated transformations.
            comparison.transformations.emplace_back(Transformation(c1BytesSoFar, INSERTION, remainingChars));
        }
        else if (!c1_.atEnd() && c2_.atEnd())
        {
            // c2 is done.  Get the remaining chunks from c1 and mark them as insertions.
            string remainingChars;
            while (!c1_.atEnd())
            {
                remainingChars += getNextChunkOfChars(c1_, trailingNonTelomereCharsOnC1_);
            }

            // Put the remaining characters in a deletion transformation.
            // Append that insertion to the accumulated transformations.
            comparison.transformations.emplace_back(Transformation(c1BytesSoFar, DELETION, remainingChars));
        }

        // Now do some post-processing to make sure that we didn't
        // mis-identify transformations due to the mis-alignment at the
        // chunk boundaries.
        if (!comparison.transformations.empty())
        {
            size_t last = comparison.transformations.size() - 1;
            size_t i = 0;
            while (i < last)
            {
                // If the two adjacent transformations cancel each other out, then remove them.
                if (shouldSpliceAt(comparison.transformations, i))
                {
                    vector<Transformation>::iterator first = comparison.transformations.begin() + i;
                    vector<Transformation>::iterator second = first + 1;
                    comparison.transformations.erase(first, second);
                    last -= 2;
                }
                i++;
            }
        }

        return comparison;
    }

    string Chromosome_Comparer::unpackChunk(const sequence_buffer<byte_view>& bytes)
    {
        // Convert each byte in the chunk to characters to build a string.
        std::ostringstream ostr;
        for (size_t i=0; i<bytes.size(); i++)
        {
            ostr << to_char(bytes[i]);
        }
        return ostr.str();
    }

    int Chromosome_Comparer::initializeStream(DNA_Stream& stream, bool trackBytesRead)
    {
        size_t charsRead = 0;
        sequence_buffer<byte_view> currentBytes = stream.read();
        charsRead += currentBytes.size();
        string chars = unpackChunk(currentBytes);

        // Now look for telomere fragments at the beginning of the string.
        size_t telomereFragmentSize = 0;
        for (const auto& fragment : LEADING_TELOMERE_FRAGMENTS)
        {
            if (chars.starts_with(fragment))
            {
                telomereFragmentSize = fragment.size();
                break;
            }
        }

        // Now start advancing past the full telomeres in this chunk.
        string nextPrefix;
        string currentPrefix;
        bool searchForMore = findFullTelomeresInChars(chars, telomereFragmentSize, currentPrefix, nextPrefix);
        while (searchForMore)
        {
            // Read the next chunk from the stream and unpack it.
            currentBytes = stream.read();
            charsRead += currentBytes.size();
            chars = unpackChunk(currentBytes);

            // Note the remaining characters from the previous chunk.
            currentPrefix = nextPrefix;
            nextPrefix = "";
            telomereFragmentSize = 0;

            searchForMore = findFullTelomeresInChars(chars, telomereFragmentSize, currentPrefix, nextPrefix);
        }

        // The nextPrefix is now the start of the string that needs to be compared.
        // It's possible that the last leading telomere ended in the middle of a byte.
        size_t bytesRemainingInChunk = nextPrefix.size();
        size_t endOfTelomeres = charsRead - bytesRemainingInChunk;
        auto offset = endOfTelomeres / packed_size::value;
        int charsToIgnoreFromLastTelomere = endOfTelomeres % packed_size::value;
        stream.seek(offset);

        if (trackBytesRead)
        {
            bytesReadFromC1_ = endOfTelomeres;
        }

        // The charsToIgnoreFromLastTelomere represent the number of characters to
        // ignore when we start comparing this chromosome with another.  If the
        // number is greater than zero, then these are the characters at the end of
        // the last telomere, which did not end on a byte boundary.
        // We reset the stream to the byte containing the end of the last telomere
        // and note how many characters of that telomere to ignore in the comparison.
        return charsToIgnoreFromLastTelomere;
    }

    bool Chromosome_Comparer::findFullTelomeresInChars(
        const string& chars,
        size_t startPoint,
        const string& previousChars,
        string& nextPrefix)
    {
        string charsToSearch = previousChars.empty() ? chars : previousChars + chars;

        // Advance past all telomeres found.
        while (charsToSearch.find(TELOMERE, startPoint) == startPoint)
        {
            startPoint += TELOMERE.size();
        }

        // Now see if there's much left in this chunk.
        // If the amount left is less than the size of a telomere, then
        // we should continue looking for more telomeres in the remaining
        // fragment plus the next chunk.
        auto remainingChars = charsToSearch.size() - startPoint;
        nextPrefix = charsToSearch.substr(startPoint, remainingChars);
        if (remainingChars > 0 && remainingChars < TELOMERE.size())
        {
            return true;
        }
        else if (remainingChars >= TELOMERE.size())
        {
            // There are no more telomeres.
            return false;
        }

        // The number of remaining chars is 0.  We're exactly at the end of the buffer,
        // so it's possible that there will be more telomeres in the next buffer.
        nextPrefix = "";
        return true;
    }

    string Chromosome_Comparer::getNextChunkOfChars(DNA_Stream& stream, int& trailingNonTelomereChars)
    {
        auto c1Chunk = stream.read();
        string charString = unpackChunk(c1Chunk);
        if (trailingNonTelomereChars > 0)
        {
            // Prepend the non-telomere characters that were found during
            // initialization of cthe stream.
            charString.erase(0, trailingNonTelomereChars);
            trailingNonTelomereChars = 0;
        }

        // Now see if there's a telomere in this chunk.
        auto telomereIndex = charString.find(TELOMERE);
        if (telomereIndex != string::npos)
        {
            // Found one.  Is it just a random sequence, or does it indicate
            // the start of the telomeres on the end of the chromosome?
            // See if we can find another one.
            auto remainingChars = charString.size() - (telomereIndex + TELOMERE.size());
            if (remainingChars >= TELOMERE.size())
            {
                if (charString.find(TELOMERE, telomereIndex + TELOMERE.size()))
                {
                    // We found another whole one.  Let's assume that this means
                    // that we have found the start of the ending telomeres.
                    // Truncate the string at the first found telomere and
                    // stop reading more chunks.
                    charString.erase(telomereIndex);
                    stream.advanceToEnd();
                }
            }
            else if (remainingChars > 0)
            {
                // The distance from the telomere to the end of the chunk is smaller than
                // the length of a telomere.  Let's see if we find a telomere fragment.
                string fragmentCandidate = TAILING_TELOMERE_FRAGMENTS[TELOMERE.size() - 1 - remainingChars];
                if (charString.find(fragmentCandidate, telomereIndex + TELOMERE.size()))
                {
                    // Found the telomere fragment.  Assume this means that we have reached
                    // the ending telomeres.
                    // Truncate the string at the first found telomere and
                    // stop reading more chunks.
                    charString.erase(telomereIndex);
                    stream.advanceToEnd();
                }
            }
            else
            {
                // The number of remaining characters is zero.  Thus the telomere
                // was found at the end of the buffer.  This could be a random sequence,
                // or there could be more telomeres at the start of the next buffer.
                // In that case, don't remove the telomere.  Perhaps in the next chunk
                // we will find more telomeres at the beginning of the string.  In that
                // case, not having removed this telomere will have been a mistake.
                // But if this is just a random sequence of characters and not a telomere,
                // then not removing it will have been the appropriate choice.
            }
        }
        return charString;
    }

    bool Chromosome_Comparer::shouldSpliceAt(const vector<Transformation>& transforms, size_t i)
    {
        // If we inserted a string at the end of one chunk and then deleted the same string
        // at the beginning of the next chunk, then these two adjacent transformations
        // cancel each other out.

        if (transforms[i].index == transforms[i + 1].index &&
            transforms[i].s1 == transforms[i + 1].s1)
        {
            if (transforms[i].type == INSERTION && transforms[i + 1].type == DELETION)
                return true;
            if (transforms[i].type == DELETION && transforms[i + 1].type == INSERTION)
                return true;
        }
            
        return false;
    }

}
