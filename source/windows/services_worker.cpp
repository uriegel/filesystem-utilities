#include <napi.h>
#include <string>
#include <Windows.h>
#include "../wstring.h"
using namespace Napi;
using namespace std;

struct Service_item {
    wstring display_name;
    wstring name;
    int state;
};

class Services_worker : public AsyncWorker
{
public:
    Services_worker(const Napi::Env& env)
    : AsyncWorker(env)
    , deferred(Promise::Deferred::New(Env())) 
   {} 
    ~Services_worker() { }

    void Execute() { 

        DWORD bytesNeeded = 0;
        DWORD resumeHandle = 0;
        DWORD servicesReturned = 0;

        auto hSCManager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
        EnumServicesStatusExW(
            hSCManager,
            SC_ENUM_PROCESS_INFO,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            nullptr,
            0,
            &bytesNeeded,
            &servicesReturned,
            &resumeHandle,
            nullptr
        );

        if (GetLastError() != ERROR_MORE_DATA) {
            wprintf(L"EnumServicesStatusExW failed. Error: %lu\n", GetLastError());
            CloseServiceHandle(hSCManager);
            return;
        }

        std::vector<BYTE> buffer(bytesNeeded);
        LPENUM_SERVICE_STATUS_PROCESS services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

        if (!EnumServicesStatusExW(
            hSCManager,
            SC_ENUM_PROCESS_INFO,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            reinterpret_cast<LPBYTE>(services),
            bytesNeeded,
            &bytesNeeded,
            &servicesReturned,
            &resumeHandle,
            nullptr
        )) {
            wprintf(L"EnumServicesStatusExW failed. Error: %lu\n", GetLastError());
            CloseServiceHandle(hSCManager);
            return;
        }

        // Print service name, display name, and executable path
        for (auto i = 0u; i < servicesReturned; ++i) {
            items.push_back(Service_item{ 
                (wstring)(LPWSTR)services[i].lpDisplayName, 
                (wstring)(LPWSTR)services[i].lpServiceName, 
                2
            });
        }

        CloseServiceHandle(hSCManager);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
    vector<Service_item> items;
};

void Services_worker::OnOK() {
    HandleScope scope(Env());

    auto array = Array::New(Env(), items.size());
    int i{0};
    for(auto item: items) {
        auto obj = Object::New(Env());

        obj.Set("name", WString::New(Env(), item.name));
        obj.Set("displayName", WString::New(Env(), item.display_name));
        obj.Set("state", Number::New(Env(), item.state));

        array.Set(i++, obj);
    }
    deferred.Resolve(array);
}

Value GetServices(const CallbackInfo& info) {
    auto worker = new Services_worker(info.Env());
    worker->Queue();
    return worker->GetPromise();
}

