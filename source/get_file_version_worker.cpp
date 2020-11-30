#include <napi.h>
#include <vector>
#include <optional>
#include "nullfunction.h"
#include "file_version.h"
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
using namespace Napi;
using namespace std;

class Get_file_version_worker : public AsyncWorker {
public:
    Get_file_version_worker(const Napi::Env& env, const stdstring& file)
    : AsyncWorker(Function::New(env, NullFunction, "nullfunction"))
    , deferred(Promise::Deferred::New(Env()))
    , file(file) {}
    ~Get_file_version_worker() {}

    void Execute () { version = get_file_info_version(file); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    stdstring file;
    optional<Version_info> version;
};

void Get_file_version_worker::OnOK() {
    auto env = Env();
    HandleScope scope(env);

    if (version.has_value()) {
        auto obj = Object::New(Env());
        obj.Set("major", Number::New(Env(), static_cast<double>(version.value().major)));
        obj.Set("minor", Number::New(Env(), static_cast<double>(version.value().minor)));
        obj.Set("build", Number::New(Env(), static_cast<double>(version.value().build)));
        obj.Set("patch", Number::New(Env(), static_cast<double>(version.value().patch)));
        deferred.Resolve(obj);
    } else 
        deferred.Resolve(Env().Null());
}

Value GetFileVersion(const CallbackInfo& info) {
    auto file = (stdstring)info[0].As<nodestring>();

    auto worker = new Get_file_version_worker(info.Env(), file);
    worker->Queue();
    return worker->Promise();
}

