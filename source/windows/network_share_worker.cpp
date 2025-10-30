#include <napi.h>
#include "network_share_worker.h"
using namespace Napi;
using namespace std;

class Netwotk_share_worker : public AsyncWorker {
public:
    Netwotk_share_worker(const Napi::Env& env, wstring share, wstring name, wstring passwd)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env()))
    , share(share) 
    , name(name) 
    , passwd(passwd) {}
    ~Netwotk_share_worker() {}

    void Execute () { versions = get_versions(input, cancellation); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    wstring share;
    wstring name;
    wstring passwd;
    vector<VersionInfoResult> versions;
};

Value AddNetworkShare(const CallbackInfo& info) {
    auto share = info[0].As<WString>().WValue();
    auto name = info[1].As<WString>().WValue();
    auto passwd = info[2].As<WString>().WValue();

    auto worker = new Netwotk_share_worker(info.Env(), share, name, passwd);
    worker->Queue();
    return worker->Promise();
}