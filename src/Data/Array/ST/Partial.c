#include <purescript.h>

PURS_FFI_FUNC_3(Data_Array_ST_Partial_peekImpl, _i, _xs, _, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	const purs_any_int_t i = purs_any_get_int(_i);
	return xs->data[i];
});

PURS_FFI_FUNC_4(Data_Array_ST_Partial_pokeImpl, _i, a, _xs, _, {
	const purs_vec_t * xs = purs_any_get_array(_xs);
	const purs_any_int_t i = purs_any_get_int(_i);
	xs->data[i] = a;
	return NULL;
});
