#include <winsock2.h>
#include <ws2tcpip.h>

#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/Generated/Vector4.hpp>

#include <iostream>
#include <string>

#include "json.hpp"

#pragma comment( lib, "ws2_32.lib" )

SOCKET s;

struct NetworkController : RED4ext::IScriptable
{
    RED4ext::CClass* GetNativeType();

    nlohmann::json data;
};

RED4ext::TTypedClass<NetworkController> networkControllerClass("NetworkController");

RED4ext::CClass* NetworkController::GetNativeType()
{
    return &networkControllerClass;
}

void Clear(RED4ext::IScriptable* aContext, RED4ext::CStackFrame* aFrame, void* aOut, int64_t a4)
{
    auto self = static_cast<NetworkController*>(aContext);
    self->data.clear();
}

void AddString(RED4ext::IScriptable* aContext, RED4ext::CStackFrame* aFrame, void* aOut, int64_t a4)
{
    auto self = static_cast<NetworkController*>(aContext);

    RED4ext::CString key, value;
    RED4ext::GetParameter(aFrame, &key);
    RED4ext::GetParameter(aFrame, &value);
    aFrame->code++; // skip ParamEnd

    self->data[key.c_str()] = value.c_str();
}

void AddVector4(RED4ext::IScriptable* aContext, RED4ext::CStackFrame* aFrame, void* aOut, int64_t a4)
{
    auto self = static_cast<NetworkController*>(aContext);

    RED4ext::CString key;
    RED4ext::Vector4 value;
    RED4ext::GetParameter(aFrame, &key);
    RED4ext::GetParameter(aFrame, &value);
    aFrame->code++; // skip ParamEnd

    self->data[key.c_str()] = nlohmann::json::array({value.X, value.Y, value.Z, value.W});
}

void AddFloat(RED4ext::IScriptable* aContext, RED4ext::CStackFrame* aFrame, void* aOut, int64_t a4)
{
    auto self = static_cast<NetworkController*>(aContext);

    RED4ext::CString key;
    float value;
    RED4ext::GetParameter(aFrame, &key);
    RED4ext::GetParameter(aFrame, &value);
    aFrame->code++; // skip ParamEnd

    self->data[key.c_str()] = value;
}

void SendUDP(RED4ext::IScriptable* aContext, RED4ext::CStackFrame* aFrame, void* aOut, int64_t a4)
{
    auto self = static_cast<NetworkController*>(aContext);
 
    const char* destIP = "127.0.0.1";
    sockaddr_in dest;

    aFrame->code++; // skip ParamEnd

    dest.sin_family = AF_INET;
    inet_pton(AF_INET, destIP, &dest.sin_addr.s_addr);
    dest.sin_port = htons(2020);

    std::string dump = self->data.dump();
    sendto(s, dump.c_str(), dump.size(), 0, (sockaddr*)&dest, sizeof(dest));

    self->data.clear();
}


void RegisterTypes()
{
    RED4ext::CRTTISystem::Get()->RegisterType(&networkControllerClass);
}

void PostRegisterTypes()
{
    auto rtti = RED4ext::CRTTISystem::Get();
    auto scriptable = rtti->GetClass("IScriptable");
    networkControllerClass.parent = scriptable;

    auto addString = RED4ext::CClassFunction::Create(&networkControllerClass, "AddString", "AddString", &AddString, { .isNative = true });
    addString->AddParam("String", "key");
    addString->AddParam("String", "value");
    networkControllerClass.RegisterFunction(addString);

    auto addVector4 = RED4ext::CClassFunction::Create(&networkControllerClass, "AddVector4", "AddVector4", &AddVector4, { .isNative = true });
    addVector4->AddParam("String", "key");
    addVector4->AddParam("Vector4", "value");
    networkControllerClass.RegisterFunction(addVector4);

    auto addFloat = RED4ext::CClassFunction::Create(&networkControllerClass, "AddFloat", "AddFloat", &AddFloat, { .isNative = true });
    addFloat->AddParam("String", "key");
    addFloat->AddParam("Float", "value");
    networkControllerClass.RegisterFunction(addFloat);

    auto clear = RED4ext::CClassFunction::Create(&networkControllerClass, "Clear", "Clear", &Clear, { .isNative = true });
    networkControllerClass.RegisterFunction(clear);

    auto sendUDP = RED4ext::CClassFunction::Create(&networkControllerClass, "Send", "Send", &SendUDP, { .isNative = true });
    //getNumber->SetReturnType("Float");
    networkControllerClass.RegisterFunction(sendUDP);
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason, const RED4ext::Sdk* aSdk)
{
    switch (aReason)
    {
        case RED4ext::EMainReason::Load:
        {
            WSAData data;
            WSAStartup(MAKEWORD(2, 2), &data);

            const char* srcIP = "127.0.0.1";
            sockaddr_in local;

            local.sin_family = AF_INET;
            inet_pton(AF_INET, srcIP, &local.sin_addr.s_addr);
            local.sin_port = htons(0);

            s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            bind(s, (sockaddr*)&local, sizeof(local));

            RED4ext::RTTIRegistrator::Add(RegisterTypes, PostRegisterTypes);;
            break;
        }
        case RED4ext::EMainReason::Unload:
        {
            closesocket(s);
            WSACleanup();
            break;
        }
    }

    return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"Plugin's name";
    aInfo->author = L"Author's name";
    aInfo->version = RED4EXT_SEMVER(1, 0, 0); // Set your version here.
    aInfo->runtime = RED4EXT_RUNTIME_LATEST;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}