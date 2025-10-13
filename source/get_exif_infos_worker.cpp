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
    Get_exif_infos_worker(const Napi::Env& env, const vector<stdstring>& files)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , files(files)
   {} 
    ~Get_exif_infos_worker() {}

    void Execute () { exif_date = get_exif_date("file"s); }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    vector<stdstring> files;
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
    auto pathes = info[0].As<Array>();
    auto count = pathes.Length();
    vector<stdstring> files;
    for (auto i = 0u; i < count; i++) 
        files.push_back(pathes.Get(i).As<nodestring>());

    auto worker = new Get_exif_infos_worker(info.Env(), files);
    worker->Queue();
    return worker->GetPromise();
}
