extern "C"
{
#include <libpdbg.h>
}

#include "phalerror/phal_error.hpp"
#include "procedures/phal/common_utils.hpp"

#include <fmt/format.h>
#include <libekb.H>

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace phal
{

using namespace phosphor::logging;

void phal_init(enum ipl_mode mode)
{
    // TODO: Setting boot error callback should not be in common code
    //       because, we wont get proper reason in PEL for failure.
    //       So, need to make code like caller of this function pass error
    //       handling callback.
    // add callback methods for debug traces and for boot failures
    openpower::pel::addBootErrorCallbacks();

    // PDBG_DTB environment variable set to CEC device tree path
    static constexpr auto PDBG_DTB_PATH =
        "/var/lib/phosphor-software-manager/pnor/rw/DEVTREE";

    if (setenv("PDBG_DTB", PDBG_DTB_PATH, 1))
    {
        log<level::ERR>(
            fmt::format("Failed to set PDBG_DTB: ({})", strerror(errno))
                .c_str());
        throw std::runtime_error("Failed to set PDBG_DTB");
    }

    if (!pdbg_targets_init(NULL))
    {
        log<level::ERR>("pdbg_targets_init failed");
        throw std::runtime_error("pdbg target initialization failed");
    }

    if (libekb_init())
    {
        log<level::ERR>("libekb_init failed");
        throw std::runtime_error("libekb initialization failed");
    }

    if (ipl_init(mode) != 0)
    {
        log<level::ERR>("ipl_init failed");
        throw std::runtime_error("libipl initialization failed");
    }
}

} // namespace phal
} // namespace openpower