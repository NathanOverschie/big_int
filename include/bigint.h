#include "common.h"
#include "dint.h"

namespace bigint
{
    class bigint
    {
    public:
        bigint();
        ~bigint();

    private:
        union rep
        {
            dint dint;
            base base = 0;
        };
    };

    bigint::bigint(/* args */)
    {
    }

    bigint::~bigint()
    {
    }

}