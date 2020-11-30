#include <napi.h>
#include "../nullfunction.h"
#include "../get_icon_worker.h"
#include "gtk.h"
using namespace Napi;
using namespace std;

class Get_icon_worker : public AsyncWorker {
public:
    Get_icon_worker(const Napi::Env& env, const string& extension)
    : AsyncWorker(Function::New(env, NullFunction, "nullfunction"))
    , deferred(Promise::Deferred::New(Env())) 
    , extension(extension) {}
    ~Get_icon_worker() {}

    void Execute () { 
        auto type = guess_content_type(".js", nullptr, nullptr, nullptr);
        auto icon = get_icon_from_content_type(type);
        gtk_free(type);
        auto theme = get_default_icon_theme();
        auto names = get_icon_names(icon);
        auto icon_info = theme_choose_icon(theme, names, 16, Choose_icon_flags::Force_svg);
        auto icon_name = icon_info_get_filename(icon_info);
        this->icon = icon_name;
        gtk_free(icon_name);
        unref(icon);
    }

    void OnOK();

    Napi::Promise Promise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    string extension;
    string icon;
};

void Get_icon_worker::OnOK() {
    auto env = Env();
    HandleScope scope(env);

    deferred.Resolve(String::New(env, icon));
}

Value GetIcon(const CallbackInfo& info) {
    auto extension = info[0].As<String>();

    auto worker = new Get_icon_worker(info.Env(), extension);
    worker->Queue();
    return worker->Promise();
}

