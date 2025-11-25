#include <napi.h>
#include <string>
#include <Windows.h>
using namespace Napi;
using namespace std;

struct Service_item {
    wstring name;
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
        for (DWORD i = 0; i < servicesReturned; ++i) {
            wprintf(L"Service Name: %s\n", services[i].lpServiceName);
            wprintf(L"Display Name: %s\n", services[i].lpDisplayName);
        }

        CloseServiceHandle(hSCManager);
    }

    void OnOK();

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    Promise::Deferred deferred;
};

void Services_worker::OnOK() {
    HandleScope scope(Env());

    // auto native_err_code = get<0>(copy_result);
    // auto err_code = get<1>(copy_result);
    // auto err_msg = get<2>(copy_result);
    // if (native_err_code == 0) 
    //     deferred.Resolve(Env().Null());
    // else {
    //     Napi::Object errObj = Env().Global()
    //         .Get("Error").As<Napi::Function>()
    //         .New({ nodestring::New(Env(), err_msg) });        
    //     errObj.Set("error", nodestring::New(Env(), err_code));
    //     errObj.Set("nativeError", Number::New(Env(), (int)native_err_code));
    //     deferred.Reject(errObj);
    // }
    deferred.Resolve(Env().Null());
}

Value GetServices(const CallbackInfo& info) {
    auto worker = new Services_worker(info.Env());
    worker->Queue();
    return worker->GetPromise();
}

