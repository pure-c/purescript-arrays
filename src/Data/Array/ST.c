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

PURS_FFI_FUNC_5(Data_Array_ST_splice, _start, _deleteCount, _items, _arr, _) {
	purs_int_t sz = 0, items_len;
	purs_int_t start = purs_any_force_int(_start);
	purs_int_t delete_count = purs_any_force_int(_deleteCount);
	purs_vec_t *arr = (purs_vec_t*) purs_any_force_array(_arr);
	const purs_vec_t *items = purs_any_force_array(_items);
	purs_vec_t *removed = NULL;
	purs_any_t ret = purs_any_array(removed);

	if (purs_vec_is_empty(arr)) {
		goto end;
	}

	/* determine starting point */
	if (start > arr->length) {
		start = arr->length;
	} else if (start < 0) {
		start = arr->length - start;
		if (start < 0) {
			start = 0;
		}
	}

	/* determine delete count */
	if (delete_count < 0)
		delete_count = 0;
	if (delete_count >= arr->length - start)
		delete_count = arr->length - start; // delete all

	if (delete_count > 0) {
		removed = (purs_vec_t*) purs_vec_new1(delete_count);
		removed->length = delete_count;
		ret = purs_any_array(removed);
		for (int i = start, j = 0; j < delete_count; i++, j++) {
			removed->data[j] = arr->data[i];
			PURS_ANY_RETAIN(removed->data[j]);
		}
	}

	/* determine how many items to delete, or add to adjust space. */
	/* TODO we're taking a semi-efficient approach here:
	   we're overriding the elements we can, however, if there's
	   insufficient space we'll use 'vec_insert' to introduce elements
	   one by one into the middle of the array which may incur a 'realloc'
	   and does incur a 'memmove' per call. A single 'realloc'/'memmove'
	   would be preferable but require an update to purec / vec.h. */
	items_len = purs_vec_length(items);
	delete_count = delete_count - items_len;
	if (delete_count > 0) purs_vec_splice_mut(arr, start, delete_count);
	delete_count = delete_count + items_len;

	for (int i = start, k = 0; k < items_len; i++, k++) {
		if (delete_count > 0) {
			PURS_ANY_RELEASE(arr->data[i]);
			arr->data[i] = items->data[k];
			PURS_ANY_RETAIN(arr->data[i]);
			delete_count--;
			continue;
		}
		purs_vec_insert_mut(&arr, i, items->data[k]);
	}

end:
	PURS_RC_RELEASE(arr);
	PURS_RC_RELEASE(items);
	return ret;
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

	if (purs_vec_is_empty(xs)) {
		PURS_ANY_RETAIN(_xs);
		return _xs;
	}

	for (i = 0; i < xs->length-1; i++) {
		swapped = 0;
		for (j = 0; j < xs->length-i-1; j++) {
			purs_any_t tmp1 = purs_any_app(comp, xs->data[j]);
			purs_any_t r = purs_any_app(tmp1, xs->data[j+1]);
			if (purs_any_force_int(r) > 0) {
				tmp = xs->data[j];
				xs->data[j] = xs->data[j+1];
				xs->data[j+1] = tmp;
				swapped = 1;
			}
			PURS_ANY_RELEASE(r);
			PURS_ANY_RELEASE(tmp1);
		}

		// IF no two elements were swapped by inner loop, then break
		if (swapped == 0) {
			break;
		}
	}

	PURS_ANY_RETAIN(_xs);
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
