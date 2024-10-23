#include "platform/os.h"
#include "platform/platform.h"
#include "platform/log.h"

#include "ut_platform.h"
#include "ut_darray.h"


platform_state_st* platform_state;

int main(void)
{
    LOG_INFO("TEST\n");

    STANDARD_RESULT res;
    res = platform_init(&platform_state);
    if (res) { return -1; }

    ut_platform_run();
    ut_darray_run();

    return 0;
}