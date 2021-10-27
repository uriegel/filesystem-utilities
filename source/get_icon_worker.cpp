#include <napi.h>
#include <vector>
#include "wstring.h"
#include "nullfunction.h"
#include "get_icon_worker.h"
#include "icon.h"
#include "std_utils.h"
using namespace Napi;
using namespace std;

class Get_icon_worker : public AsyncWorker {
public:
    Get_icon_worker(const Napi::Env& env, const stdstring& extension)
    : AsyncWorker(Function::New(env, NullFunction, "nullfunction"))
    , deferred(Promise::Deferred::New(Env())) 
    , extension(extension) {}
    ~Get_icon_worker() {}

    void Execute () { icon_bytes = move(get_icon(extension)); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    stdstring extension;
    vector<char> icon_bytes;
};

void Get_icon_worker::OnOK() {
    auto env = Env();
    HandleScope scope(env);

    auto buffer_result = new vector<char>(move(icon_bytes));
    auto buffer = Buffer<char>::New(env, buffer_result->data(), buffer_result->size(), [](Napi::Env, char*, vector<char>* to_delete){ delete to_delete; }, buffer_result);
    deferred.Resolve(buffer);
}

Value GetIcon(const CallbackInfo& info) {
    auto extension = (stdstring)info[0].As<nodestring>();
    auto worker = new Get_icon_worker(info.Env(), extension);
    worker->Queue();
    return worker->Promise();
}

