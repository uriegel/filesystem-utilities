#include <napi.h>
#include <vector>
#include "get_files_worker.h"
#include "files.h"
#if WINDOWS
#include "windows/platform.h"
#elif LINUX
#include "linux/platform.h"
#endif
using namespace Napi;
using namespace std;

class Get_files_worker : public AsyncWorker {
public:
    Get_files_worker(const Napi::Env& env, const stdstring& directory, bool show_hidden)
    : AsyncWorker(env)
    , directory(directory)
    , show_hidden(show_hidden)
    , deferred(Promise::Deferred::New(Env())) {}
    ~Get_files_worker() {}

    void Execute () { files_result = move(get_files(directory, show_hidden)); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    stdstring directory;
    bool show_hidden;
    Promise::Deferred deferred;
    tuple<int, string, vector<File_item>> files_result;
};

void Get_files_worker::OnOK() {
    HandleScope scope(Env());

    auto err_code = get<0>(files_result);
    auto err_msg = get<1>(files_result);
    auto items = get<2>(files_result);
    if (err_code == 0) {
        auto array = Array::New(Env(), items.size());
        int i{0};
        for(auto item: items) {
            auto obj = Object::New(Env());

            obj.Set("name", nodestring::New(Env(), item.name));
            obj.Set("size", Number::New(Env(), static_cast<double>(item.size)));
            napi_value time;
            napi_create_date(Env(), static_cast<double>(item.time), &time);
            obj.Set("time", time);
            obj.Set("isDirectory", Boolean::New(Env(), item.is_directory));
            obj.Set("isHidden", Boolean::New(Env(), item.is_hidden));
            obj.Set("idx", Number::New(Env(), i+1));

            array.Set(i++, obj);
        }
        deferred.Resolve(array);
    } else {
        auto obj = Object::New(Env());
        obj.Set("code", Number::New(Env(), (int)err_code));
        obj.Set("description", String::New(Env(), err_msg));
        deferred.Reject(obj);
    }
}

Value GetFiles(const CallbackInfo& info) {
    auto directory = (stdstring)info[0].As<nodestring>();
    auto show_hidden = info[1].As<Boolean>().Value();
    auto worker = new Get_files_worker(info.Env(), directory, show_hidden);
    worker->Queue();
    return worker->Promise();
}