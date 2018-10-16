#include <purescript.h>

PURS_FFI_FUNC_1(Data_Array_ST_empty, _, {
	return purs_any_array_new(purs_vec_new());
});

PURS_FFI_FUNC_5(Data_Array_ST_peekImpl, just, nothing, _i, _xs, _, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	const purs_any_int_t i = purs_any_get_int(_i);
	if (i >= 0 && i< xs->length) {
		return purs_any_app(just, xs->data[i]);
	} else {
		return nothing;
	}
});

PURS_FFI_FUNC_4(Data_Array_ST_poke, _i, a, _xs, _, {
	purs_vec_t * xs = (purs_vec_t *) purs_any_get_array(_xs);
	const purs_any_int_t i = purs_any_get_int(_i);
	if (i >= 0 && i < xs->length) {
		xs->data[i] = a;
		return purs_any_true;
		} else {
			return purs_any_false;
		}
});

PURS_FFI_FUNC_3(Data_Array_ST_pushAll, _as, _xs, _, {
	purs_vec_t * xs = (purs_vec_t *) purs_any_get_array(_xs);
	const purs_vec_t * as = purs_any_get_array(_as);
	const purs_any_t * tmp;
	purs_any_int_t i;
	purs_vec_foreach(as, tmp, i) {
		purs_vec_push_mut(xs, tmp);
	}
	return purs_any_int_new(as -> length);
});

PURS_FFI_FUNC_5(Data_Array_ST_splice, _i, _howMany, _bs, _xs, _, {
	const purs_any_int_t i = purs_any_get_int(_i);
	const purs_any_int_t howMany = purs_any_get_int(_howMany);
	const purs_vec_t * bs = purs_any_get_array(_bs);
	purs_vec_t * xs = (purs_vec_t *) purs_any_get_array(_xs);
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	purs_vec_t * head = (purs_vec_t *) purs_vec_new();
	purs_vec_t * tail = (purs_vec_t *) purs_vec_new();
	const purs_any_t * tmp;
	purs_any_int_t j;
	purs_vec_foreach(xs, tmp, j) {
		if (j >= i) {
			if (out->length < howMany) {
				purs_vec_push_mut(out, tmp);
			} else {
				purs_vec_push_mut(tail, tmp);
			}
		} else {
			purs_vec_push_mut(head, tmp);
		}
	}
	purs_vec_reserve(xs, head->length + tail->length + bs->length);
	purs_vec_foreach(bs, tmp, j) {
		xs->data[head->length + j] = tmp;
	}
	purs_vec_foreach(tail, tmp, j) {
		xs->data[head->length + bs->length + j] = tmp;
	}
	xs->length = head->length + tail->length + bs->length;
	return purs_any_array_new(out);
});

PURS_FFI_FUNC_2(Data_Array_ST_copyImpl, _xs, _, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	return purs_any_array_new(purs_vec_copy(xs));
});

PURS_FFI_FUNC_3(Data_Array_ST_sortByImpl, comp, _xs, _, {
	purs_vec_t * xs = (purs_vec_t *) purs_any_get_array(_xs);
	purs_any_int_t i;
	purs_any_int_t j;
	purs_any_int_t swapped;
	const purs_any_t * tmp;

	for (i = 0; i < xs->length-1; i++) {
		swapped = 0;
		for (j = 0; j < xs->length-i-1; j++) {
		const purs_any_t * r = purs_any_app(purs_any_app(comp, xs->data[j]), xs->data[j+1]);
			if (purs_any_get_int(r) > 0) {
				tmp = xs->data[j];
				xs->data[j] = xs->data[j+1];
				xs->data[j+1] = tmp;
				swapped = 1;
			}
		}

		// IF no two elements were swapped by inner loop, then break
		if (swapped == 0) {
			break;
		}
	}

	return purs_any_array_new(xs);
});

PURS_FFI_FUNC_2(Data_Array_ST_toAssocArray, _xs, _, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	purs_any_int_t i;
	const purs_any_t * tmp;
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	purs_vec_foreach(xs, tmp, i) {
	purs_vec_push_mut(out, purs_any_record_new(
				purs_record_new_from_kvps(2, "value", tmp, "index", purs_any_int_new(i))
				));
	}
	return purs_any_array_new(out);
});
