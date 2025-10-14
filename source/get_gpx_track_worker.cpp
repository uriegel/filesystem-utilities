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

    auto obj = Object::New(Env());

    obj.Set("name", nodestring::New(Env(), gpx_track.name));
    obj.Set("distance", Number::New(Env(), static_cast<double>(gpx_track.distance)));
    obj.Set("duration", Number::New(Env(), static_cast<int>(gpx_track.duration)));
    obj.Set("date", nodestring::New(Env(), gpx_track.date));
    obj.Set("averageSpeed", Number::New(Env(), static_cast<int>(gpx_track.averageSpeed)));

    auto array = Array::New(Env(), gpx_track.trackPoints.size());
    int i{0};
    for(auto item: gpx_track.trackPoints) {
        auto pnt = Object::New(Env());

        pnt.Set("lat", Number::New(Env(), item.lat));
        pnt.Set("lon", Number::New(Env(), item.lon));
        pnt.Set("ele", Number::New(Env(), item.ele));
        pnt.Set("time", nodestring::New(Env(), item.time));
        pnt.Set("heartrate", Number::New(Env(), item.heartrate));
        pnt.Set("velocity", Number::New(Env(), item.velocity));
        array.Set(i++, pnt);
    }
    obj.Set("trackPoints", array);

    deferred.Resolve(obj);
}

Value GetGpxTrack(const CallbackInfo& info) {
    auto path = (stdstring)info[0].As<nodestring>();
    auto worker = new Get_gpx_track_worker(info.Env(), path);
    worker->Queue();
    return worker->Promise();
}