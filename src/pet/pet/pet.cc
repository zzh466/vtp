#include <node.h>
#include <uv.h>
#include "uv_wrap_trader_pet.h"

using namespace v8;

bool islog;//log?


void CreateTrader(const FunctionCallbackInfo<Value>& args) {
  UVWrapTraderPet::NewInstance(args);
}

void Settings(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  if (!args[0]->IsUndefined() && args[0]->IsObject()) {
    Local<Object> setting = args[0]->ToObject(context).ToLocalChecked();
    Local<Value> log = setting->Get(context, String::NewFromUtf8(isolate, "log").ToLocalChecked()).ToLocalChecked();
    if (!log->IsUndefined()) {
      islog = log->BooleanValue(isolate);
    }
  }

  args.GetReturnValue().Set(Undefined(isolate));
}

void Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  UVWrapTraderPet::Init(isolate);

  NODE_SET_METHOD(exports, "createTrader", CreateTrader);
  NODE_SET_METHOD(exports, "settings", Settings);
}

NODE_MODULE(pet, Init)