// This will export some functions to Node.js

#ifdef BUILD_NODE
#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "borst_object.h"

/*
var borst_generator = require('./build/Release/borst_generator');

function borst_update(list) {}
function borst_done(list) {}

var gen = borst_generator.generate({
	path: '<image-path>',
	count: 4000,
	alpha: 2,
	background: [ <r>, <g>, <b> ],
	intervall: 100
}, borst_update, borst_done);

// Stop generating shapes. (Blocking)
gen.stop();

*/

#define EXP_THROW_EXCEPTION(isolate, msg) isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, msg).ToLocalChecked()))
#define HAS_OR_THROW_EXCEPTION(map, ctx, isolate, key, msg) \
	if(!map->Has(ctx, key).FromMaybe(false)) { \
		EXP_THROW_EXCEPTION(isolate, msg); \
		return; \
	}
#define IS_OR_THROW_EXCEPTION(isolate, expr, msg) \
	if(!expr) { \
		EXP_THROW_EXCEPTION(isolate, msg); \
		return; \
	}

namespace bob_rust_generator {
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::Number;
	using v8::Value;
	using v8::Exception;
	using v8::String;
	using v8::Context;
	using v8::MaybeLocal;
	
	// TODO: Create a callback function to tell the js app that we have generated the shapes
	// TODO: Create a way to send data between C++ and node.js
	// TODO: Create a way to run everything async

	void Generate(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		
		if(args.Length() != 3) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
			return;
		}

		if(!args[0]->IsMap()) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Expected a map object args[0]").ToLocalChecked()));
			return;
		}

		Local<Value> key_path = String::NewFromUtf8(isolate, "path").ToLocalChecked();
		Local<Value> key_count = String::NewFromUtf8(isolate, "count").ToLocalChecked();
		Local<Value> key_alpha = String::NewFromUtf8(isolate, "alpha").ToLocalChecked();
		Local<Value> key_bg = String::NewFromUtf8(isolate, "background").ToLocalChecked();
		Local<Value> key_intervall = String::NewFromUtf8(isolate, "intervall").ToLocalChecked();

		Local<Context> context = isolate->GetCurrentContext();
		Local<Object> options = args[0]->ToObject(context).ToLocalChecked();

		HAS_OR_THROW_EXCEPTION(options, context, isolate, key_path, "Could not find the property 'path' in args[0]")
		HAS_OR_THROW_EXCEPTION(options, context, isolate, key_count, "Could not find the property 'count' in args[0]")
		HAS_OR_THROW_EXCEPTION(options, context, isolate, key_intervall, "Could not find the property 'intervall' in args[0]")
		HAS_OR_THROW_EXCEPTION(options, context, isolate, key_alpha, "Could not find the property 'alpha' in args[0]")
		HAS_OR_THROW_EXCEPTION(options, context, isolate, key_bg, "Could not find the property 'background' in args[0]")

		Local<Value> m_opt_path = options->Get(context, key_path).ToLocalChecked();
		Local<Value> m_opt_count = options->Get(context, key_count).ToLocalChecked();
		Local<Value> m_opt_intervall = options->Get(context, key_intervall).ToLocalChecked();
		Local<Value> m_opt_alpha = options->Get(context, key_alpha).ToLocalChecked();
		Local<Value> m_opt_bg = options->Get(context, key_bg).ToLocalChecked();

		IS_OR_THROW_EXCEPTION(isolate, m_opt_path->IsString(), "The property 'path' was not a string")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_count->IsInt32(), "The property 'count' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_intervall->IsInt32(), "The property 'intervall' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_alpha->IsInt32(), "The property 'alpha' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_bg->IsArray(), "The property 'background' was not an array")
		
		Color bg{ 0, 0, 0, 0xff };
		{
			Local<v8::Array> opt_bg = m_opt_bg->ToObject(context).ToLocalChecked();
			IS_OR_THROW_EXCEPTION(isolate, opt_bg->Length() != 3, "The property 'background' was not a color [ r, g, b, ]")

			Local<Value> col_r = opt_bg->Get(context, 0).ToLocalChecked();
			Local<Value> col_g = opt_bg->Get(context, 1).ToLocalChecked();
			Local<Value> col_b = opt_bg->Get(context, 2).ToLocalChecked();

			IS_OR_THROW_EXCEPTION(isolate, col_r->IsInt32(), "The property 'background' contained a non integer value")
			IS_OR_THROW_EXCEPTION(isolate, col_g->IsInt32(), "The property 'background' contained a non integer value")
			IS_OR_THROW_EXCEPTION(isolate, col_b->IsInt32(), "The property 'background' contained a non integer value")

			bg.r = (unsigned char)col_r->IntegerValue(context).FromMaybe(0);
			bg.g = (unsigned char)col_g->IntegerValue(context).FromMaybe(0);
			bg.b = (unsigned char)col_b->IntegerValue(context).FromMaybe(0);
		}

		// This needs to be deleted with free()
		Settings settings;
		String::Utf8Value str(isolate, m_opt_path->ToString(context).ToLocalChecked());
		settings.ImagePath = strdup(*str);
		settings.MaxShapes = m_opt_count->IntegerValue(context).FromMaybe(0);
		settings.Alpha = m_opt_alpha->IntegerValue(context).FromMaybe(0);
		settings.CallbackShapes = m_opt_intervall->IntegerValue(context).FromMaybe(0);
		settings.Background = bg;

		printf("Settings:\n");
		printf("  ImagePath  = %s\n", settings.ImagePath);
		printf("  MaxShapes  = %d\n", settings.MaxShapes);
		printf("  Callback   = %d\n", settings.CallbackShapes);
		printf("  Alpha      = %d\n", settings.Alpha);
		printf("  Background = (%d, %d, %d, %d)\n", bg.r, bg.g, bg.b, bg.a);
		printf("\n");

		// Settings is not used yet so instead of introducing a memory leak we free it
		free(settings.ImagePath);
	}

	void Initialize(Local<Object> exports) {
		NODE_SET_METHOD(exports, "generate", Generate);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
}

#undef EXP_THROW_EXCEPTION
#undef HAS_OR_THROW_EXCEPTION
#undef IS_OR_THROW_EXCEPTION

#endif