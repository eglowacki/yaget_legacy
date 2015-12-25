#include "Base.h"
#include <set>
#include <UnitTest++.h>

namespace
{
    struct Association
    {
        Association() : Source(0), Target(0), Type(0)
        {}

        Association(uint32_t source, uint32_t target, uint32_t type)
        : Source(source), Target(target), Type(type)
        {}

        uint32_t Source;
        uint32_t Target;
        uint32_t Type;
    };

    struct AssociationSorter
    {
        bool operator()(const Association& elem1, const Association& elem2) const
        {
            return elem1.Target < elem2.Target;
        }
    };

    bool operator==(const Association& elem1, const Association& elem2)
    {
        return elem1.Source == elem2.Source &&
               elem1.Target == elem2.Target &&
               elem1.Type == elem2.Type;
    }


} // namespace
TEST(STL_SET)
{

    Association association1(1, 10, 100);
    Association association2(1, 11, 100);
    Association association3(2, 11, 200);
    Association association4(1, 12, 100);

    std::set<Association, AssociationSorter> areas;
    CHECK(areas.insert(association1).second);
    CHECK(areas.insert(association2).second);
    CHECK(areas.insert(association3).second == false);
    CHECK(areas.insert(association4).second);

    Association associationA(1, 11, 100);
    std::set<Association, AssociationSorter>::const_iterator ita = areas.find(associationA);
    CHECK(ita != areas.end());
    CHECK(association2 == *ita);

    Association associationB(3, 11, 65);
    std::set<Association, AssociationSorter>::const_iterator itb = areas.find(associationB);
    CHECK(itb != areas.end());
    CHECK(association2 == *itb);

    int z = 0;

}

