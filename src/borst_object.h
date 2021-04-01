#ifndef __BORST_OBJECT_H__
#define __BORST_OBJECT_H__

#include <vector>
#include <node.h>
#include "utils.h"
#include "recode/model.h"
#include "recode/bundle.h"

namespace bob_rust_generator {
	struct BorstCallback {
		Settings settings;
		v8::Persistent<v8::Function> callback;
	};

	v8::Local<v8::Array> create_borst_list(v8::Isolate* isolate, Model* model, int count);
}

#endif