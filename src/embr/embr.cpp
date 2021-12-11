#include "embr.hpp"

extern void embr_regClasses();
extern void embr_unregClasses();

namespace embr {

bool init()
{
    embr_regClasses();
    return true;
}

void fini()
{
    embr_unregClasses();
}

}  // namespace embr
