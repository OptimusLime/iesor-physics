#ifndef WORLDWRAPPER_H
#define WORLDWRAPPER_H

#include <node.h>
#include <IESoR/iesor.h>

using namespace v8;

class IESoRWrap : node::ObjectWrap {
  static Persistent<Function> constructor;

  public:
  static void Init(Handle<Object> exports);
  
  IESoRWorld* World() const { return world_; }


 private:
  explicit IESoRWrap();
  ~IESoRWrap();

  IESoRWorld* world_;

  static Handle<Value> New(const Arguments& args);
  static Handle<Value> LoadBody(const Arguments& args);
};

#endif