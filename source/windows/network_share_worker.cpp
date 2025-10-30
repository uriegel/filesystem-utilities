#include <napi.h>
#include <windows.h>
#include <winnetwk.h>
#include "network_share_worker.h"
#include "..\wstring.h"
using namespace Napi;
using namespace std;

int make_share(const wstring& share, const wstring& name, const wstring& passwd) {
//    [DllImport("mpr.dll")]

    NETRESOURCEW nr = {
        RESOURCE_GLOBALNET,
        RESOURCETYPE_DISK,
        RESOURCEDISPLAYTYPE_SHARE,
        0,
        (LPWSTR)share.c_str(),
        nullptr, 
        nullptr
    };

    return WNetAddConnection2W(&nr, passwd.c_str(), name.c_str(), 0);
}

class Netwotk_share_worker : public AsyncWorker {
public:
    Netwotk_share_worker(const Napi::Env& env, wstring share, wstring name, wstring passwd)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env()))
    , share(share) 
    , name(name) 
    , passwd(passwd) {}
    ~Netwotk_share_worker() {}

    void Execute () { make_share(share, name, passwd); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    wstring share;
    wstring name;
    wstring passwd;
};

void Netwotk_share_worker::OnOK() {
    
}

Value AddNetworkShare(const CallbackInfo& info) {
    auto share = info[0].As<WString>().WValue();
    auto name = info[1].As<WString>().WValue();
    auto passwd = info[2].As<WString>().WValue();

    auto worker = new Netwotk_share_worker(info.Env(), share, name, passwd);
    worker->Queue();
    return worker->Promise();
}