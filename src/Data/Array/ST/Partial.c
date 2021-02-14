#include <purescript.h>

PURS_FFI_FUNC_3(Data_Array_ST_Partial_peekImpl, _i, _xs, _) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	purs_int_t i = purs_any_force_int(_i);
	purs_any_t ret = xs->data[i];
	PURS_ANY_RETAIN(ret);
	PURS_RC_RELEASE(xs);
	return ret;
}

PURS_FFI_FUNC_4(Data_Array_ST_Partial_pokeImpl, _i, a, _xs, _) {
	const purs_vec_t *xs = purs_any_force_array(_xs);
	const purs_int_t i = purs_any_force_int(_i);
	PURS_ANY_RELEASE(xs->data[i]);
	PURS_ANY_RETAIN(a);
	xs->data[i] = a;
	PURS_RC_RELEASE(xs);
	return purs_any_null;
}
