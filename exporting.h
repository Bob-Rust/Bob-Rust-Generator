// This will export some functions to Node.js
//#define BUILD_NODE

#ifdef BUILD_NODE
#include <node.h>

namespace bob_rust_generator {
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::Number;
	using v8::value;
	
	// TODO: Create a callback function to tell the js app that we have generated the shapes
	// TODO: Create a way to send data between C++ and node.js
	// TODO: Create a way to run everything async

	void Method(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

	}

	void Initialize(Local<Object> exports) {
		NODE_SET_METHOD(exports, "???", Method);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);
}

#endif