#include <purescript.h>

PURS_FFI_FUNC_1(Data_Array_ST_empty, _) {
	return purs_any_array(purs_vec_new());
}

PURS_FFI_FUNC_5(Data_Array_ST_peekImpl, just, nothing, _i, _xs, _) {
	const purs_vec_t *xs = purs_any_unsafe_get_array(_xs);
	purs_int_t i = purs_any_force_int(_i);
	if (i >= 0 && i< xs->length) {
		return purs_any_app(just, xs->data[i]);
	}
	return nothing;
}

PURS_FFI_FUNC_4(Data_Array_ST_poke, _i, a, _xs, _) {
	purs_vec_t *xs = (purs_vec_t *) purs_any_unsafe_get_array(_xs);
	purs_int_t i = purs_any_force_int(_i);
	if (i >= 0 && i < xs->length) {
		PURS_ANY_RETAIN(a);
		xs->data[i] = a;
		return purs_any_true;
	}
	return purs_any_false;
}

PURS_FFI_FUNC_3(Data_Array_ST_pushAll, _as, _xs, _) {
	purs_vec_t *xs = (purs_vec_t *) purs_any_unsafe_get_array(_xs);
	const purs_vec_t *as = purs_any_force_array(_as);
	purs_any_t tmp;
	purs_int_t i;
	purs_vec_foreach(as, tmp, i) {
		PURS_ANY_RETAIN(tmp);
		purs_vec_push_mut(xs, tmp);
	}
	PURS_RC_RELEASE(as);
	return purs_any_int(xs -> length);
}

PURS_FFI_FUNC_5(Data_Array_ST_splice, _i, _howMany, _bs, _xs, _) {
	purs_int_t i = purs_any_force_int(_i);
	purs_int_t howMany = purs_any_force_int(_howMany);
	const purs_vec_t *bs = purs_any_force_array(_bs);
	purs_vec_t *xs = (purs_vec_t *) purs_any_unsafe_get_array(_xs);
	purs_vec_t *removed = (purs_vec_t *) purs_vec_new();
	purs_vec_t *head = (purs_vec_t *) purs_vec_new();
	purs_vec_t *tail = (purs_vec_t *) purs_vec_new();
	purs_any_t tmp;
	purs_int_t j;
	purs_vec_foreach(xs, tmp, j) {
		if (j >= i) {
			if (removed->length < howMany) {
				purs_vec_push_mut(removed, tmp);
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
	PURS_RC_RELEASE(bs);
	PURS_RC_RELEASE(head);
	PURS_RC_RELEASE(tail);
	return purs_any_array(removed);
}

PURS_FFI_FUNC_2(Data_Array_ST_copyImpl, _xs, _) {
	const purs_vec_t *xs = purs_any_unsafe_get_array(_xs);
	return purs_any_array(purs_vec_copy(xs));
}

PURS_FFI_FUNC_3(Data_Array_ST_sortByImpl, comp, _xs, _) {
	purs_vec_t *xs = (purs_vec_t *) purs_any_unsafe_get_array(_xs);
	purs_int_t i;
	purs_int_t j;
	purs_int_t swapped;
	purs_any_t tmp;

	for (i = 0; i < xs->length-1; i++) {
		swapped = 0;
		for (j = 0; j < xs->length-i-1; j++) {
			purs_any_t r = purs_any_app(purs_any_app(comp, xs->data[j]), xs->data[j+1]);
			if (purs_any_force_int(r) > 0) {
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

	return _xs;
}

PURS_FFI_FUNC_2(Data_Array_ST_toAssocArray, _xs, _) {
	const purs_vec_t *xs = purs_any_unsafe_get_array(_xs);
	purs_int_t i;
	purs_any_t tmp;
	purs_vec_t *out = (purs_vec_t *) purs_vec_new();
	purs_vec_foreach(xs, tmp, i) {
		purs_vec_push_mut(
			out,
			purs_any_record(
				purs_record_new_va(
					2,
					"value", tmp,
					"index", purs_any_int(i))));
	}
	return purs_any_array(out);
}
