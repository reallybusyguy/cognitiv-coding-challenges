#include <algorithm>
#include "String_Comparer.hpp"

using std::min;
using std::swap;

namespace dna
{
    vector<Transformation> String_Comparer::Compare(const string& s1, const string& s2) const
    {
        vector<Transformation> transformations;

        if (s1.empty() && s2.empty()) {
            return transformations;
        }
        else if (s1.empty())
        {
            transformations.emplace_back(0, INSERTION, s2);
            return transformations;
        }
        else if (s2.empty())
        {
            transformations.emplace_back(0, DELETION, s1);
            return transformations;
        }

        // Neither s1 nor s2 is empty.
        auto table = buildLevenshteinTable(s1, s2);

        // Start in the lower right corner, where the Levenshtein number
        // is, and navigate through the implicit transformations to the
        // upper left corner, where the number is 0.
        // This algorithm favors substitutions over insertions and deletions.

        size_t i = s1.size();
        size_t j = s2.size();
        while (i > 1 || j > 1)
        {
            int current = table[i][j];
            if (i > 1 && j > 1)
            {
                int upperLeft = table[i - 1][j - 1];
                int above = table[i - 1][j];
                int left = table[i][j - 1];
                // Figure out which of the three values to select.
                if (upperLeft <= above && upperLeft <= left)
                {
                    // Going up diagonally is the optimal choice.
                    if (upperLeft < current)
                    {
                        // The diagonal value changed.  That indicates a substitution.
                        transformations.emplace_back(Transformation(i - 1, SUBSTITUTION,
                            string(1, s1[i - 1]),
                            string(1, s2[j - 1])));
                    }
                    i--;
                    j--;
                }
                else if (above < left)
                {
                    // The value above is lower.  That indicates a deletion.
                    transformations.emplace_back(Transformation(i - 1, DELETION, string(1, s1[i - 1])));
                    i--;
                }
                else
                {
                    // The value on the left is lower. That indicates an insertion.
                    transformations.emplace_back(Transformation(i, INSERTION, string(1, s2[j - 1])));
                    j--;
                }
            }
            else if (i > 1)
            {
                // j == 1.  We have reached the left column.  Go only up to capture the deletions.
                int above = table[i - 1][j];
                if (above < current)
                {
                    transformations.emplace_back(Transformation(0, DELETION, s1.substr(0, i)));
                }
                else
                {
                    transformations.emplace_back(Transformation(0, DELETION, s1.substr(0, i-1)));
                }
                i = 0;
            }
            else
            {
                // i == 1.  We have reached the top row.  Go only left to capture the insertions.
                int left = table[i][j - 1];
                if (left < current)
                {
                    transformations.emplace_back(Transformation(0, INSERTION, s2.substr(0, j)));
                }
                else
                {
                    transformations.emplace_back(Transformation(0, INSERTION, s2.substr(0, j-1)));
                }
                j = 0;
            }
        }

        // Final check of the first letter
        if (i == 1 && j == 1 && table[i][j] > 0)
        {
            // This was a substitution in the first letter.
            transformations.emplace_back(Transformation(0, SUBSTITUTION,
                string(1, s1[0]),
                string(1, s2[0])));
        }

        // The transformations are for single character changes, and in reverse order.
        // Now revise the transformations by reversing the order and merging adjacent
        // transformations of the same type.
        reviseTransformations(transformations);

        return transformations;
    }

    vector<vector<int>> String_Comparer::buildLevenshteinTable(const string& s1, const string& s2) const
    {
        vector<vector<int>> table;
        table.resize(s1.size() + 1);
        for (int i = 0; i <= s1.size(); i++) {
            table[i].resize(s2.size() + 1);
        }

        // Initialize top row.
        for (int j = 0; j <= s2.size(); j++) {
            table[0][j] = j;
        }
        // Initialize first column.
        for (int i = 1; i <= s1.size(); i++) {
            table[i][0] = i;
        }
        // Initialize internal rows.
        for (int i = 1; i <= s1.size(); i++) {
            for (int j = 1; j <= s2.size(); j++) {
                table[i][j] = getLevenshteinValue(i, j, s1, s2, table);
            }
        }

        return table;
    }

    int String_Comparer::getLevenshteinValue(int i, int j,
                                             const string& s1, const string& s2,
                                             const vector<vector<int>>& table) const
    {
        int leftCell = table[i][j - 1] + 1;
        int aboveCell = table[i - 1][j] + 1;
        int upperLeftCell = table[i - 1][j - 1];
        if (s1[i - 1] != s2[j - 1])
            upperLeftCell++;
        return min(leftCell, min(aboveCell, upperLeftCell));
    }

    void String_Comparer::reviseTransformations(vector<Transformation>& transformations) const
    {
        if (transformations.size() < 2)
            return;

        // The transformations are in reverse order.  Fix that.
        size_t i = 0;
        size_t j = transformations.size() - 1;
        while (i < j)
        {
            swap(transformations[i++], transformations[j--]);
        }

        // Update the later transformation indices, since they are impacted by earlier insertions and deletions.
        size_t last = transformations.size() - 1;
        for (i = 0; i < last; i++)
        {
            if (transformations[i].type == INSERTION)
            {
                size_t length = transformations[i].s1.length();
                for (j = i + 1; j <= last; j++)
                    transformations[j].index += length;
            }
            else if (transformations[i].type == DELETION)
            {
                size_t length = transformations[i].s1.length();
                for (j = i + 1; j <= last; j++)
                    transformations[j].index -= length;
            }
        }

        // Now merge adjacent transformations of the same type, so that the
        // transformations can be more efficient, rather than transforming
        // one character at a time.
        i = 0;
        while (i < last)
        {
            while (i < last && canBeMerged(transformations, i))
            {
                transformations[i].s1 += transformations[i + 1].s1;
                if (transformations[i].type == SUBSTITUTION)
                {
                    transformations[i].s2 += transformations[i + 1].s2;
                }
                transformations.erase(transformations.begin() + i + 1);
                last--;
            }
            i++;
        }
    }

    bool String_Comparer::canBeMerged(const vector<Transformation>& transformations, size_t i) const
    {
        if (transformations[i + 1].type == transformations[i].type)
        {
            if (transformations[i].type == INSERTION || transformations[i].type == SUBSTITUTION)
            {
                return transformations[i + 1].index == transformations[i].index + transformations[i].s1.size();
            }

            // For deletions, the adjacent indices should match.
            return transformations[i + 1].index == transformations[i].index;
        }

        return false;
    }
}
