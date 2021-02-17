#include <purescript.h>

PURS_FFI_FUNC_2(Data_Array_NonEmpty_Internal_fold1Impl, f, _xs) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_any_t acc = xs->data[0], tmp1;
	int acc_changed = 0;
	for (int i = 1; i < xs->length; i++) {
		tmp1 = purs_any_app(f, acc);
		if (acc_changed) PURS_ANY_RELEASE(acc);
		acc = purs_any_app(tmp1, xs->data[i]);
		PURS_ANY_RELEASE(tmp1);
	}
	PURS_RC_RELEASE(xs);
	return acc;
}

/* the last elem of the cons list [X, []] */
PURS_FFI_FUNC_1(DANI_finalCell, head) {
	return purs_any_array(purs_vec_new_va(2, head, purs_any_array_empty));
}

/* build a cons list from an array [X, [X, ...]] */
PURS_FFI_FUNC_2(DANI_curryCons, head, tail) {
	return purs_any_array(purs_vec_new_va(2, head, tail));
}

PURS_FUNC_1(traverse1Impl__go_ret, scope, _, __) {
	purs_any_t
		go = purs_scope_binding_at(scope, 0),
		buildFrom = purs_scope_binding_at(scope, 1),
		last = purs_scope_binding_at(scope, 2),
		acc = purs_scope_binding_at(scope, 3),
		currentLen = purs_scope_binding_at(scope, 4),
		xs = purs_scope_binding_at(scope, 5),
		tmp1 = purs_any_app(buildFrom, last, acc),
		ret;
	purs_int_t len = purs_any_force_int(currentLen);
	ret = purs_any_app(go, tmp1, purs_any_int(len - 1), xs);
	PURS_ANY_RELEASE(tmp1);
	return ret;
}

PURS_FUNC_1(traverse1Impl__go, scope, acc, args) {
	purs_any_t currentLength = va_arg(args, purs_any_t);
	purs_any_t _xs = va_arg(args, purs_any_t);
	purs_any_t buildFrom = purs_scope_binding_at(scope, 0);
	purs_int_t len = purs_any_force_int(currentLength);

	if (len == 0) {
		PURS_ANY_RETAIN(acc);
		return acc;
	}

	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_any_t
		last = xs->data[len - 1],
		ret,
		go = purs_any_cont(purs_cont_new(scope, traverse1Impl__go));

	{ /* assemble ret continuation */
		const purs_scope_t *scope = purs_scope_new(6,
							   go,
							   buildFrom,
							   last,
							   acc,
							   currentLength,
							   _xs);
		ret = purs_any_cont(purs_cont_new(scope, traverse1Impl__go_ret));
		PURS_RC_RELEASE(scope);
	}

	PURS_RC_RELEASE(xs);
	PURS_ANY_RELEASE(go);

	return ret;
}

PURS_FUNC_1(traverse1Impl__buildFrom, scope, x, args) {
	purs_any_t
		ys = va_arg(args, purs_any_t),
		apply = purs_scope_binding_at(scope, 0),
		map = purs_scope_binding_at(scope, 1),
		f = purs_scope_binding_at(scope, 2),
		tmp1 = purs_any_app(map, DANI_curryCons),
		tmp2 = purs_any_app(f, x),
		tmp3 = purs_any_app(tmp1, tmp2),
		tmp4 = purs_any_app(apply, tmp3),
		ret = purs_any_app(tmp4, ys);
	PURS_ANY_RELEASE(tmp4);
	PURS_ANY_RELEASE(tmp3);
	PURS_ANY_RELEASE(tmp2);
	PURS_ANY_RELEASE(tmp1);
	return ret;
}

PURS_FFI_FUNC_1(DANI_listToArray, list) {
	purs_any_t head, tail, ret = purs_any_array_empty;
	const purs_vec_t *xs = purs_any_force_array(list);
	const purs_vec_t *ys;
	int sz, i;

	if (purs_vec_is_empty(xs)) goto end;

	sz = 0;
	ys = xs;
	while (1) {
		if (purs_vec_is_empty(ys)) break;
		ys = purs_any_unsafe_get_array(ys->data[1]);
		sz++;
	}

	purs_vec_t *zs = (purs_vec_t*) purs_vec_new1(sz);
	zs->length = sz;
	ret = purs_any_array(zs);

	i = 0;
	ys = xs;
	while (1) {
		if (purs_vec_is_empty(ys)) break;
		zs->data[i] = ys->data[0];
		PURS_ANY_RETAIN(zs->data[i]);
		i++;
		ys = purs_any_unsafe_get_array(ys->data[1]);
	}

end:
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FUNC_1(traverse1Impl__ret, scope, array, __) {
	purs_any_t
		map = purs_scope_binding_at(scope, 0),
		f = purs_scope_binding_at(scope, 1),
		go = purs_scope_binding_at(scope, 2);
	const purs_vec_t *xs = purs_any_force_array(array);
	purs_any_t tmp1 = purs_any_app(map, DANI_finalCell);
	purs_any_t tmp2 = purs_any_app(f, xs->data[purs_vec_length(xs) - 1]);
	purs_any_t tmp3 = purs_any_app(map, DANI_listToArray);
	purs_any_t acc = purs_any_app(tmp1, tmp2);
	purs_any_t result = purs_any_app(go, acc, purs_any_int(purs_vec_length(xs) - 1), array);
	purs_any_t tmp4;
	while (result.tag == PURS_ANY_TAG_CONT) {
		tmp4 = purs_any_app(result, purs_any_null);
		PURS_ANY_RELEASE(result);
		result = tmp4;
	}
	PURS_ANY_RELEASE(tmp2);
	PURS_ANY_RELEASE(tmp1);
	PURS_ANY_RELEASE(acc);
	purs_any_t ret = purs_any_app(tmp3, result);
	PURS_ANY_RELEASE(result);
	PURS_ANY_RELEASE(tmp3);
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_3(Data_Array_NonEmpty_Internal_traverse1Impl, apply, map, f) {
	purs_any_t buildFrom, go, ret;

	{ /* assemble buildFrom continuation */
		const purs_scope_t *scope = purs_scope_new(3, apply, map, f);
		buildFrom = purs_any_cont(purs_cont_new(scope, traverse1Impl__buildFrom));
		PURS_RC_RELEASE(scope);
	}

	{ /* assemble go continuation */
		const purs_scope_t *scope = purs_scope_new(1, buildFrom);
		go = purs_any_cont(purs_cont_new(scope, traverse1Impl__go));
		PURS_RC_RELEASE(scope);
	}

	{ /* assemble return continuation */
		const purs_scope_t *scope = purs_scope_new(3, map, f, go);
		ret = purs_any_cont(purs_cont_new(scope, traverse1Impl__ret));
		PURS_RC_RELEASE(scope);
	}

	PURS_ANY_RELEASE(buildFrom);
	PURS_ANY_RELEASE(go);

	return ret;
}
