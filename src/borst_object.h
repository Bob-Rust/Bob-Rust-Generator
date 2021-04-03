#ifndef __BORST_OBJECT_H__
#define __BORST_OBJECT_H__

#include <vector>
#include <node.h>
#include "utils.h"
#include "recode/model.h"
#include "recode/bundle.h"

namespace bob_rust_generator {
	v8::Local<v8::Array> create_borst_list(v8::Isolate* isolate, Model* model, int count);
	v8::Local<v8::Object> create_borst_object(v8::Isolate* isolate, Model* model, int count);
}

#endif
