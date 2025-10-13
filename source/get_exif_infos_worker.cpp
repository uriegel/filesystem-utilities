#include <napi.h>
#include <vector>
#include "get_exif_infos_worker.h"
#include "exif_reader.h"
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
using namespace Napi;
using namespace std;

class Get_exif_infos_worker : public AsyncWorker {
public:
    Get_exif_infos_worker(const Napi::Env& env, const vector<ExifInfosInput>& input)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , input(input)
   {} 
    ~Get_exif_infos_worker() {}

    void Execute () { exif_date = get_exif_infos(input); }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    vector<ExifInfosInput> input;
    uint64_t exif_date;
};

void Get_exif_infos_worker::OnOK() {
    HandleScope scope(Env());
    if (exif_date && exif_date < 0x3BA87F85800) {
        napi_value time;
        napi_create_date(Env(), static_cast<double>(exif_date), &time);
        deferred.Resolve(time);
    }
    else 
        deferred.Resolve(Env().Null());
}

Value GetExifInfosAsync(const CallbackInfo& info) {
    auto inputs = info[0].As<Array>();
    auto count = inputs.Length();
    vector<ExifInfosInput> input;
    for (auto i = 0u; i < count; i++) {
        auto val = inputs.Get(i);
        auto obj = val.As<Object>();
        ExifInfosInput eii;
        eii.path = obj.Get("path").As<nodestring>();
        eii.idx = obj.Get("idx").As<Number>().Int32Value();
        input.push_back(eii);
    }

    auto worker = new Get_exif_infos_worker(info.Env(), input);
    worker->Queue();
    return worker->GetPromise();
}
