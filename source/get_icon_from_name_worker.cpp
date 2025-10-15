#include <napi.h>
#include <vector>
#include "get_icon_worker.h"
#include "icon.h"
#include "std_utils.h"
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
using namespace Napi;
using namespace std;

class Get_icon_from_name_worker : public AsyncWorker {
public:
    Get_icon_from_name_worker(const Napi::Env& env, const stdstring& name)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , name(name) {}
    ~Get_icon_from_name_worker() {}

    void Execute () { 
        icon_bytes = move(get_icon_from_name(name)); 
    }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    stdstring name;
    vector<char> icon_bytes;
};

void Get_icon_from_name_worker::OnOK() {
    auto env = Env();
    HandleScope scope(env);

    auto buffer = Napi::Buffer<char>::Copy(env, icon_bytes.data(), icon_bytes.size());
    deferred.Resolve(buffer);    
}

Value GetIconFromName(const CallbackInfo& info) {
    checkInitializeIcons();
    auto name = (stdstring)info[0].As<nodestring>();
    auto worker = new Get_icon_from_name_worker(info.Env(), name);
    worker->Queue();
    return worker->Promise();
}

