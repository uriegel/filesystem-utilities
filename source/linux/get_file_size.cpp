#include <stdio.h>
#include "get_file_size.h"
#include "../nullfunction.h"
using namespace Napi;
using namespace std;

Value GetFileSizeSync(const CallbackInfo& info) {
    auto file = (string)info[0].As<String>();
    auto size = get_file_size(file);
    return Number::New(info.Env(), static_cast<double>(size));
}

int64_t get_file_size(const string& file_name) {
    auto file = fopen(file_name.c_str(), "r");
    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    fclose(file);
    return size;   
}

class Get_file_size_worker : public AsyncWorker {
public:
    Get_file_size_worker(const Napi::Env& env, const string& file)
    : AsyncWorker(Function::New(env, NullFunction, "nullfunction"))
    , deferred(Promise::Deferred::New(Env()))
    , file(file) {}
    ~Get_file_size_worker() {}

    void Execute () { size = get_file_size(file); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    string file;
    int64_t size;
};

void Get_file_size_worker::OnOK() {
    auto env = Env();
    HandleScope scope(env);
    deferred.Resolve(Number::New(Env(), static_cast<double>(size)));
}

Value GetFileSize(const CallbackInfo& info) {
    auto file = (string)info[0].As<String>();

    auto worker = new Get_file_size_worker(info.Env(), file);
    worker->Queue();
    return worker->Promise();
}


