// test torjo log
#include "torjo_log.h"

using namespace eg;

void PlayTorjo()
{
    config::InitLogs("UT_Utility.log");

    log_debug << "Back buffer looks slightly wacked";
    log_trace("util") << "Started watching directory...";
    log_warning << "We lost device";
    ::Sleep(8);
    log_error << "Could not create component";
    log_fatal << "Object Manager does not exist this sh---------------.";
}