#include <map>
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
#include "cancellation.h"
using namespace std;

map<stdstring, bool> cancellations;

void register_cancellable(stdstring cancellation) {
    if (!cancellation.empty())
        cancellations[cancellation] = false;
}

void unregister_cancellable(stdstring cancellation) {
    if (!cancellation.empty())
        cancellations.erase(cancellation);
}

bool is_cancelled(stdstring cancellation) {
    return cancellations[cancellation];
}

void cancel(stdstring cancellation) {
    if (cancellations.contains(cancellation))
        cancellations[cancellation] = true;
}

Napi::Value Cancel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto cancellation = (stdstring)info[0].As<nodestring>();
    cancel(cancellation);

    return env.Undefined();    
}