#include <napi.h>
#include <vector>
#include <optional>
#include "get_versions_worker.h"
#include "platform.h"
using namespace Napi;
using namespace std;

vector<VersionInfo> get_versions(vector<VersionsInput>& input, wstring cancellation) {
	vector<VersionInfo> output;

	register_cancellable(cancellation);

	for (auto& vi: input) {
		if (is_cancelled(cancellation))
			return vector<VersionInfo>();

		
        
        // auto ret = get_exif_info(vi.path, vi.idx);
		// if (ret.date != 0 || ret.latitude != 0 || ret.longitude != 0)
		// 	output.push_back(ret);


	}

	unregister_cancellable(cancellation);

	return output;
}

class Get_versions_worker : public AsyncWorker {
public:
    Get_versions_worker(const Napi::Env& env, const vector<VersionsInput>& input, wstring cancellation)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env()))
    , input(input) {}
    , cancellation(cancellation) {}
    ~Get_versions_worker() {}

    void Execute () { version = get_versions(input, cancellation); }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Napi::Promise::Deferred deferred;
    vector<VersionsInput> input;
    wstring cancellation;
    optional<Version_info> version;
};

void Get_versions_worker::OnOK() {
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

Value GetVersions(const CallbackInfo& info) {
    auto inputs = info[0].As<Array>();
    auto count = inputs.Length();
    vector<VersionsInput> input;
    for (auto i = 0u; i < count; i++) {
        auto val = inputs.Get(i);
        auto obj = val.As<Object>();
        VersionsInput vi;
        vi.path = obj.Get("path").As<nodestring>();
        vi.idx = obj.Get("idx").As<Number>().Int32Value();
        input.push_back(vi);
    }

    auto cancelation = (info.Length() > 1) ? info[1].As<nodestring>() : wstring();

    auto worker = new Get_versions_worker(info.Env(), input, cancelation);
    worker->Queue();
    return worker->Promise();
}

