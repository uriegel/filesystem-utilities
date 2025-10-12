#include <napi.h>
#include <vector>
#include "get_gpx_track_worker.h"
#include "gpx_track.h"
using namespace Napi;
using namespace std;

class Get_gpx_track_worker : public AsyncWorker {
public:
    Get_gpx_track_worker(const Napi::Env& env, const stdstring& path)
    : AsyncWorker(env)
    , path(path)
    , deferred(Promise::Deferred::New(Env())) {}
    ~Get_gpx_track_worker() {}

    void Execute () { gpx_track = move(get_gpx_track(path)); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    stdstring path;
    Promise::Deferred deferred;
    GpxTrack gpx_track;
};

void Get_gpx_track_worker::OnOK() {
    HandleScope scope(Env());

    // auto err_code = get<0>(files_result);
    // auto err_msg = get<1>(files_result);
    // auto items = get<2>(files_result);
    // if (err_code == 0) {
    //     auto array = Array::New(Env(), items.size());
    //     int i{0};
    //     for(auto item: items) {
    //         auto obj = Object::New(Env());

    //         obj.Set("name", nodestring::New(Env(), item.name));
    //         obj.Set("size", Number::New(Env(), static_cast<double>(item.size)));
    //         napi_value time;
    //         napi_create_date(Env(), static_cast<double>(item.time), &time);
    //         obj.Set("time", time);
    //         obj.Set("isDirectory", Boolean::New(Env(), item.is_directory));
    //         obj.Set("isHidden", Boolean::New(Env(), item.is_hidden));

    //         array.Set(i++, obj);
    //     }
    //     deferred.Resolve(array);
    // } else {
    //     auto obj = Object::New(Env());
    //     obj.Set("code", Number::New(Env(), (int)err_code));
    //     obj.Set("description", String::New(Env(), err_msg));
    //     deferred.Reject(obj);
    // }
}

Value GetGpxTrack(const CallbackInfo& info) {
    auto path = (stdstring)info[0].As<nodestring>();
    auto worker = new Get_gpx_track_worker(info.Env(), path);
    worker->Queue();
    return worker->Promise();
}