#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "worldWrapper.h"

using namespace v8;

Persistent<Function> IESoRWrap::constructor;

IESoRWrap::IESoRWrap() {
  world_ = new IESoRWorld();
}

IESoRWrap::~IESoRWrap() {
}

void IESoRWrap::Init(Handle<Object> exports) {
   HandleScope scope;
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("iesorWorld"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("loadBody"),
      FunctionTemplate::New(LoadBody)->GetFunction());

  constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("iesorWorld"), constructor);
}

Handle<Value> IESoRWrap::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new IESoRWrap(...)`
    IESoRWrap* obj = new IESoRWrap();
    obj->Wrap(args.This());
    return args.This();
  } else {
    // Invoked as plain function `IESoRWrap(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

Handle<Value> IESoRWrap::LoadBody(const Arguments& args) {
  HandleScope scope;

  IESoRWrap* obj = ObjectWrap::Unwrap<IESoRWrap>(args.This());
  
  std::string param1(*v8::String::Utf8Value(args[0]->ToString()));
  param1 += "--yes sir";

  return scope.Close(String::New(param1.c_str()));
}


