#include <purescript.h>

//------------------------------------------------------------------------------
// Array creation --------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_range, _start, _end, {
	const purs_any_int_t start = purs_any_get_int(_start);
	const purs_any_int_t end = purs_any_get_int(_end);
	purs_any_int_t step = start > end ? -1 : 1;
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	purs_any_int_t i = start;
	while (i != end) {
		purs_vec_push_mut(result, purs_any_int_new(i));
		i += step;
	}
	purs_vec_push_mut(result, purs_any_int_new(i));
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_2(Data_Array_replicate, _count, value, {
	const purs_any_int_t count = purs_any_get_int(_count);
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = 0; i < count; i++) {
		purs_vec_push_mut(result, value);
	}
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_2(Data_Array_curryCons, head, tail, {
	return purs_any_record_new(
		purs_record_new_from_kvps(2, "head", head, "tail", tail));
});

PURS_FFI_FUNC_1(Data_Array_listToArray, list, {
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	purs_any_t * xs = (purs_any_t *) list;
	while (xs != NULL) {
		const purs_record_t * record = purs_any_get_record(xs);
		const purs_any_t * head = purs_record_find_by_key(record, "head")->value;
		const purs_any_t * tail = purs_record_find_by_key(record, "tail")->value;
		purs_vec_push_mut(result, head);
		xs = (purs_any_t *) tail;
	}
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_2(Data_Array_fromFoldableImpl, foldr, xs, {
	return purs_any_app(Data_Array_listToArray$,
				purs_any_app(
					purs_any_app(
						purs_any_app(foldr, Data_Array_curryCons$),
						NULL
					),
					xs
				)
		 );
});

//------------------------------------------------------------------------------
// Array size ------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_1(Data_Array_length, _xs, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	return purs_any_int_new(xs->length);
});

//------------------------------------------------------------------------------
// Extending arrays ------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_cons, e, l, {
	return purs_any_array_new(purs_vec_insert(purs_any_get_array(l), 0, e));
});

PURS_FFI_FUNC_2(Data_Array_snoc, _l, e, {
	const purs_vec_t * l = purs_any_get_array(_l);
	return purs_any_array_new(purs_vec_insert(l, l->length, e));
});

//------------------------------------------------------------------------------
// Non-indexed reads -----------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_uncons$, empty, next, _xs, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	if (xs->length == 0) {
		return purs_any_app(empty, NULL);
	} else {
		return purs_any_app(purs_any_app(next, xs->data[0]), purs_any_array_new(purs_vec_slice(xs, 1)));
	}
});

//------------------------------------------------------------------------------
// Indexed operations ----------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_4(Data_Array_indexImpl, just, nothing, _xs, _i, {
	const purs_any_int_t i = purs_any_get_int(_i);
	const purs_vec_t * xs = purs_any_get_array(_xs);
	if (i < 0 || i >= xs->length) {
		return nothing;
	} else {
		return purs_any_app(just, xs->data[i]);
	}
});

PURS_FFI_FUNC_4(Data_Array_findIndexImpl, just, nothing, f, _xs, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	for (purs_any_int_t i = 0; i < xs->length; i++) {
		if (purs_any_app(f, xs->data[i]) == purs_any_true) {
			return purs_any_app(just, purs_any_int_new(i));
		}
	}
	return nothing;
});

PURS_FFI_FUNC_4(Data_Array_findLastIndexImpl, just, nothing, f, _xs, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	for (purs_any_int_t i = xs->length - 1; i >= 0; i--) {
		if (purs_any_app(f, xs->data[i]) == purs_any_true) {
			return purs_any_app(just, purs_any_int_new(i));
		}
	}
	return nothing;
});

PURS_FFI_FUNC_5(Data_Array__insertAt, just, nothing, _i, a, _l, {
	const purs_any_int_t i = purs_any_get_int(_i);
	const purs_vec_t * l = purs_any_get_array(_l);
	if (i < 0 || i > l->length) {
		return nothing;
	}
	return purs_any_app(just, purs_any_array_new(purs_vec_insert(l, i, a)));
});

PURS_FFI_FUNC_4(Data_Array__deleteAt, just, nothing, _i, _l, {
	const purs_any_int_t i = purs_any_get_int(_i);
	const purs_vec_t * l = purs_any_get_array(_l);
	if (i < 0 || i >= l->length) {
		return nothing;
	}
	purs_vec_t * l1 = (purs_vec_t *) purs_vec_copy(l);
	vec_splice(l1, i, 1);
	return purs_any_app(just, purs_any_array_new(l1));
});

PURS_FFI_FUNC_5(Data_Array__updateAt, just, nothing, _i, a, _l, {
	const purs_any_int_t i = purs_any_get_int(_i);
	const purs_vec_t * l = purs_any_get_array(_l);
	if (i < 0 || i >= l->length) {
		return nothing;
	}
	purs_vec_t * l1 = (purs_vec_t *) purs_vec_copy(l);
	l1->data[i] = a;
	return purs_any_app(just, purs_any_array_new(l1));
});

//------------------------------------------------------------------------------
// Transformations -------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_1(Data_Array_reverse, _l, {
	const purs_vec_t * l = purs_any_get_array(_l);
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = l -> length - 1; i >= 0; i--) {
		purs_vec_push_mut(result, l->data[i]);
	}
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_1(Data_Array_concat, xss, {
	purs_any_int_t i;
	purs_any_int_t j;
	const purs_any_t * xs;
	const purs_any_t * tmp;
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	purs_vec_foreach(purs_any_get_array(xss), xs, i) {
		purs_vec_foreach(purs_any_get_array(xs), tmp, j) {
			purs_vec_push_mut(result, tmp);
		}
	}
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_2(Data_Array_filter, f, xs, {
	const purs_any_t * tmp;
	purs_any_int_t i;
	purs_vec_t * result = (purs_vec_t *) purs_vec_new();
	purs_vec_foreach(purs_any_get_array(xs), tmp, i) {
		if (purs_any_app(f, tmp) == purs_any_true) {
			purs_vec_push_mut(result, tmp);
		}
	}
	return purs_any_array_new(result);
});

PURS_FFI_FUNC_2(Data_Array_partition, f, xs, {
	purs_vec_t * yes = (purs_vec_t *) purs_vec_new();
	purs_vec_t * no = (purs_vec_t *) purs_vec_new();
	purs_any_int_t i;
	const purs_any_t * tmp;
	purs_vec_foreach(purs_any_get_array(xs), tmp, i) {
		if (purs_any_app(f, tmp) == purs_any_true) {
			purs_vec_push_mut(yes, tmp);
		} else {
			purs_vec_push_mut(no, tmp);
		}
	}
	return purs_any_record_new(
		purs_record_new_from_kvps(2,
			"yes", purs_any_array_new(yes),
			"no", purs_any_array_new(no)));
});

//------------------------------------------------------------------------------
// Sorting ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_sortImpl, f, _l, {
	purs_vec_t * l = (purs_vec_t *) purs_vec_copy(purs_any_get_array(_l));
	purs_any_int_t i;
	purs_any_int_t j;
	purs_any_int_t swapped;
	const purs_any_t * tmp;

	for (i = 0; i < l->length-1; i++) {
		swapped = 0;
		for (j = 0; j < l->length-i-1; j++) {
		const purs_any_t * r = purs_any_app(purs_any_app(f, l->data[j]), l->data[j+1]);
			if (purs_any_get_int(r) > 0) {
				tmp = l->data[j];
				l->data[j] = l->data[j+1];
				l->data[j+1] = tmp;
				swapped = 1;
			}
		}

		// IF no two elements were swapped by inner loop, then break
		if (swapped == 0) {
			break;
		}
	}

	return purs_any_array_new(l);
});

//------------------------------------------------------------------------------
// Subarrays -------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_slice, _s, _e, _l, {
	const purs_any_int_t s = purs_any_get_int(_s);
	const purs_any_int_t e = purs_any_get_int(_e);
	const purs_vec_t * l = purs_any_get_array(_l);
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = s; i < e; i ++) {
		if (i >= 0 && i < l->length) {
			purs_vec_push_mut(out, l->data[i]);
		}
	}
	return purs_any_array_new(out);
});

PURS_FFI_FUNC_2(Data_Array_take, _n, _l, {
	const purs_any_int_t n = purs_any_get_int(_n);
	const purs_vec_t * l = purs_any_get_array(_l);
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = 0; i < n; i ++) {
		if (i >= 0 && i < l->length) {
			purs_vec_push_mut(out, l->data[i]);
		}
	}
	return purs_any_array_new(out);
});

PURS_FFI_FUNC_2(Data_Array_drop, _n, _l, {
	const purs_any_int_t n = purs_any_get_int(_n);
	const purs_vec_t * l = purs_any_get_array(_l);
	if (n < 1) {
		return _l;
	}
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = n; i < l->length; i++) {
		purs_vec_push_mut(out, l->data[i]);
	}
	return purs_any_array_new(out);
});

//------------------------------------------------------------------------------
// Zipping ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_zipWith, f, _xs, _ys, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	const purs_vec_t * ys = purs_any_get_array(_ys);
	purs_any_int_t l = xs->length < ys->length ? xs->length : ys->length;
	purs_vec_t * out = (purs_vec_t *) purs_vec_new();
	for (purs_any_int_t i = 0; i < l; i++) {
		purs_vec_push_mut(out, purs_any_app(purs_any_app(f, xs->data[i]), ys->data[i]));
	}
	return purs_any_array_new(out);
});

//------------------------------------------------------------------------------
// Partial ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_unsafeIndexImpl, _xs, _n, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	const purs_any_int_t n = purs_any_get_int(_n);
	return xs->data[n];
});
