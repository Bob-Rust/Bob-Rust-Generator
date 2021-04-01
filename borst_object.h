#ifndef __BORST_OBJECT_H__
#define __BORST_OBJECT_H__

#include <vector>
#include <node.h>
#include "utils.h"
#include "recode/model.h"

namespace bob_rust_generator {
	// v8::Local<v8::Value> create_borst_object(v8::Isolate* isolate, Circle shape, Color color);
	v8::Local<v8::Array> create_borst_list(v8::Isolate* isolate, Model* model);
}

#endif