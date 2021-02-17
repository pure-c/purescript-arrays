#include <purescript.h>

//------------------------------------------------------------------------------
// Array creation --------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_range, _start, _end) {
	purs_int_t start = purs_any_force_int(_start);
	purs_int_t end = purs_any_force_int(_end);
	purs_int_t step = start > end ? -1 : 1;
	int sz = step * (end - start) + 1;
	if (sz <= 0) {
		return purs_any_array_empty;
	}
	purs_vec_t *result = (purs_vec_t *) purs_vec_new1(sz);
	purs_int_t i = start, n = 0;
	while (i != end) {
		result->data[n++] = purs_any_int(i);
		i += step;
	}
	result->data[n] = purs_any_int(i);
	result->length = n + 1;
	return purs_any_array(result);
}

PURS_FFI_FUNC_2(Data_Array_replicate, _count, value) {
	purs_int_t count = purs_any_force_int(_count);
	if (count <= 0) {
		return purs_any_array_empty;
	}

	purs_vec_t *result = (purs_vec_t *) purs_vec_new1(count);
	for (purs_int_t i = 0; i < count; i++) {
		result->data[i] = value;
		PURS_ANY_RETAIN(value);
	}
	result->length = count;
	return purs_any_array(result);
}

/* build a cons list from an array [X, [X, ...]] */
PURS_FFI_FUNC_2(Data_Array_curryCons, head, tail) {
	return purs_any_array(purs_vec_new_va(2, head, tail));
}

PURS_FFI_FUNC_2(Data_Array_fromFoldableImpl, foldr, xs) {
	int i, sz;
	purs_vec_t *result;
	purs_any_t cur, tmp1, tmp2, cons, ret = purs_any_array_empty;

	tmp1 = purs_any_app(foldr, Data_Array_curryCons);
	tmp2 = purs_any_app(tmp1, purs_any_array_empty);
	cons = purs_any_app(tmp2, xs);

	if (purs_vec_is_empty(purs_any_unsafe_get_array(cons))) {
		goto end;
	}

	/* first trip through the linked list to find size */
	sz = 0;
	cur = cons;
	while (1) {
		const purs_vec_t *xs = purs_any_unsafe_get_array(cur);
		if (purs_vec_is_empty(xs)) break;
		sz++;
		cur = xs->data[1];
	}

	/* should never hit, but just in case. */
	if (sz == 0) goto end;

	/* second trip through the linked to copy values and free list. */
	result = (purs_vec_t*) purs_vec_new1(sz);
	result->length = sz;
	cur = cons;
	i = 0;
	while (1) {
		purs_any_t head;
		const purs_vec_t *xs = purs_any_unsafe_get_array(cur);
		if (purs_vec_is_empty(xs)) break;
		head = xs->data[0];
		PURS_ANY_RETAIN(head);
		result->data[i] = head;
		cur = xs->data[1];
		i++;
	}
	ret = purs_any_array(result);

 end:
	PURS_ANY_RELEASE(cons);
	PURS_ANY_RELEASE(tmp2);
	PURS_ANY_RELEASE(tmp1);

	return ret;
}

//------------------------------------------------------------------------------
// Array size ------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_1(Data_Array_length, _xs) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_int(0);
	}
	purs_any_t ret = purs_any_int(xs->length);
	PURS_RC_RELEASE(xs);
	return ret;
}

//------------------------------------------------------------------------------
// Extending arrays ------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_cons, e, _xs) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array(purs_vec_new_va(1, e));
	}
	purs_any_t ret = purs_any_array(purs_vec_insert(xs, 0, e));
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_2(Data_Array_snoc, _xs, e) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array(purs_vec_new_va(1, e));
	}
	purs_any_t ret = purs_any_array(purs_vec_insert(xs, xs->length, e));
	PURS_RC_RELEASE(xs);
	return ret;
}

//------------------------------------------------------------------------------
// Non-indexed reads -----------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_uncons$, empty, next, _xs) {
	purs_any_t tmp1, tail, ret;
	const purs_vec_t *xs = purs_any_force_array(_xs);
	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_app(empty, purs_any_null);
	}

	if (xs->length == 1) {
		tail = purs_any_array_empty;
	} else {
		purs_vec_t *result = (purs_vec_t*) purs_vec_new1(xs->length - 1);
		result->length = xs->length - 1;
		tail = purs_any_array(result);
		for (int i = 0; i < xs->length - 1; i++) {
			result->data[i] = xs->data[i+1];
			PURS_ANY_RETAIN(result->data[i]);
		}
	}

	tmp1 = purs_any_app(next, xs->data[0]);
	ret = purs_any_app(tmp1, tail);
	PURS_ANY_RELEASE(tmp1);
	PURS_ANY_RELEASE(tail);
	PURS_RC_RELEASE(xs);
	return ret;
}

//------------------------------------------------------------------------------
// Indexed operations ----------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_4(Data_Array_indexImpl, just, nothing, _xs, _i) {
	purs_any_t ret;
	purs_int_t i = purs_any_force_int(_i);
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	if (i < 0 || i >= xs->length) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	ret = purs_any_app(just, xs->data[i]);
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_4(Data_Array_findIndexImpl, just, nothing, f, _xs) {
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	for (int i = 0; i < xs->length; i++) {
		purs_any_t tmp1 = purs_any_app(f, xs->data[i]);
		if (purs_any_eq(tmp1, purs_any_true)) {
			PURS_ANY_RELEASE(tmp1);
			PURS_RC_RELEASE(xs);
			return purs_any_app(just, purs_any_int(i));
		}
		PURS_ANY_RELEASE(tmp1);
	}

	PURS_RC_RELEASE(xs);
	return nothing;
}

PURS_FFI_FUNC_4(Data_Array_findLastIndexImpl, just, nothing, f, _xs) {
	purs_any_t ret = nothing;
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		goto end;
	}

	for (int i = xs->length - 1; i >= 0; i--) {
		purs_any_t tmp1 = purs_any_app(f, xs->data[i]);
		if (purs_any_eq(tmp1, purs_any_true)) {
			ret = purs_any_app(just, purs_any_int(i));
			PURS_ANY_RELEASE(tmp1);
			goto end;
		}
		PURS_ANY_RELEASE(tmp1);
	}

 end:
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_5(Data_Array__insertAt, just, nothing, _i, a, _xs) {
	purs_any_t ret, tmp1;
	purs_int_t i = purs_any_force_int(_i);
	const purs_vec_t *xs = purs_any_force_array(_xs);

	// special case insert '0' into empty array
	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		if (i == 0) {
			tmp1 = purs_any_array(purs_vec_new_va(1, a));
			ret = purs_any_app(just, tmp1);
			PURS_ANY_RELEASE(tmp1);
			return ret;
		}
		return nothing;
	}

	if (i < 0 || i > xs->length) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	tmp1 = purs_any_array(purs_vec_insert(xs, i, a));
	ret = purs_any_app(just, tmp1);
	PURS_ANY_RELEASE(tmp1);
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_4(Data_Array__deleteAt, just, nothing, _i, _xs) {
	purs_any_t ret;
	purs_int_t i = purs_any_force_int(_i);
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	if (i < 0 || i >= xs->length) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	purs_vec_t *l1 = (purs_vec_t *) purs_vec_copy(xs);
	vec_splice(l1, i, 1);

	ret = purs_any_app(just, purs_any_array(l1));
	PURS_RC_RELEASE(xs);
	PURS_RC_RELEASE(l1);
	return ret;
}

PURS_FFI_FUNC_5(Data_Array__updateAt, just, nothing, _i, a, _xs) {
	purs_any_t ret;
	purs_int_t i = purs_any_force_int(_i);
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	if (i < 0 || i >= xs->length) {
		PURS_RC_RELEASE(xs);
		return nothing;
	}

	purs_vec_t *l1 = (purs_vec_t *) purs_vec_copy(xs);
	l1->data[i] = a;
	ret = purs_any_app(just, purs_any_array(l1));
	PURS_RC_RELEASE(xs);
	PURS_RC_RELEASE(l1);
	return ret;
}

//------------------------------------------------------------------------------
// Transformations -------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_1(Data_Array_reverse, _xs) {
	purs_vec_t *result;
	const purs_vec_t *xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array_empty;
	}

	result = (purs_vec_t *) purs_vec_new();
	for (int i = xs->length - 1; i >= 0; i--) {
		PURS_ANY_RETAIN(xs->data[i]);
		purs_vec_push_mut(result, xs->data[i]);
	}

	PURS_RC_RELEASE(xs);
	return purs_any_array(result);
}

PURS_FFI_FUNC_1(Data_Array_concat, _xss) {
	purs_int_t i;
	purs_int_t j;
	purs_any_t xs;
	purs_any_t tmp;
	purs_vec_t *result = (purs_vec_t *) purs_vec_new();
	const purs_vec_t *xss = purs_any_force_array(_xss);
	purs_vec_foreach(xss, xs, i) {
		const purs_vec_t *tmp1 = purs_any_force_array(xs);
		purs_vec_foreach(tmp1, tmp, j) {
			PURS_ANY_RETAIN(tmp);
			purs_vec_push_mut(result, tmp);
		}
		PURS_RC_RELEASE(tmp1);
	}
	PURS_RC_RELEASE(xss);
	return purs_any_array(result);
}

PURS_FFI_FUNC_2(Data_Array_filter, f, _xs) {
	purs_any_t x;
	purs_int_t i;
	purs_vec_t *result = (purs_vec_t *) purs_vec_new();
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_vec_foreach(xs, x, i) {
		purs_any_t tmp = purs_any_app(f, x);
		if (purs_any_eq(tmp, purs_any_true)) {
			purs_vec_push_mut(result, x);
		}
	}
	PURS_RC_RELEASE(xs);
	return purs_any_array(result);
}

PURS_FFI_FUNC_2(Data_Array_partition, f, _xs) {
	purs_vec_t *yes = purs_vec_empty;
	purs_vec_t *no = purs_vec_empty;
	purs_int_t i;
	purs_any_t x;

	const purs_vec_t* xs = purs_any_force_array(_xs);
	if (purs_vec_is_empty(xs)) {
		goto end;
	}

	yes = (purs_vec_t *) purs_vec_new();
	no = (purs_vec_t *) purs_vec_new();

	purs_any_t tmp1;
	purs_vec_foreach(xs, x, i) {
		tmp1 = purs_any_app(f, x);
		if (purs_any_eq(tmp1, purs_any_true)) {
			PURS_ANY_RETAIN(x);
			purs_vec_push_mut(yes, x);
		} else {
			PURS_ANY_RETAIN(x);
			purs_vec_push_mut(no, x);
		}
		PURS_ANY_RELEASE(tmp1);
	}

 end:
	PURS_RC_RELEASE(xs);
	return purs_any_record(
		purs_record_new_va(2,
			"yes", purs_any_array(yes),
			"no", purs_any_array(no)));
}

//------------------------------------------------------------------------------
// Sorting ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_sortImpl, f, _xs) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_vec_t *copy = (purs_vec_t *) purs_vec_copy(xs);
	purs_int_t i;
	purs_int_t j;
	purs_int_t swapped;
	purs_any_t tmp;

	for (i = 0; i < copy->length-1; i++) {
		swapped = 0;
		for (j = 0; j < copy->length-i-1; j++) {
			purs_any_t tmp1 = purs_any_app(f, copy->data[j]);
			purs_any_t r = purs_any_app(tmp1, copy->data[j+1]);

			if (purs_any_force_int(r) > 0) {
				tmp = copy->data[j];
				copy->data[j] = copy->data[j+1];
				copy->data[j+1] = tmp;
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

	PURS_RC_RELEASE(xs);
	return purs_any_array(copy);
}

//------------------------------------------------------------------------------
// Subarrays -------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_slice, _s, _e, _xs) {
	purs_int_t s = purs_any_force_int(_s);
	purs_int_t e = purs_any_force_int(_e);
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_vec_t *result;

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array_empty;
	}

	result = (purs_vec_t *) purs_vec_new();
	for (purs_int_t i = s; i < e; i ++) {
		if (i >= 0 && i < xs->length) {
			PURS_ANY_RETAIN(xs->data[i]);
			purs_vec_push_mut(result, xs->data[i]);
		}
	}

	PURS_RC_RELEASE(xs);
	return purs_any_array(result);
}

PURS_FFI_FUNC_2(Data_Array_take, _n, _xs) {
	purs_int_t n = purs_any_force_int(_n);
	if (n < 1) {
		return purs_any_array_empty;
	}

	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_vec_t *result;

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array_empty;
	}

	result = (purs_vec_t *) purs_vec_new();
	for (purs_int_t i = 0; i < n; i ++) {
		if (i >= 0 && i < xs->length) {
			PURS_ANY_RETAIN(xs->data[i]);
			purs_vec_push_mut(result, xs->data[i]);
		}
	}

	PURS_RC_RELEASE(xs);
	return purs_any_array(result);
}

PURS_FFI_FUNC_2(Data_Array_drop, _n, _xs) {
	purs_int_t n = purs_any_force_int(_n);
	const purs_vec_t *xs;
	purs_vec_t *result ;

	if (n < 1) {
		PURS_ANY_RETAIN(_xs);
		return _xs;
	}

	xs = purs_any_force_array(_xs);

	if (purs_vec_is_empty(xs)) {
		PURS_RC_RELEASE(xs);
		return purs_any_array_empty;
	}

	result = (purs_vec_t *) purs_vec_new();
	for (purs_int_t i = n; i < xs->length; i++) {
		PURS_ANY_RETAIN(xs->data[i]);
		purs_vec_push_mut(result, xs->data[i]);
	}
	PURS_RC_RELEASE(xs);
	return purs_any_array(result);
}

//------------------------------------------------------------------------------
// Zipping ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_3(Data_Array_zipWith, f, _xs, _ys) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	const purs_vec_t *ys = purs_any_force_array(_ys);

	if (purs_vec_is_empty(xs) || purs_vec_is_empty(ys)) {
		PURS_RC_RELEASE(xs);
		PURS_RC_RELEASE(ys);
		return purs_any_array_empty;
	}

	purs_int_t sz = xs->length < ys->length ? xs->length : ys->length;
	purs_vec_t *result = (purs_vec_t *) purs_vec_new1(sz);
	result->length = sz;
	for (purs_int_t i = 0; i < sz; i++) {
		purs_any_t tmp1, tmp2;
		tmp1 = purs_any_app(f, xs->data[i]);
		tmp2 = purs_any_app(tmp1, ys->data[i]);
		result->data[i] = tmp2;
		PURS_ANY_RELEASE(tmp1);
	}

	PURS_RC_RELEASE(xs);
	PURS_RC_RELEASE(ys);

	return purs_any_array(result);
}

//------------------------------------------------------------------------------
// Partial ---------------------------------------------------------------------
//------------------------------------------------------------------------------

PURS_FFI_FUNC_2(Data_Array_unsafeIndexImpl, _xs, _n) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_int_t n = purs_any_force_int(_n);
	purs_any_t ret = xs->data[n];
	PURS_ANY_RETAIN(ret);
	PURS_RC_RELEASE(xs);
	return ret;
}
