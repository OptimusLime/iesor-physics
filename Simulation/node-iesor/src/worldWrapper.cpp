#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "worldWrapper.h"
#include <json.h>

using namespace v8;

Persistent<Function> IESoRWrap::constructor;

IESoRWrap::IESoRWrap() {
  director_ = new IESoRDirector();
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
  tpl->PrototypeTemplate()->Set(String::NewSymbol("loadBodyFromNetwork"),
      FunctionTemplate::New(LoadBodyFromNetwork)->GetFunction());

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

//TODO: Make this asynchronous
Handle<Value> IESoRWrap::LoadBodyFromNetwork(const Arguments& args) {
  HandleScope scope;

  IESoRWrap* obj = ObjectWrap::Unwrap<IESoRWrap>(args.This());
  
  std::string byteNetwork(*v8::String::Utf8Value(args[0]->ToString()));
  
  IESoRDirector* director = obj->Director();

  //send the body into our director (which will in turn load into a iesorWorld
  director->insertBodyFromNetwork(byteNetwork);

  //... in the future, we'll then use the body information to insert into our world
  
  //until then, just send back the body information
  return scope.Close(String::New(byteNetwork.c_str()));
}


