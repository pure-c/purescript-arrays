#include <purescript.h>

PURS_FFI_FUNC_2(Data_Array_NonEmpty_Internal_fold1Impl, f, _xs) {
	/* const purs_vec_t *xs = purs_any_get_array(_xs); */
	/* const purs_any_t *acc = xs->data[0]; */
	/* for (purs_int_t i = 1; i < xs->length; i++) { */
	/* 	acc = purs_any_app(purs_any_app(f, acc), xs->data[i]); */
	/* } */
	/* return acc; */
	return purs_any_null;
}

PURS_FFI_FUNC_1(Cont, fn) {
	/* return purs_any_record( */
	/* 			purs_record_new_from_kvps(2, */
	/* 				"type", purs_any_string("Cont"), */
	/* 				"fn", fn)); */
	return purs_any_null;
}

PURS_FFI_FUNC_1(finalCell, head) {
	/* return purs_any_record( */
	/* 			purs_record_new_from_kvps(3, */
	/* 				"type", purs_any_string("ConsCell"), */
	/* 				"head", head, */
	/* 				"tail", NULL)); */
	return purs_any_null;
}

PURS_FFI_FUNC_2(consList, x, xs) {
	/* return purs_any_record( */
	/* 			purs_record_new_from_kvps(3, */
	/* 				"type", purs_any_string("ConsCell"), */
	/* 				"head", x, */
	/* 				"tail", xs)); */
	return purs_any_null;
}

PURS_FFI_FUNC_1(listToArray, list) {
	/* purs_vec_t * result = (purs_vec_t *) purs_vec_new(); */
	/* purs_any_t * xs = (purs_any_t *) list; */
	/* while (xs != NULL) { */
	/* 	const purs_record_t * record = purs_any_get_record(xs); */
	/* 	const purs_any_t * head = purs_record_find_by_key(record, "head")->value; */
	/* 	const purs_any_t * tail = purs_record_find_by_key(record, "tail")->value; */
	/* 	purs_vec_push_mut(result, head); */
	/* 	xs = (purs_any_t *) tail; */
	/* } */
	/* return purs_any_array(result); */
	return purs_any_null;
}

/* const purs_any_t * buildFrom( */
/* 		const purs_any_t * apply, */
/* 		const purs_any_t * map, */
/* 		const purs_any_t * f, */
/* 		const purs_any_t * x, */
/* 		const purs_any_t * ys) { */
/* 		return purs_any_app( */
/* 					purs_any_app( */
/* 						apply, */
/* 						purs_any_app( */
/* 							purs_any_app(map, consList), */
/* 							purs_any_app(f, x) */
/* 						) */
/* 					), */
/* 					ys */
/* 				 ); */
/* } */

/* const purs_any_t * go( */
/* 		const purs_any_t * apply, */
/* 		const purs_any_t * map, */
/* 		const purs_any_t * f, */
/* 		const purs_any_t * acc, */
/* 		const purs_int_t currentLen, */
/* 		const purs_vec_t * xs) { */
/* 	if (currentLen == 0) { */
/* 		return acc; */
/* 	} else { */
/* 		const purs_any_t * last = xs->data[currentLen - 1]; */
/* 		const purs_any_t * fn = go(apply, map, f, buildFrom(apply, map, f, last, acc), currentLen - 1, xs); */
/* 		return purs_any_app(Cont, fn); */
/* 	} */
/* } */

PURS_FFI_FUNC_4(Data_Array_NonEmpty_Internal_traverse1Impl, apply, map, f, _array) {
	return purs_any_null;
	/* const purs_vec_t * array = purs_any_get_array(_array); */
	/* const purs_any_t * acc = purs_any_app( */
	/* 				purs_any_app(map, finalCell), */
	/* 				purs_any_app(f, array->data[array->length - 1])); */
	/* const purs_any_t * result = go(apply, map, f, acc, array->length - 1, array); */
	/* while (1) { */
	/* 	if (result -> tag != PURS_ANY_TAG_RECORD) { */
	/* 		break; */
	/* 	} */
	/* 	const purs_record_t * record = purs_any_get_record(result); */
	/* 	const purs_any_t * tp = purs_record_find_by_key(record, "type")->value; */
	/* 	if (purs_any_eq_string(tp, "Cont")) { */
	/* 		result = purs_record_find_by_key(record, "fn")->value; */
	/* 	} else { */
	/* 		break; */
	/* 	} */
	/* } */

	/* return purs_any_app(purs_any_app(map, listToArray), result); */
}
