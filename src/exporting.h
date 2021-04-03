#ifdef BUILD_NODE
#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <uv.h>
#include "borst_object.h"
#include "borst_object.cpp"
#include "recode/bundle.h"
#include "sort/blob_sort.h"

/*
var borst_generator = require('./build/Release/borst_generator');

var COLORS = borst_generator.colors();
var ALPHAS = borst_generator.alphas();
var SIZES = borst_generator.sizes();

var gen = borst_generator.generate({
	path: '<image-path>',
	count: 4000,
	interval: 100,
	alpha: 2,
	width: 0,
	height: 0,
	background: [ <r>, <g>, <b> ]
}, (map) => {
	console.log('Batch');
}, (map) => {
	console.log('Done');
});

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
	if(!(expr)) { \
		EXP_THROW_EXCEPTION(isolate, msg); \
		return; \
	}
#define GET_OR_THROW(isolate, ctx, map, key_name, out_name) \
	Local<Value> __key_##out_name = String::NewFromUtf8(isolate, key_name).ToLocalChecked(); \
	if(!map->Has(ctx, __key_##out_name).FromMaybe(false)) { \
		char message[256] = { 0 }; \
		sprintf_s(message, "Could not find the property '%s' in args[0]", key_name); \
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, message).ToLocalChecked())); \
		return; \
	} \
	Local<Value> out_name = map->Get(ctx, __key_##out_name).ToLocalChecked();

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
	using v8::Function;
	using v8::Persistent;
	
	// TODO: Create a callback function to tell the js app that we have generated the shapes
	typedef Persistent<Function, v8::CopyablePersistentTraits<Function>> PersistentFunc;

	struct BobRustData {
		bool working;
		uv_work_t request;
		PersistentFunc work_callback;
		PersistentFunc done_callback;
		Model* model;
		Settings settings;
	} global_data;

	void worker_send_data(BobRustData* data, PersistentFunc func) {
		Isolate* isolate = Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		Local<Context> ctx = isolate->GetCurrentContext();
		Local<Function> cb = func.Get(isolate);

		Local<Value> args[1];

		// TODO: This could contain a outdated count.. Get the correct count from a parameter!
		int count = data->model->shapes.size();
		args[0] = bob_rust_generator::create_borst_object(isolate, data->model, count);
		cb->Call(ctx, v8::Null(isolate), 1, args);
	}
	
	void worker_close_async(uv_handle_t* handle) {
		free(handle);
	}

	void worker_send_async(uv_async_t* handle) {
		BobRustData* data = static_cast<BobRustData*>(handle->data);
		worker_send_data(data, data->work_callback);
		uv_close((uv_handle_t*)handle, worker_close_async);
	}

	void worker_generate(uv_work_t* req) {
		BobRustData* data = static_cast<BobRustData*>(req->data);
		Settings settings = data->settings;

		Image* image = BorstLoadImage(settings.ImagePath, settings.Width, settings.Height);
		if(!image) {
			delete image;
			return;
		}
			
		const int Count = settings.MaxShapes;
		const int Callb = settings.CallbackShapes;
		const int Alpha = ARR_ALPHAS[settings.Alpha];

		Model* model = new Model(image, settings.Background, Alpha);
		data->model = model;
			
		for(int i = 0; i <= Count; i++) {
			int n = model->Step();

			if((i % Callb) == 0) {
				uv_async_t* job = (uv_async_t*)malloc(sizeof(uv_async_t));
				job->data = data;
				uv_async_init(uv_default_loop(), job, worker_send_async);
				uv_async_send(job);
			}
		}
			
		delete image;
	}

	void worker_generate_after(uv_work_t* req, int status) {
		BobRustData* data = static_cast<BobRustData*>(req->data);
		Settings settings = data->settings;

		bobrust::sort_blob_list(data->model);
		worker_send_data(data, data->done_callback);
		
		// Settings is not used yet so instead of introducing a memory leak we free it
		free(settings.ImagePath);

		data->work_callback.Reset();
		data->done_callback.Reset();
		
		// Free memory
		delete data->model;
		delete data;
	}

	void Generate(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		
		if(args.Length() != 3) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
			return;
		}

		if(!args[0]->IsObject()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Expected a map object args[0]").ToLocalChecked()));
			return;
		}

		if(!args[1]->IsFunction()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Expected a function args[1]").ToLocalChecked()));
			return;
		}

		if(!args[2]->IsFunction()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Expected a function args[2]").ToLocalChecked()));
			return;
		}
		
		Local<Context> context = isolate->GetCurrentContext();
		Local<Object> options = args[0]->ToObject(context).ToLocalChecked();

		GET_OR_THROW(isolate, context, options, "path", m_opt_path);
		GET_OR_THROW(isolate, context, options, "count", m_opt_count);
		GET_OR_THROW(isolate, context, options, "interval", m_opt_interval);
		GET_OR_THROW(isolate, context, options, "alpha", m_opt_alpha);
		GET_OR_THROW(isolate, context, options, "background", m_opt_bg);
		GET_OR_THROW(isolate, context, options, "width", m_opt_width);
		GET_OR_THROW(isolate, context, options, "height", m_opt_height);

		IS_OR_THROW_EXCEPTION(isolate, m_opt_path->IsString(), "The property 'path' was not a string")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_count->IsInt32(), "The property 'count' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_interval->IsInt32(), "The property 'interval' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_alpha->IsInt32(), "The property 'alpha' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_width->IsInt32(), "The property 'width' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_height->IsInt32(), "The property 'height' was not an integer")
		IS_OR_THROW_EXCEPTION(isolate, m_opt_bg->IsArray(), "The property 'background' was not an array")
		
		Color bg{ 0, 0, 0, 0xff };
		{
			Local<v8::Array> opt_bg = m_opt_bg->ToObject(context).ToLocalChecked().As<v8::Array>();
			IS_OR_THROW_EXCEPTION(isolate, opt_bg->Length() == 3, "The property 'background' was not a color [ r, g, b, ]")

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
		
		Settings settings;
		String::Utf8Value str(isolate, m_opt_path->ToString(context).ToLocalChecked());
		settings.ImagePath = _strdup(*str);
		settings.MaxShapes = (int)m_opt_count->IntegerValue(context).FromMaybe(0);
		settings.Alpha = (int)m_opt_alpha->IntegerValue(context).FromMaybe(0);
		settings.CallbackShapes = (int)m_opt_interval->IntegerValue(context).FromMaybe(0);
		settings.Width = (int)m_opt_width->IntegerValue(context).FromMaybe(0);
		settings.Height = (int)m_opt_height->IntegerValue(context).FromMaybe(0);
		settings.Background = bg;

		
		BobRustData* data = new BobRustData();
		data->request.data = data;
		data->settings = settings;
		data->work_callback.Reset(isolate, Local<Function>::Cast(args[1]));
		data->done_callback.Reset(isolate, Local<Function>::Cast(args[2]));

		uv_queue_work(uv_default_loop(), &data->request, worker_generate, worker_generate_after);
	}

	void GetAlphas(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		Local<Context> context = isolate->GetCurrentContext();
		Local<Array> array = Array::New(isolate, NUM_ALPHAS);
		for(int i = 0; i < NUM_ALPHAS; i++) array->Set(context, i, Integer::New(isolate, ARR_ALPHAS[i]));
		args.GetReturnValue().Set(array);
	}

	void GetColors(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		Local<Context> context = isolate->GetCurrentContext();
		Local<Array> array = Array::New(isolate, NUM_COLORS);
		for(int i = 0; i < NUM_COLORS; i++) {
			const Color c = ARR_COLORS[i];
			Local<Array> rgb = Array::New(isolate, 3);
			rgb->Set(context, 0, Integer::New(isolate, c.r));
			rgb->Set(context, 1, Integer::New(isolate, c.g));
			rgb->Set(context, 2, Integer::New(isolate, c.b));
			array->Set(context, i, rgb);
		}
		args.GetReturnValue().Set(array);
	}

	void GetSizes(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();
		Local<Context> context = isolate->GetCurrentContext();
		Local<Array> array = Array::New(isolate, NUM_SIZES);
		for(int i = 0; i < NUM_SIZES; i++) array->Set(context, i, Integer::New(isolate, ARR_SIZES[i]));
		args.GetReturnValue().Set(array);
	}

	void Initialize(Local<Object> exports) {
		NODE_SET_METHOD(exports, "generate", Generate);
		NODE_SET_METHOD(exports, "alphas", GetAlphas);
		NODE_SET_METHOD(exports, "colors", GetColors);
		NODE_SET_METHOD(exports, "sizes", GetSizes);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
}

#undef EXP_THROW_EXCEPTION
#undef HAS_OR_THROW_EXCEPTION
#undef IS_OR_THROW_EXCEPTION
#undef GET_OR_THROW

#endif
