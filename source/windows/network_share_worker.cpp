#include <napi.h>
#include <windows.h>
#include <winnetwk.h>
#include "network_share_worker.h"
#include "..\wstring.h"
#include "..\error.h"
using namespace Napi;
using namespace std;

tuple<int, wstring, wstring> make_share(const wstring& share, const wstring& name, const wstring& passwd) {
//    [DllImport("mpr.dll")]

    NETRESOURCEW nr = {
        RESOURCE_GLOBALNET,
        RESOURCETYPE_DISK,
        RESOURCEDISPLAYTYPE_SHARE,
        0,
        nullptr, 
        (LPWSTR)share.c_str(),
        nullptr
    };

    auto error_code = WNetAddConnection2W(&nr, passwd.c_str(), name.c_str(), 0);
    if (error_code == 0) {
        tuple<int, wstring, wstring> result(0, L""s, L""s);
        return result;
    } else
        return make_result(error_code);
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

    void Execute () { result = make_share(share, name, passwd); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    wstring share;
    wstring name;
    wstring passwd;
    tuple<int, wstring, wstring> result;
};

void Netwotk_share_worker::OnOK() {
    HandleScope scope(Env());
    auto native_err_code = get<0>(result);
    auto err_code = get<1>(result);
    auto err_msg = get<2>(result);
    if (native_err_code == 0) 
        deferred.Resolve(Env().Null());
    else {
        Napi::Object errObj = Env().Global()
            .Get("Error").As<Napi::Function>()
            .New({ WString::New(Env(), err_msg) });        
        errObj.Set("error", WString::New(Env(), err_code));
        errObj.Set("nativeError", Number::New(Env(), (int)native_err_code));
        deferred.Reject(errObj);
    }
}

Value AddNetworkShare(const CallbackInfo& info) {
    auto share = info[0].As<WString>().WValue();
    auto name = info[1].As<WString>().WValue();
    auto passwd = info[2].As<WString>().WValue();

    auto worker = new Netwotk_share_worker(info.Env(), share, name, passwd);
    worker->Queue();
    return worker->Promise();
}