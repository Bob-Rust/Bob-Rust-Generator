#ifdef BUILD_NODE
#include <node.h>
#include "borst_object.h"

using namespace v8;

Local<Array> bob_rust_generator::create_borst_list(Isolate* isolate, Model* model, int count) {
	Local<Context> context = isolate->GetCurrentContext();
	Local<Array> array = Array::New(isolate, count);

	for(int i = 0; i < count; i++) {
		Local<Object> obj = Object::New(isolate);
		{
			Circle shape = model->shapes[i];
			Color color = model->colors[i];
			
			int x = shape.x;
			int y = shape.y;
			int s = SIZE_INDEX(shape.r);
			int c = COLOR_INDEX(color);

			obj->Set(context, String::NewFromUtf8(isolate, "x").ToLocalChecked(), Integer::New(isolate, x));
			obj->Set(context, String::NewFromUtf8(isolate, "y").ToLocalChecked(), Integer::New(isolate, y));
			obj->Set(context, String::NewFromUtf8(isolate, "size").ToLocalChecked(), Integer::New(isolate, s));
			obj->Set(context, String::NewFromUtf8(isolate, "color").ToLocalChecked(), Integer::New(isolate, c));
		}

		array->Set(context, i, obj);
	}

	return array;
}

Local<Object> bob_rust_generator::create_borst_object(Isolate* isolate, Model* model, int count) {
	Local<Context> context = isolate->GetCurrentContext();
	Local<Array> list = bob_rust_generator::create_borst_list(isolate, model, count);
	
	Local<Object> obj = Object::New(isolate);
	obj->Set(context, String::NewFromUtf8(isolate, "list").ToLocalChecked(), list);
	obj->Set(context, String::NewFromUtf8(isolate, "width").ToLocalChecked(), Integer::New(isolate, model->width));
	obj->Set(context, String::NewFromUtf8(isolate, "height").ToLocalChecked(), Integer::New(isolate, model->height));

	return obj;
}

#endif
