#include <napi.h>
#include <vector>
#include "get_exif_date_worker.h"
#include "exif_reader.h"
#if WINDOWS
    #include "windows/platform.h"
#elif LINUX
    #include "linux/platform.h"
#endif
using namespace Napi;
using namespace std;

class Get_exif_date_worker : public AsyncWorker {
public:
    Get_exif_date_worker(const Napi::Env& env, const stdstring& file)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
    , file(file)
   {} 
    ~Get_exif_date_worker() {}

    void Execute () { exif_date = get_exif_date(file); }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    stdstring file;
    uint64_t exif_date;
};

void Get_exif_date_worker::OnOK() {
    HandleScope scope(Env());
    if (exif_date && exif_date < 0x3BA87F85800) {
        napi_value time;
        napi_create_date(Env(), static_cast<double>(exif_date), &time);
        deferred.Resolve(time);
    }
    else 
        deferred.Resolve(Env().Null());
}

Value GetExifDate(const CallbackInfo& info) {
    auto file = (stdstring)info[0].As<nodestring>();

    auto worker = new Get_exif_date_worker(info.Env(), file);
    worker->Queue();
    return worker->GetPromise();
}
