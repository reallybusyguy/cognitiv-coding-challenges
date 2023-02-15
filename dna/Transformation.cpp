#include "Transformation.hpp"

namespace dna
{
    Transformation::Transformation(size_t ind, TransformType t, const string& str1) :
        index(ind), type(t), s1(str1) {}
    Transformation::Transformation(size_t ind, TransformType t, const string& str1, const string& str2) :
        index(ind), type(t), s1(str1), s2(str2) {}

    ostream& operator << (ostream& ostr, const Transformation& t)
    {
        string typeStr = (t.type == INSERTION ? "INSERTION" : (t.type == DELETION ? "DELETION" : "SUBSTITUTION"));
        ostr << "Index: " << t.index << ", Type: " << typeStr;
        if (t.type == SUBSTITUTION)
        {
            ostr << ", Strings: " << t.s1 << ", " << t.s2;
        }
        else
        {
            ostr << ", String: " << t.s1;
        }
        return ostr;
    }

    string applyTransformations(const string& str, const vector<Transformation>& transformations)
    {
        string output = str;

        for (auto& t : transformations)
        {
            if (t.type == INSERTION)
            {
                output.insert(t.index, t.s1);
            }
            else if (t.type == DELETION)
            {
                output.erase(t.index, t.s1.size());
            }
            else
            {
                // Substitution
                output.replace(t.index, t.s1.size(), t.s2);
            }
        }

        return output;
    }

}