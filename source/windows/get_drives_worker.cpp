#include <napi.h>
#include <vector>
#include "get_drives_worker.h"
#include "drives.h"
#include "../nullfunction.h"
#include "platform.h"
using namespace Napi;
using namespace std;

class Get_drives_worker : public AsyncWorker {
public:
    Get_drives_worker(const Napi::Env& env)
    : AsyncWorker(Function::New(env, NullFunction, "theFunction"))
    , deferred(Promise::Deferred::New(Env())) {}
    ~Get_drives_worker() {}

    void Execute () { drives = move(get_drives()); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    vector<Drive_item> drives;
};

void Get_drives_worker::OnOK() {
    HandleScope scope(Env());

    auto array = Array::New(Env(), drives.size());
    int i{0};
    for(auto item: drives) {
        auto obj = Object::New(Env());

        obj.Set("name", nodestring::New(Env(), item.name));
        obj.Set("description", nodestring::New(Env(), item.description));
        obj.Set("size", Number::New(Env(), static_cast<double>(item.size)));
        obj.Set("type", Number::New(Env(), static_cast<double>(item.type)));
        obj.Set("isMounted", Boolean::New(Env(), item.is_mounted));

        array.Set(i++, obj);
    }

    deferred.Resolve(array);
}

Value GetDrives(const CallbackInfo& info) {
    auto worker = new Get_drives_worker(info.Env());
    worker->Queue();
    return worker->Promise();
}