/* Generated by DBCC, see <https://github.com/howerj/dbcc> 
This file was generated by dbcc: See <https://github.com/howerj/dbcc>

Consider donating; see the repo for more information, this project 
requires your support to continue.

For those with paid support or to inquire about paid support
please Email <mailto:hello.operator.co.uk@gmail.com>.

*/

#include "dps_messages.h"
#include <inttypes.h>

#define UNUSED(X) ((void)(X))

static inline uint64_t reverse_byte_order(uint64_t x) {
	x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
	x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
	x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;
	return x;
}

static inline int print_helper(int r, int print_return_value) {
	return ((r >= 0) && (print_return_value >= 0)) ? r + print_return_value : -1;
}

static int pack_can_0x28a_DpsSlaveMex(can_obj_dps_messages_h_t *o, uint64_t *data) {
	register uint64_t x;
	register uint64_t i = 0;
	/* board_id: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
	x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.board_id)) & 0xf;
	i |= x;
	/* Mode: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
	x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.Mode)) & 0xf;
	x <<= 4; 
	i |= x;
	switch (o->can_0x28a_DpsSlaveMex.Mode) {
	case 0:
		/* board_name: start-bit 16, length 56, endianess intel, scaling 1, offset 0 */
		x = ((uint64_t)(o->can_0x28a_DpsSlaveMex.board_name)) & 0xffffffffffffff;
		x <<= 16; 
		i |= x;
		/* board_id: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.board_id)) & 0xf;
		i |= x;
		/* Mode: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.Mode)) & 0xf;
		x <<= 4; 
		i |= x;
		/* id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.id)) & 0xf;
		x <<= 8; 
		i |= x;
		break;
	case 1:
		/* var_name: start-bit 16, length 48, endianess intel, scaling 1, offset 0 */
		x = ((uint64_t)(o->can_0x28a_DpsSlaveMex.var_name)) & 0xffffffffffff;
		x <<= 16; 
		i |= x;
		/* info_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.info_var_id)) & 0xf;
		x <<= 8; 
		i |= x;
		break;
	case 2:
		/* value_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.value_var_id)) & 0xf;
		x <<= 8; 
		i |= x;
		/* type: start-bit 12, length 2, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.type)) & 0x3;
		x <<= 12; 
		i |= x;
		/* size: start-bit 14, length 2, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.size)) & 0x3;
		x <<= 14; 
		i |= x;
		break;
	case 3:
		/* value: start-bit 16, length 32, endianess intel, scaling 1, offset 0 */
		x = ((uint32_t)(o->can_0x28a_DpsSlaveMex.value)) & 0xffffffff;
		x <<= 16; 
		i |= x;
		/* var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28a_DpsSlaveMex.var_id)) & 0xf;
		x <<= 8; 
		i |= x;
		break;
	default:
		return -1;
	}
	*data = (i);
	o->can_0x28a_DpsSlaveMex_tx = 1;
	return 8;
}

static int unpack_can_0x28a_DpsSlaveMex(can_obj_dps_messages_h_t *o, uint64_t data, uint8_t dlc, dbcc_time_stamp_t time_stamp) {
	register uint64_t x;
	register uint64_t i = (data);
	if (dlc < 8)
		return -1;
	/* board_id: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
	x = i & 0xf;
	o->can_0x28a_DpsSlaveMex.board_id = x;
	/* Mode: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
	x = (i >> 4) & 0xf;
	o->can_0x28a_DpsSlaveMex.Mode = x;
	switch (o->can_0x28a_DpsSlaveMex.Mode) {
	case 0:
		/* board_name: start-bit 16, length 56, endianess intel, scaling 1, offset 0 */
		x = (i >> 16) & 0xffffffffffffff;
		o->can_0x28a_DpsSlaveMex.board_name = x;
		/* board_id: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
		x = i & 0xf;
		o->can_0x28a_DpsSlaveMex.board_id = x;
		/* Mode: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 4) & 0xf;
		o->can_0x28a_DpsSlaveMex.Mode = x;
		/* id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28a_DpsSlaveMex.id = x;
		break;
	case 1:
		/* var_name: start-bit 16, length 48, endianess intel, scaling 1, offset 0 */
		x = (i >> 16) & 0xffffffffffff;
		o->can_0x28a_DpsSlaveMex.var_name = x;
		/* info_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28a_DpsSlaveMex.info_var_id = x;
		break;
	case 2:
		/* value_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28a_DpsSlaveMex.value_var_id = x;
		/* type: start-bit 12, length 2, endianess intel, scaling 1, offset 0 */
		x = (i >> 12) & 0x3;
		o->can_0x28a_DpsSlaveMex.type = x;
		/* size: start-bit 14, length 2, endianess intel, scaling 1, offset 0 */
		x = (i >> 14) & 0x3;
		o->can_0x28a_DpsSlaveMex.size = x;
		break;
	case 3:
		/* value: start-bit 16, length 32, endianess intel, scaling 1, offset 0 */
		x = (i >> 16) & 0xffffffff;
		o->can_0x28a_DpsSlaveMex.value = x;
		/* var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28a_DpsSlaveMex.var_id = x;
		break;
	default:
		return -1;
	}
	o->can_0x28a_DpsSlaveMex_rx = 1;
	o->can_0x28a_DpsSlaveMex_time_stamp_rx = time_stamp;
	return 8;
}

int decode_can_0x28a_board_name(const can_obj_dps_messages_h_t *o, uint64_t *out) {
	uint64_t rval = (uint64_t)(o->can_0x28a_DpsSlaveMex.board_name);
	*out = rval;
	return 0;
}

int encode_can_0x28a_board_name(can_obj_dps_messages_h_t *o, uint64_t in) {
	o->can_0x28a_DpsSlaveMex.board_name = in;
	return 0;
}

int decode_can_0x28a_board_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.board_id);
	*out = rval;
	return 0;
}

int encode_can_0x28a_board_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.board_id = in;
	return 0;
}

int decode_can_0x28a_Mode(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.Mode);
	*out = rval;
	return 0;
}

int encode_can_0x28a_Mode(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.Mode = in;
	return 0;
}

int decode_can_0x28a_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.id);
	*out = rval;
	return 0;
}

int encode_can_0x28a_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.id = in;
	return 0;
}

int decode_can_0x28a_var_name(const can_obj_dps_messages_h_t *o, uint64_t *out) {
	uint64_t rval = (uint64_t)(o->can_0x28a_DpsSlaveMex.var_name);
	*out = rval;
	return 0;
}

int encode_can_0x28a_var_name(can_obj_dps_messages_h_t *o, uint64_t in) {
	o->can_0x28a_DpsSlaveMex.var_name = in;
	return 0;
}

int decode_can_0x28a_info_var_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.info_var_id);
	*out = rval;
	return 0;
}

int encode_can_0x28a_info_var_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.info_var_id = in;
	return 0;
}

int decode_can_0x28a_value_var_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.value_var_id);
	*out = rval;
	return 0;
}

int encode_can_0x28a_value_var_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.value_var_id = in;
	return 0;
}

int decode_can_0x28a_type(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.type);
	if (rval <= 2) {
		*out = rval;
		return 0;
	} else {
		*out = (uint8_t)0;
		return -1;
	}
}

int encode_can_0x28a_type(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.type = 0;
	if (in > 2)
		return -1;
	o->can_0x28a_DpsSlaveMex.type = in;
	return 0;
}

int decode_can_0x28a_size(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.size);
	if (rval <= 2) {
		*out = rval;
		return 0;
	} else {
		*out = (uint8_t)0;
		return -1;
	}
}

int encode_can_0x28a_size(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.size = 0;
	if (in > 2)
		return -1;
	o->can_0x28a_DpsSlaveMex.size = in;
	return 0;
}

int decode_can_0x28a_value(const can_obj_dps_messages_h_t *o, uint32_t *out) {
	uint32_t rval = (uint32_t)(o->can_0x28a_DpsSlaveMex.value);
	if (rval <= 2) {
		*out = rval;
		return 0;
	} else {
		*out = (uint32_t)0;
		return -1;
	}
}

int encode_can_0x28a_value(can_obj_dps_messages_h_t *o, uint32_t in) {
	o->can_0x28a_DpsSlaveMex.value = 0;
	if (in > 2)
		return -1;
	o->can_0x28a_DpsSlaveMex.value = in;
	return 0;
}

int decode_can_0x28a_var_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28a_DpsSlaveMex.var_id);
	*out = rval;
	return 0;
}

int encode_can_0x28a_var_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28a_DpsSlaveMex.var_id = in;
	return 0;
}

int print_can_0x28a_DpsSlaveMex(const can_obj_dps_messages_h_t *o, FILE *output) {
	int r = 0;
	r = print_helper(r, fprintf(output, "board_name = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.board_name)));
	r = print_helper(r, fprintf(output, "board_id = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.board_id)));
	r = print_helper(r, fprintf(output, "Mode = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.Mode)));
	r = print_helper(r, fprintf(output, "id = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.id)));
	r = print_helper(r, fprintf(output, "var_name = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.var_name)));
	r = print_helper(r, fprintf(output, "info_var_id = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.info_var_id)));
	r = print_helper(r, fprintf(output, "value_var_id = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.value_var_id)));
	r = print_helper(r, fprintf(output, "type = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.type)));
	r = print_helper(r, fprintf(output, "size = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.size)));
	r = print_helper(r, fprintf(output, "value = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.value)));
	r = print_helper(r, fprintf(output, "var_id = (wire: %.0f)\n", (double)(o->can_0x28a_DpsSlaveMex.var_id)));
	return r;
}

static int pack_can_0x28b_DpsMasterMex(can_obj_dps_messages_h_t *o, uint64_t *data) {
	register uint64_t x;
	register uint64_t i = 0;
	/* Mode: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
	x = ((uint8_t)(o->can_0x28b_DpsMasterMex.Mode)) & 0xf;
	i |= x;
	switch (o->can_0x28b_DpsMasterMex.Mode) {
	case 0:
		/* reserved: start-bit 4, length 1, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.reserved)) & 0x1;
		x <<= 4; 
		i |= x;
		break;
	case 1:
		/* var_name_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.var_name_board_id)) & 0xf;
		x <<= 4; 
		i |= x;
		break;
	case 2:
		/* var_refresh_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.var_refresh_board_id)) & 0xf;
		x <<= 4; 
		i |= x;
		/* var_refresh_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.var_refresh_var_id)) & 0xf;
		x <<= 8; 
		i |= x;
		break;
	case 3:
		/* value: start-bit 16, length 32, endianess intel, scaling 1, offset 0 */
		x = ((uint32_t)(o->can_0x28b_DpsMasterMex.value)) & 0xffffffff;
		x <<= 16; 
		i |= x;
		/* var_value_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.var_value_board_id)) & 0xf;
		x <<= 4; 
		i |= x;
		/* var_value_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = ((uint8_t)(o->can_0x28b_DpsMasterMex.var_value_var_id)) & 0xf;
		x <<= 8; 
		i |= x;
		break;
	default:
		return -1;
	}
	*data = (i);
	o->can_0x28b_DpsMasterMex_tx = 1;
	return 8;
}

static int unpack_can_0x28b_DpsMasterMex(can_obj_dps_messages_h_t *o, uint64_t data, uint8_t dlc, dbcc_time_stamp_t time_stamp) {
	register uint64_t x;
	register uint64_t i = (data);
	if (dlc < 8)
		return -1;
	/* Mode: start-bit 0, length 4, endianess intel, scaling 1, offset 0 */
	x = i & 0xf;
	o->can_0x28b_DpsMasterMex.Mode = x;
	switch (o->can_0x28b_DpsMasterMex.Mode) {
	case 0:
		/* reserved: start-bit 4, length 1, endianess intel, scaling 1, offset 0 */
		x = (i >> 4) & 0x1;
		o->can_0x28b_DpsMasterMex.reserved = x;
		break;
	case 1:
		/* var_name_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 4) & 0xf;
		o->can_0x28b_DpsMasterMex.var_name_board_id = x;
		break;
	case 2:
		/* var_refresh_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 4) & 0xf;
		o->can_0x28b_DpsMasterMex.var_refresh_board_id = x;
		/* var_refresh_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28b_DpsMasterMex.var_refresh_var_id = x;
		break;
	case 3:
		/* value: start-bit 16, length 32, endianess intel, scaling 1, offset 0 */
		x = (i >> 16) & 0xffffffff;
		o->can_0x28b_DpsMasterMex.value = x;
		/* var_value_board_id: start-bit 4, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 4) & 0xf;
		o->can_0x28b_DpsMasterMex.var_value_board_id = x;
		/* var_value_var_id: start-bit 8, length 4, endianess intel, scaling 1, offset 0 */
		x = (i >> 8) & 0xf;
		o->can_0x28b_DpsMasterMex.var_value_var_id = x;
		break;
	default:
		return -1;
	}
	o->can_0x28b_DpsMasterMex_rx = 1;
	o->can_0x28b_DpsMasterMex_time_stamp_rx = time_stamp;
	return 8;
}

int decode_can_0x28b_Mode(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.Mode);
	*out = rval;
	return 0;
}

int encode_can_0x28b_Mode(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.Mode = in;
	return 0;
}

int decode_can_0x28b_reserved(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.reserved);
	*out = rval;
	return 0;
}

int encode_can_0x28b_reserved(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.reserved = in;
	return 0;
}

int decode_can_0x28b_var_name_board_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.var_name_board_id);
	*out = rval;
	return 0;
}

int encode_can_0x28b_var_name_board_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.var_name_board_id = in;
	return 0;
}

int decode_can_0x28b_var_refresh_board_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.var_refresh_board_id);
	*out = rval;
	return 0;
}

int encode_can_0x28b_var_refresh_board_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.var_refresh_board_id = in;
	return 0;
}

int decode_can_0x28b_var_refresh_var_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.var_refresh_var_id);
	*out = rval;
	return 0;
}

int encode_can_0x28b_var_refresh_var_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.var_refresh_var_id = in;
	return 0;
}

int decode_can_0x28b_value(const can_obj_dps_messages_h_t *o, uint32_t *out) {
	uint32_t rval = (uint32_t)(o->can_0x28b_DpsMasterMex.value);
	*out = rval;
	return 0;
}

int encode_can_0x28b_value(can_obj_dps_messages_h_t *o, uint32_t in) {
	o->can_0x28b_DpsMasterMex.value = in;
	return 0;
}

int decode_can_0x28b_var_value_board_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.var_value_board_id);
	*out = rval;
	return 0;
}

int encode_can_0x28b_var_value_board_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.var_value_board_id = in;
	return 0;
}

int decode_can_0x28b_var_value_var_id(const can_obj_dps_messages_h_t *o, uint8_t *out) {
	uint8_t rval = (uint8_t)(o->can_0x28b_DpsMasterMex.var_value_var_id);
	*out = rval;
	return 0;
}

int encode_can_0x28b_var_value_var_id(can_obj_dps_messages_h_t *o, uint8_t in) {
	o->can_0x28b_DpsMasterMex.var_value_var_id = in;
	return 0;
}

int print_can_0x28b_DpsMasterMex(const can_obj_dps_messages_h_t *o, FILE *output) {
	int r = 0;
	r = print_helper(r, fprintf(output, "Mode = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.Mode)));
	r = print_helper(r, fprintf(output, "reserved = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.reserved)));
	r = print_helper(r, fprintf(output, "var_name_board_id = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.var_name_board_id)));
	r = print_helper(r, fprintf(output, "var_refresh_board_id = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.var_refresh_board_id)));
	r = print_helper(r, fprintf(output, "var_refresh_var_id = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.var_refresh_var_id)));
	r = print_helper(r, fprintf(output, "value = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.value)));
	r = print_helper(r, fprintf(output, "var_value_board_id = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.var_value_board_id)));
	r = print_helper(r, fprintf(output, "var_value_var_id = (wire: %.0f)\n", (double)(o->can_0x28b_DpsMasterMex.var_value_var_id)));
	return r;
}

int unpack_message(can_obj_dps_messages_h_t *o, const unsigned long id, uint64_t data, uint8_t dlc, dbcc_time_stamp_t time_stamp) {
	switch (id) {
	case 0x28a: return unpack_can_0x28a_DpsSlaveMex(o, data, dlc, time_stamp);
	case 0x28b: return unpack_can_0x28b_DpsMasterMex(o, data, dlc, time_stamp);
	default: break; 
	}
	return -1; 
}

int pack_message(can_obj_dps_messages_h_t *o, const unsigned long id, uint64_t *data) {
	switch (id) {
	case 0x28a: return pack_can_0x28a_DpsSlaveMex(o, data);
	case 0x28b: return pack_can_0x28b_DpsMasterMex(o, data);
	default: break; 
	}
	return -1; 
}

int message_dlc(const unsigned long id) {
	switch (id) {
	case 0x28a: return 8;
	case 0x28b: return 8;
	default: break; 
	}
	return -1; 
}

int print_message(const can_obj_dps_messages_h_t *o, const unsigned long id, FILE *output) {
	switch (id) {
	case 0x28a: return print_can_0x28a_DpsSlaveMex(o, output);
	case 0x28b: return print_can_0x28b_DpsMasterMex(o, output);
	default: break; 
	}
	return -1; 
}

