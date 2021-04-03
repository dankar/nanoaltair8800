#include "intel8080.h"
#include "op_codes.h"
#include <string.h>
#include "memory.h"
#include "sys/log.h"

LOG_UNIT("intel8080");
LOG_TYPE(PORT_DEBUG, OUTPUT_OFF);
LOG_TYPE(CPU_DEBUG, OUTPUT_OFF);

#define RESET_FLAGS 0x2

static inline bool get_parity(uint8_t val)
{
	val ^= val >> 4;
	val &= 0xf;
	return !((0x6996 >> val) & 1);
}

void i8080_reset(intel8080_t *cpu)
{
	LOG(CPU_DEBUG, "Machine reset\n");
	memset(cpu, 0, sizeof(intel8080_t));
	cpu->registers.flags_byte = RESET_FLAGS;
}

void i8080_assign_port(intel8080_t *cpu, uint8_t port, port_io_t *io)
{
	memcpy(&cpu->ports[port], io, sizeof(port_io_t));
}

static inline bool i8080_check_carry(uint16_t a, uint16_t b, int carry)
{
	if((a + b + carry) > 0xff)
		return true;
	else
		return false;
}

static inline bool i8080_check_half_carry(uint16_t a, uint16_t b, int carry)
{
	a &= 0xf;
	b &= 0xf;
    
	if((a + b + carry) > 0xf)
		return true;
	else
		return false;
}

static inline void i8080_mwrite(intel8080_t *cpu)
{
	write8(cpu->address_bus, cpu->data_bus);
}

static inline void i8080_mread(intel8080_t *cpu)
{
	cpu->data_bus = read8(cpu->address_bus);
}

static inline void i8080_pairwrite(intel8080_t *cpu, uint8_t pair, uint16_t val)
{
	switch(pair)
	{
        case PAIR_BC:
		cpu->registers.bc = val;
		break;
        case PAIR_DE:
		cpu->registers.de = val;
		break;
        case PAIR_HL:
		cpu->registers.hl = val;
		break;
        case PAIR_SP:
		cpu->registers.sp = val;
		break;
	}
}

static inline uint16_t i8080_pairread(intel8080_t *cpu, uint8_t pair)
{
	switch(pair)
	{
        case PAIR_BC:
		return cpu->registers.bc;
		break;
        case PAIR_DE:
		return cpu->registers.de;
		break;
        case PAIR_HL:
		return cpu->registers.hl;
		break;
        case PAIR_SP:
		return cpu->registers.sp;
		break;
        default:
		return 0;
	}
}

void i8080_regwrite(intel8080_t *cpu, uint8_t reg, uint8_t val)
{
	switch(reg)
	{
        case REGISTER_A:
		cpu->registers.a = val;
		break;
        case REGISTER_B:
		cpu->registers.b = val;
		break;
        case REGISTER_C:
		cpu->registers.c = val;
		break;
        case REGISTER_D:
		cpu->registers.d = val;
		break;
        case REGISTER_E:
		cpu->registers.e = val;
		break;
        case REGISTER_H:
		cpu->registers.h = val;
		break;
        case REGISTER_L:
		cpu->registers.l = val;
		break;
        case MEMORY_ACCESS:
		cpu->address_bus = cpu->registers.hl;
		cpu->data_bus = val;
		i8080_mwrite(cpu);
	}
}

uint8_t i8080_regread(intel8080_t *cpu, uint8_t reg)
{
	switch(reg)
	{
        case REGISTER_A:
		return cpu->registers.a;
		break;
        case REGISTER_B:
		return cpu->registers.b;
		break;
        case REGISTER_C:
		return cpu->registers.c;
		break;
        case REGISTER_D:
		return cpu->registers.d;
		break;
        case REGISTER_E:
		return cpu->registers.e;
		break;
        case REGISTER_H:
		return cpu->registers.h;
		break;
        case REGISTER_L:
		return cpu->registers.l;
		break;
        case MEMORY_ACCESS:
		cpu->address_bus = cpu->registers.hl;
		i8080_mread(cpu);
		return cpu->data_bus;
        default:
		return 0;
	}
}

bool i8080_check_condition(intel8080_t *cpu, uint8_t condition)
{
	switch(condition)
	{
        case CONDITION_NZ:
		return !(cpu->registers.flags.zero);
        case CONDITION_Z:
		return (cpu->registers.flags.zero);
        case CONDITION_NC:
		return !(cpu->registers.flags.carry);
        case CONDITION_C:
		return (cpu->registers.flags.carry);
        case CONDITION_PO:
		return !(cpu->registers.flags.parity);
        case CONDITION_PE:
		return (cpu->registers.flags.parity);
        case CONDITION_P:
		return !(cpu->registers.flags.sign);
        case CONDITION_M:
		return (cpu->registers.flags.sign);
	}
	return 0;
}

void i8080_examine(intel8080_t *cpu, uint16_t address)
{
	// Jump to the supplied address
	cpu->registers.pc = cpu->address_bus = address;
	cpu->registers.status.memr = 1;
	cpu->data_bus = read8(cpu->address_bus);
}

void i8080_examine_next(intel8080_t *cpu)
{
	cpu->address_bus++;
	cpu->registers.status.memr = 1;
	cpu->data_bus = read8(cpu->address_bus);
}

void i8080_deposit(intel8080_t *cpu, uint8_t data)
{
	cpu->data_bus = data;
	cpu->registers.status.wo = 1;
	i8080_mwrite(cpu);
}

void i8080_deposit_next(intel8080_t *cpu, uint8_t data)
{
	i8080_examine_next(cpu);
	cpu->data_bus = data;
	cpu->registers.status.wo = 1;
	i8080_mwrite(cpu);
}

#define UPDATE_FLAGS_ZPS(cpu, val) \
	do { \
		cpu->registers.flags.parity = get_parity(val); \
		cpu->registers.flags.zero = \
			((val) == 0) ? true : false; \
		cpu->registers.flags.sign = \
			((val) & 0x80) ? true : false; \
	} while(0)

void i8080_gensub(intel8080_t *cpu, uint16_t val, bool carry)
{
	uint16_t a, b;
	uint8_t carry_v = carry ? 0 : 1;
	// Subtract by adding with two-complement of val.
	// Carry-flag meaning becomes inverted since we add.
	a = cpu->registers.a;
	b = (uint8_t)~val;
	cpu->registers.flags.half_carry =
		i8080_check_half_carry(a, b, carry_v);
	cpu->registers.flags.carry = !i8080_check_carry(a, b, carry_v);
	a += b + carry_v;
	cpu->registers.a = a & 0xff;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
}

void i8080_compare(intel8080_t *cpu, uint8_t val)
{
	uint8_t tmp_a = cpu->registers.a;
	i8080_gensub(cpu, val, false);
	cpu->registers.a = tmp_a;
}

void i8080_mov(intel8080_t *cpu)
{
	uint8_t dest = DESTINATION(cpu->current_op_code);
	uint8_t source = SOURCE(cpu->current_op_code);
	uint8_t val;
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	val = i8080_regread(cpu, source);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.wo = 1;
	}
	i8080_regwrite(cpu, dest, val);
	cpu->registers.pc++;
}

void i8080_mvi(intel8080_t *cpu)
{
	uint8_t dest = DESTINATION(cpu->current_op_code);
	cpu->registers.status.memr = 1;
	uint8_t reg = read8(cpu->registers.pc+1);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.wo = 1;
	}
	i8080_regwrite(cpu, dest, reg);
	cpu->registers.pc+=2;
}

void i8080_lxi(intel8080_t *cpu)
{
	uint8_t pair = RP(cpu->current_op_code);
	cpu->registers.status.memr = 1;
	uint16_t reg = read16(cpu->registers.pc+1);
	i8080_pairwrite(cpu, pair, reg);
	cpu->registers.pc+=3;
}

void i8080_lda(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->address_bus = read16(cpu->registers.pc+1);
	i8080_mread(cpu);
	cpu->registers.a = cpu->data_bus;
	cpu->registers.pc+=3;
}

void i8080_sta(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->address_bus = read16(cpu->registers.pc+1);
	cpu->data_bus = cpu->registers.a;
	cpu->registers.status.wo = 1;
	i8080_mwrite(cpu);
	cpu->registers.pc+=3;
}

void i8080_lhld(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->registers.hl = read16(read16(cpu->registers.pc+1));
	cpu->registers.pc+=3;
}

void i8080_shld(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	uint16_t reg = read16(cpu->registers.pc+1);
	cpu->registers.status.wo = 1;
	write16(reg, cpu->registers.hl);
	cpu->registers.pc+=3;
}

// TODO: only BC and DE allowed for indirect
void i8080_ldax(intel8080_t *cpu)
{
	uint8_t pair = RP(cpu->current_op_code);
	cpu->registers.status.memr = 1;
	cpu->registers.a = read8(i8080_pairread(cpu, pair));
	cpu->registers.pc++;
}

// TODO: only BC and DE allowed for indirect
void i8080_stax(intel8080_t *cpu)
{
	uint8_t pair = RP(cpu->current_op_code);
	cpu->registers.status.wo = 1;
	write8(i8080_pairread(cpu, pair), cpu->registers.a);
	cpu->registers.pc++;
}

void i8080_xchg(intel8080_t *cpu)
{
	uint16_t tmp = cpu->registers.hl;
	cpu->registers.hl = cpu->registers.de;
	cpu->registers.de = tmp;
	cpu->registers.pc++;
}

void i8080_genadd(intel8080_t *cpu, uint16_t val, bool carry)
{
	uint8_t a;
	uint8_t carry_val = carry ? 1 : 0;
	a = i8080_regread(cpu, REGISTER_A);
	cpu->registers.flags.half_carry =
		i8080_check_half_carry(a, val, carry_val);
	cpu->registers.flags.carry = i8080_check_carry(a, val, carry_val);
	cpu->registers.a += val + carry_val;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
}

void i8080_add(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	uint8_t val = i8080_regread(cpu, source);
	i8080_genadd(cpu, val, false);
	cpu->registers.pc++;
}

void i8080_adi(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	uint8_t reg = read8(cpu->registers.pc+1);
	i8080_genadd(cpu, reg, false);
	cpu->registers.pc+=2;
}

void i8080_adc(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint16_t val = i8080_regread(cpu, source);
	i8080_genadd(cpu, val, cpu->registers.flags.carry);
	cpu->registers.pc++;
}

void i8080_aci(intel8080_t *cpu)
{
	uint16_t val;
	cpu->registers.status.memr = 1;
	val = read8(cpu->registers.pc+1);
	i8080_genadd(cpu, val, cpu->registers.flags.carry);
	cpu->registers.pc+=2;
}

void i8080_sub(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint8_t val = i8080_regread(cpu, source);
	i8080_gensub(cpu, val, false);
	cpu->registers.pc++;
}

void i8080_sui(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	uint8_t reg = read8(cpu->registers.pc+1);
	i8080_gensub(cpu, reg, false);
	cpu->registers.pc+=2;
}

void i8080_sbb(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint16_t val = i8080_regread(cpu, source);
	i8080_gensub(cpu, val, cpu->registers.flags.carry);
	cpu->registers.pc++;
}

void i8080_sbi(intel8080_t *cpu)
{
	uint16_t val;
	cpu->registers.status.memr = 1;
	val = read8(cpu->registers.pc+1);
	i8080_gensub(cpu, val, cpu->registers.flags.carry);
	cpu->registers.pc+=2;
}

void i8080_inr(intel8080_t *cpu)
{
	uint8_t dest = DESTINATION(cpu->current_op_code);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint8_t val = i8080_regread(cpu, dest);
	cpu->registers.flags.half_carry = i8080_check_half_carry(val, 1, 0);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.wo = 1;
	}
	i8080_regwrite(cpu, dest, val + 1);
	UPDATE_FLAGS_ZPS(cpu, (uint8_t)(val + 1));
	cpu->registers.pc++;
}

void i8080_dcr(intel8080_t *cpu)
{
	uint8_t dest = DESTINATION(cpu->current_op_code);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint8_t val = i8080_regread(cpu, dest);
	cpu->registers.flags.half_carry =
		i8080_check_half_carry(val, 0xff, 0);
	if(dest == MEMORY_ACCESS)
	{
		cpu->registers.status.wo = 1;
	}
	i8080_regwrite(cpu, dest, val + 0xff);
	UPDATE_FLAGS_ZPS(cpu, (uint8_t)(val + 0xff));
	cpu->registers.pc++;
}

void i8080_inx(intel8080_t *cpu)
{
	uint8_t rp = RP(cpu->current_op_code);
	i8080_pairwrite(cpu, rp, i8080_pairread(cpu, rp) + 1);
	cpu->registers.pc++;
}

void i8080_dcx(intel8080_t *cpu)
{
	uint8_t rp = RP(cpu->current_op_code);
	cpu->registers.pc++;
	i8080_pairwrite(cpu, rp, i8080_pairread(cpu, rp) - 1);
}

void i8080_dad(intel8080_t *cpu)
{
	uint8_t rp = RP(cpu->current_op_code);
	uint32_t val = i8080_pairread(cpu, rp);
	val += i8080_pairread(cpu, PAIR_HL);
	cpu->registers.flags.carry = val > 0xffff;
	i8080_pairwrite(cpu, PAIR_HL, val & 0xffff);
	cpu->registers.pc++;
}

void i8080_ana(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	uint8_t val = i8080_regread(cpu, source);
	cpu->registers.flags.half_carry =
		(val | cpu->registers.a) & 0x08 ? true : false;
	cpu->registers.a &= val;
	cpu->registers.flags.carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc++;
}

void i8080_ani(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	uint8_t val = read8(cpu->registers.pc + 1);
	cpu->registers.flags.half_carry = (val | cpu->registers.a) & 0x08;
	cpu->registers.a &= val;
	cpu->registers.flags.carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc+=2;
}

void i8080_ora(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	cpu->registers.a |= i8080_regread(cpu, source);
	cpu->registers.flags.carry = 0;
	cpu->registers.flags.half_carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc++;
}

void i8080_ori(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->registers.a |= read8(cpu->registers.pc+1);
	cpu->registers.flags.carry = 0;
	cpu->registers.flags.half_carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc+=2;
}

void i8080_xra(intel8080_t *cpu)
{
	uint8_t source = SOURCE(cpu->current_op_code);
	if(source == MEMORY_ACCESS)
	{
		cpu->registers.status.memr = 1;
	}
	cpu->registers.a ^= i8080_regread(cpu, source);
	if(source == MEMORY_ACCESS)
	{
        
	}
	cpu->registers.flags.carry = 0;
	cpu->registers.flags.half_carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc++;
}

void i8080_xri(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->registers.a ^= read8(cpu->registers.pc+1);
	cpu->registers.flags.carry = 0;
	cpu->registers.flags.half_carry = 0;
	UPDATE_FLAGS_ZPS(cpu, cpu->registers.a);
	cpu->registers.pc+=2;
}

void i8080_ei(intel8080_t *cpu)
{
	cpu->registers.pc++;
	cpu->registers.status.interrupt = 1;
}

void i8080_di(intel8080_t *cpu)
{
	cpu->registers.pc++;
	cpu->registers.status.interrupt = 0;
}

void i8080_xthl(intel8080_t *cpu)
{
	uint16_t temp = read16(cpu->registers.sp);
	write16(cpu->registers.sp, cpu->registers.hl);
	cpu->registers.hl = temp;
	cpu->registers.pc++;
}

void i8080_sphl(intel8080_t *cpu)
{
	cpu->registers.sp = cpu->registers.hl;
	cpu->registers.pc++;
}

void i8080_in(intel8080_t *cpu)
{
	cpu->registers.status.inp = 1;
	uint8_t port = read8(cpu->registers.pc+1);
	if(port == 0xff)
	{
		memory_disable_rom();
	}
	if(port == 0x00)
	{
		cpu->registers.a = 0x00;
	}
	else if(cpu->ports[port].in)
	{
		uint8_t b;
		if(cpu->ports[port].in(&b))
		{
			cpu->registers.a = b;
		}
	}
	else
	{
		cpu->registers.a = 0xff;
	}
	cpu->registers.pc+=2;
}

void i8080_out(intel8080_t *cpu)
{
	cpu->registers.status.out = 1;
	uint8_t port = read8(cpu->registers.pc+1);
	if(port == 0xff)
	{
		memory_disable_rom();
	}
	if(cpu->ports[port].out)
	{
		cpu->ports[port].out(cpu->registers.a);
	}
	cpu->registers.pc+=2;
}

void i8080_push(intel8080_t *cpu)
{
	uint8_t pair = RP(cpu->current_op_code);
	cpu->registers.status.stack = 1;
	uint16_t val;
	if(pair == PAIR_SP)
		val = cpu->registers.af;
	else
		val = i8080_pairread(cpu, pair);
	cpu->registers.sp-=2;
	write16(cpu->registers.sp, val);
	cpu->registers.pc++;	
}

void i8080_pop(intel8080_t *cpu)
{
	uint8_t pair = RP(cpu->current_op_code);
	cpu->registers.status.stack = 1;
	uint16_t val = read16(cpu->registers.sp);
	cpu->registers.sp+=2;
	if(pair == PAIR_SP)
		cpu->registers.af = val;
	else
		i8080_pairwrite(cpu, pair, val);
	cpu->registers.pc++;
}

void i8080_stc(intel8080_t *cpu)
{
	cpu->registers.flags.carry = 1;
	cpu->registers.pc++;
}

void i8080_cmc(intel8080_t *cpu)
{
	cpu->registers.flags.carry ^= 1;
	cpu->registers.pc++;
}

void i8080_rlc(intel8080_t *cpu)
{
	uint8_t high_bit = cpu->registers.a & 0x80;
	cpu->registers.a <<= 1;
	if(high_bit)
	{
		cpu->registers.flags.carry = 1;
		cpu->registers.a |= 1;
	}
	else
	{
		cpu->registers.flags.carry = 0;
		cpu->registers.a &= ~1;
	}
	cpu->registers.pc++;
}

void i8080_rrc(intel8080_t *cpu)
{
	uint8_t low_bit = cpu->registers.a & 1;
	cpu->registers.a >>= 1;
	if(low_bit)
	{
		cpu->registers.flags.carry = 1;
		cpu->registers.a |= 0x80;
	}
	else
	{
		cpu->registers.a &= ~0x80;
		cpu->registers.flags.carry = 0;
	}
	cpu->registers.pc++;
}

void i8080_ral(intel8080_t *cpu)
{
	uint8_t high_bit = cpu->registers.a & 0x80;
	cpu->registers.a <<= 1;
	if(cpu->registers.flags.carry)
		cpu->registers.a |= 1;
	else
		cpu->registers.a &= ~1;
	cpu->registers.flags.carry = high_bit;
	cpu->registers.pc++;
}

void i8080_rar(intel8080_t *cpu)
{
	
	uint8_t low_bit = cpu->registers.a & 1;
	cpu->registers.a >>= 1;
	if(cpu->registers.flags.carry)
		cpu->registers.a |= 0x80;
	else
		cpu->registers.a &= ~0x80;
	cpu->registers.flags.carry = low_bit;
	cpu->registers.pc++;
}

void i8080_jmp(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->registers.pc = read16(cpu->registers.pc+1);
}

void i8080_jccc(intel8080_t *cpu)
{
	uint8_t condition = CONDITION(cpu->current_op_code);
	if(i8080_check_condition(cpu, condition))
	{
		i8080_jmp(cpu);
	}
	else
	{
		cpu->registers.pc+=3;
	}
}

void i8080_ret(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	cpu->registers.pc = read16(cpu->registers.sp);
	cpu->registers.sp+=2;
}

void i8080_rccc(intel8080_t *cpu)
{
	uint8_t condition = CONDITION(cpu->current_op_code);
	if(i8080_check_condition(cpu, condition))
	{
		i8080_ret(cpu);
	}
	else
	{
		cpu->registers.pc++;
	}
}

void i8080_rst(intel8080_t *cpu)
{
	uint8_t vec = DESTINATION(cpu->current_op_code);
	cpu->registers.sp-=2;
	cpu->registers.status.wo = 1;
	write16(cpu->registers.sp, cpu->registers.pc + 1);
	cpu->registers.pc = vec*8;
}

void i8080_call(intel8080_t *cpu)
{
	
	cpu->registers.sp-=2;
	cpu->registers.status.wo = 1;
	write16(cpu->registers.sp, cpu->registers.pc + 3);
	cpu->registers.status.memr = 1;
	cpu->registers.pc = read16(cpu->registers.pc + 1);
}

void i8080_cccc(intel8080_t *cpu)
{
	uint8_t condition = CONDITION(cpu->current_op_code);
	if(i8080_check_condition(cpu, condition))
	{
		i8080_call(cpu);
	}
	else
	{
		cpu->registers.pc+=3;
	}
}

void i8080_pchl(intel8080_t *cpu)
{
	cpu->registers.pc = cpu->registers.hl;
}

void i8080_nop(intel8080_t *cpu)
{
	cpu->registers.pc++;
}

void i8080_cma(intel8080_t *cpu)
{
	cpu->registers.a = ~cpu->registers.a;
	cpu->registers.pc++;
}

void i8080_cmp(intel8080_t *cpu)
{
	uint8_t reg = SOURCE(cpu->current_op_code);
	if(reg == MEMORY_ACCESS)
		cpu->registers.status.memr = 1;
	uint8_t rr = i8080_regread(cpu, reg);
	i8080_compare(cpu, rr);
	cpu->registers.pc++;
}

void i8080_cpi(intel8080_t *cpu)
{
	cpu->registers.status.memr = 1;
	uint8_t reg = read8(cpu->registers.pc+1);
	i8080_compare(cpu, reg);
	cpu->registers.pc+=2;
}

void i8080_fetch_next_op(intel8080_t *cpu)
{
	cpu->address_bus = cpu->registers.pc;
	cpu->data_bus = memory[cpu->address_bus];
}

void i8080_daa(intel8080_t *cpu)
{
	uint8_t val, add = 0, set_carry = cpu->registers.flags.carry;
	val = i8080_regread(cpu, REGISTER_A);
    
	if((val & 0xf) > 9 || cpu->registers.flags.half_carry)
		add += 0x06;
    
	if ((((val & 0xf0) >> 4) >= 9 &&
	     ((val & 0xf) > 9)) ||
	    ((val & 0xf0) >> 4) > 9 ||
	    cpu->registers.flags.carry)
	{
		add += 0x60;
		set_carry = 1;
	}
    
	i8080_genadd(cpu, add, false);
	cpu->registers.pc++;
	cpu->registers.flags.carry = set_carry;
}

void i8080_interrupt(intel8080_t *cpu, uint8_t rst_vector)
{
	if(cpu->registers.status.interrupt)
	{
		cpu->interrupt_request = true;
		cpu->interrupt_instruction = 0307 | (rst_vector<<3);
	}
}

void i8080_hlt(intel8080_t *cpu)
{	
	cpu->registers.status.hlta = 1;
}

#ifdef CPU_DEBUG_ENABLED

static const char* instruction_lut[] = {
	"nop", "lxi b,i16", "stax b", "inx b", "inr b", "dcr b",
	"mvi b,i8", "rlc", "ill", "dad b", "ldax b", "dcx b", "inr c",
	"dcr c", "mvi c,i8", "rrc", "ill", "lxi d,i16", "stax d",
	"inx d", "inr d", "dcr d", "mvi d,i8", "ral", "ill", "dad d",
	"ldax d", "dcx d", "inr e", "dcr e", "mvi e,i8", "rar", "ill",
	"lxi h,i16", "shld", "inx h", "inr h", "dcr h", "mvi h,i8",
	"daa", "ill", "dad h", "lhld", "dcx h", "inr l", "dcr l",
	"mvi l,i8", "cma", "ill", "lxi sp,i16", "sta m16", "inx sp",
	"inr M", "dcr M", "mvi M,i8", "stc", "ill", "dad sp",
	"lda m16", "dcx sp", "inr a", "dcr a", "mvi a,i8", "cmc",
	"mov b,b", "mov b,c", "mov b,d", "mov b,e", "mov b,h",
	"mov b,l", "mov b,M", "mov b,a", "mov c,b", "mov c,c",
	"mov c,d", "mov c,e", "mov c,h", "mov c,l", "mov c,M",
	"mov c,a", "mov d,b", "mov d,c", "mov d,d", "mov d,e",
	"mov d,h", "mov d,l", "mov d,M", "mov d,a", "mov e,b",
	"mov e,c", "mov e,d", "mov e,e", "mov e,h", "mov e,l",
	"mov e,M", "mov e,a", "mov h,b", "mov h,c", "mov h,d",
	"mov h,e", "mov h,h", "mov h,l", "mov h,M", "mov h,a",
	"mov l,b", "mov l,c", "mov l,d", "mov l,e", "mov l,h",
	"mov l,l", "mov l,M", "mov l,a", "mov M,b", "mov M,c",
	"mov M,d", "mov M,e", "mov M,h", "mov M,l", "hlt", "mov M,a",
	"mov a,b", "mov a,c", "mov a,d", "mov a,e", "mov a,h",
	"mov a,l", "mov a,M", "mov a,a", "add b", "add c", "add d",
	"add e", "add h", "add l", "add M", "add a", "adc b",
	"adc c", "adc d", "adc e", "adc h", "adc l", "adc M",
	"adc a", "sub b", "sub c", "sub d", "sub e", "sub h",
	"sub l", "sub M", "sub a", "sbb b", "sbb c", "sbb d",
	"sbb e", "sbb h", "sbb l", "sbb M", "sbb a", "ana b",
	"ana c", "ana d", "ana e", "ana h", "ana l", "ana M",
	"ana a", "xra b", "xra c", "xra d", "xra e", "xra h",
	"xra l", "xra M", "xra a", "ora b", "ora c", "ora d",
	"ora e", "ora h", "ora l", "ora M", "ora a", "cmp b",
	"cmp c", "cmp d", "cmp e", "cmp h", "cmp l", "cmp M",
	"cmp a", "rnz", "pop b", "jnz i16", "jmp i16", "cnz i16",
	"push b", "adi i8", "rst 0", "rz", "ret", "jz i16", "ill",
	"cz i16", "call i16", "aci i8", "rst 1", "rnc", "pop d",
	"jnc i16", "out i8", "cnc i16", "push d", "sui i8", "rst 2",
	"rc", "ill", "jc i16", "in i8", "cc i16", "ill", "sbi i8",
	"rst 3", "rpo", "pop h", "jpo i16", "xthl", "cpo i16",
	"push h", "ani i8", "rst 4", "rpe", "pchl", "jpe i16",
	"xchg", "cpe i16", "ill", "xri i8", "rst 5", "rp",
	"pop psw", "jp i16", "di", "cp i16", "push psw", "ori i8",
	"rst 6", "rm", "sphl", "jm i16", "ei", "cm i16", "ill",
	"cpi i8", "rst 7" };

void i8080_disasm(intel8080_t *cpu)
{	
	uint8_t num_bytes = 1;
	char buffer[80];
	const char *instruction = instruction_lut[cpu->data_bus];
	const char *offset;
    
	strcpy(buffer, instruction);
	if((offset = strstr(buffer, "i8")))
	{
		size_t index = offset-buffer;
		num_bytes++;
		sprintf(&buffer[index], "0x%02x",
			read8(cpu->registers.pc+1));
		sprintf(&buffer[strlen(buffer)], "%s",
			&instruction[index+2]);
	}
	if((offset = strstr(buffer, "i16")))
	{
		size_t index = offset-buffer;
		num_bytes+=2;
		sprintf(&buffer[index], "0x%04x",
			read16(cpu->registers.pc+1));
		sprintf(&buffer[strlen(buffer)], "%s",
			&instruction[index+3]);
	}
	if((offset = strstr(buffer, "m16")))
	{
		size_t index = offset-buffer;
		num_bytes+=2;
		sprintf(&buffer[index], "[0x%04x]",
			read16(cpu->registers.pc+1));
		sprintf(&buffer[strlen(buffer)], "%s",
			&instruction[index+3]);
	}
	if((offset = strstr(buffer, "M")))
	{
		size_t index = offset-buffer;
		sprintf(&buffer[index], "[0x%04x]",
			cpu->registers.hl);
		sprintf(&buffer[strlen(buffer)], "%s",
			&instruction[index+1]);
	}
	printf("%04x: ", cpu->address_bus);
    
	for(int i = 0; i < num_bytes; i++)
	{
		printf("%02x ", read8(cpu->registers.pc+i));
	}
    
	printf("\t\t%s\n", buffer);
}

void i8080_set_breakpoint(intel8080_t *cpu, uint16_t address)
{
	cpu->breakpoint = address;
	cpu->breakpoint_enabled = true;
}

void i8080_disable_breakpoint(intel8080_t *cpu)
{
	cpu->breakpoint_enabled = false;
}

void i8080_print_state(intel8080_t *cpu)
{
	printf("A=%02x B=%02x C=%02x D=%02x E=%02x H=%02x L=%02x\n"
           "SP=%04x PC=%04x Flags=%02x\n",
	       cpu->registers.a, cpu->registers.b, cpu->registers.c,
	       cpu->registers.d, cpu->registers.e, cpu->registers.h,
	       cpu->registers.l, cpu->registers.sp, cpu->registers.pc,
	       cpu->registers.flags_byte);
}

#endif

void i8080_ill(intel8080_t *cpu)
{
	cpu->registers.pc++;
}

typedef void (*instruction_t)(intel8080_t*);

// Empirical testing has shown that a LUT is faster
// than a huge switch. Well there you go...
static const instruction_t i8080_opcodes[] = {
	[0x00] = i8080_nop,
	[0x01] = i8080_lxi,
	[0x02] = i8080_stax,
	[0x03] = i8080_inx,
	[0x04] = i8080_inr,
	[0x05] = i8080_dcr,
	[0x06] = i8080_mvi,
	[0x07] = i8080_rlc,
	[0x08] = i8080_ill,
	[0x09] = i8080_dad,
	[0x0a] = i8080_ldax,
	[0x0b] = i8080_dcx,
	[0x0c] = i8080_inr,
	[0x0d] = i8080_dcr,
	[0x0e] = i8080_mvi,
	[0x0f] = i8080_rrc,
	[0x10] = i8080_ill,
	[0x11] = i8080_lxi,
	[0x12] = i8080_stax,
	[0x13] = i8080_inx,
	[0x14] = i8080_inr,
	[0x15] = i8080_dcr,
	[0x16] = i8080_mvi,
	[0x17] = i8080_ral,
	[0x18] = i8080_ill,
	[0x19] = i8080_dad,
	[0x1a] = i8080_ldax,
	[0x1b] = i8080_dcx,
	[0x1c] = i8080_inr,
	[0x1d] = i8080_dcr,
	[0x1e] = i8080_mvi,
	[0x1f] = i8080_rar,
	//		[0x20] = i8080_rim,
	[0x20] = i8080_ill,
	[0x21] = i8080_lxi,
	[0x22] = i8080_shld,
	[0x23] = i8080_inx,
	[0x24] = i8080_inr,
	[0x25] = i8080_dcr,
	[0x26] = i8080_mvi,
	[0x27] = i8080_daa,
	[0x28] = i8080_ill,
	[0x29] = i8080_dad,
	[0x2a] = i8080_lhld,
	[0x2b] = i8080_dcx,
	[0x2c] = i8080_inr,
	[0x2d] = i8080_dcr,
	[0x2e] = i8080_mvi,
	[0x2f] = i8080_cma,
    //		[0x30] = i8080_sim,
	[0x30] = i8080_ill,
	[0x31] = i8080_lxi,
	[0x32] = i8080_sta,
	[0x33] = i8080_inx,
	[0x34] = i8080_inr,
	[0x35] = i8080_dcr,
	[0x36] = i8080_mvi,
	[0x37] = i8080_stc,
	[0x38] = i8080_ill,
	[0x39] = i8080_dad,
	[0x3a] = i8080_lda,
	[0x3b] = i8080_dcx,
	[0x3c] = i8080_inr,
	[0x3d] = i8080_dcr,
	[0x3e] = i8080_mvi,
	[0x3f] = i8080_cmc,
	[0x40] = i8080_mov,
	[0x41] = i8080_mov,
	[0x42] = i8080_mov,
	[0x43] = i8080_mov,
	[0x44] = i8080_mov,
	[0x45] = i8080_mov,
	[0x46] = i8080_mov,
	[0x47] = i8080_mov,
	[0x48] = i8080_mov,
	[0x49] = i8080_mov,
	[0x4a] = i8080_mov,
	[0x4b] = i8080_mov,
	[0x4c] = i8080_mov,
	[0x4d] = i8080_mov,
	[0x4e] = i8080_mov,
	[0x4f] = i8080_mov,
	[0x50] = i8080_mov,
	[0x51] = i8080_mov,
	[0x52] = i8080_mov,
	[0x53] = i8080_mov,
	[0x54] = i8080_mov,
	[0x55] = i8080_mov,
	[0x56] = i8080_mov,
	[0x57] = i8080_mov,
	[0x58] = i8080_mov,
	[0x59] = i8080_mov,
	[0x5a] = i8080_mov,
	[0x5b] = i8080_mov,
	[0x5c] = i8080_mov,
	[0x5d] = i8080_mov,
	[0x5e] = i8080_mov,
	[0x5f] = i8080_mov,
	[0x60] = i8080_mov,
	[0x61] = i8080_mov,
	[0x62] = i8080_mov,
	[0x63] = i8080_mov,
	[0x64] = i8080_mov,
	[0x65] = i8080_mov,
	[0x66] = i8080_mov,
	[0x67] = i8080_mov,
	[0x68] = i8080_mov,
	[0x69] = i8080_mov,
	[0x6a] = i8080_mov,
	[0x6b] = i8080_mov,
	[0x6c] = i8080_mov,
	[0x6d] = i8080_mov,
	[0x6e] = i8080_mov,
	[0x6f] = i8080_mov,
	[0x70] = i8080_mov,
	[0x71] = i8080_mov,
	[0x72] = i8080_mov,
	[0x73] = i8080_mov,
	[0x74] = i8080_mov,
	[0x75] = i8080_mov,
	[0x76] = i8080_hlt,
	[0x77] = i8080_mov,
	[0x78] = i8080_mov,
	[0x79] = i8080_mov,
	[0x7a] = i8080_mov,
	[0x7b] = i8080_mov,
	[0x7c] = i8080_mov,
	[0x7d] = i8080_mov,
	[0x7e] = i8080_mov,
	[0x7f] = i8080_mov,
	[0x80] = i8080_add,
	[0x81] = i8080_add,
	[0x82] = i8080_add,
	[0x83] = i8080_add,
	[0x84] = i8080_add,
	[0x85] = i8080_add,
	[0x86] = i8080_add,
	[0x87] = i8080_add,
	[0x88] = i8080_adc,
	[0x89] = i8080_adc,
	[0x8a] = i8080_adc,
	[0x8b] = i8080_adc,
	[0x8c] = i8080_adc,
	[0x8d] = i8080_adc,
	[0x8e] = i8080_adc,
	[0x8f] = i8080_adc,
	[0x90] = i8080_sub,
	[0x91] = i8080_sub,
	[0x92] = i8080_sub,
	[0x93] = i8080_sub,
	[0x94] = i8080_sub,
	[0x95] = i8080_sub,
	[0x96] = i8080_sub,
	[0x97] = i8080_sub,
	[0x98] = i8080_sbb,
	[0x99] = i8080_sbb,
	[0x9a] = i8080_sbb,
	[0x9b] = i8080_sbb,
	[0x9c] = i8080_sbb,
	[0x9d] = i8080_sbb,
	[0x9e] = i8080_sbb,
	[0x9f] = i8080_sbb,
	[0xa0] = i8080_ana,
	[0xa1] = i8080_ana,
	[0xa2] = i8080_ana,
	[0xa3] = i8080_ana,
	[0xa4] = i8080_ana,
	[0xa5] = i8080_ana,
	[0xa6] = i8080_ana,
	[0xa7] = i8080_ana,
	[0xa8] = i8080_xra,
	[0xa9] = i8080_xra,
	[0xaa] = i8080_xra,
	[0xab] = i8080_xra,
	[0xac] = i8080_xra,
	[0xad] = i8080_xra,
	[0xae] = i8080_xra,
	[0xaf] = i8080_xra,
	[0xb0] = i8080_ora,
	[0xb1] = i8080_ora,
	[0xb2] = i8080_ora,
	[0xb3] = i8080_ora,
	[0xb4] = i8080_ora,
	[0xb5] = i8080_ora,
	[0xb6] = i8080_ora,
	[0xb7] = i8080_ora,
	[0xb8] = i8080_cmp,
	[0xb9] = i8080_cmp,
	[0xba] = i8080_cmp,
	[0xbb] = i8080_cmp,
	[0xbc] = i8080_cmp,
	[0xbd] = i8080_cmp,
	[0xbe] = i8080_cmp,
	[0xbf] = i8080_cmp,
	[0xc0] = i8080_rccc,
	[0xc1] = i8080_pop,
	[0xc2] = i8080_jccc,
	[0xc3] = i8080_jmp,
	[0xc4] = i8080_cccc,
	[0xc5] = i8080_push,
	[0xc6] = i8080_adi,
	[0xc7] = i8080_rst,
	[0xc8] = i8080_rccc,
	[0xc9] = i8080_ret,
	[0xca] = i8080_jccc,
	[0xcb] = i8080_ill,
	[0xcc] = i8080_cccc,
	[0xcd] = i8080_call,
	[0xce] = i8080_aci,
	[0xcf] = i8080_rst,
	[0xd0] = i8080_rccc,
	[0xd1] = i8080_pop,
	[0xd2] = i8080_jccc,
	[0xd3] = i8080_out,
	[0xd4] = i8080_cccc,
	[0xd5] = i8080_push,
	[0xd6] = i8080_sui,
	[0xd7] = i8080_rst,
	[0xd8] = i8080_rccc,
	[0xd9] = i8080_ill,
	[0xda] = i8080_jccc,
	[0xdb] = i8080_in,
	[0xdc] = i8080_cccc,
	[0xde] = i8080_sbi,
	[0xdf] = i8080_rst,
	[0xe0] = i8080_rccc,
	[0xe1] = i8080_pop,
	[0xe2] = i8080_jccc,
	[0xe3] = i8080_xthl,
	[0xe4] = i8080_cccc,
	[0xe5] = i8080_push,
	[0xe6] = i8080_ani,
	[0xe7] = i8080_rst,
	[0xe8] = i8080_rccc,
	[0xe9] = i8080_pchl,
	[0xea] = i8080_jccc,
	[0xeb] = i8080_xchg,
	[0xec] = i8080_cccc,
	[0xed] = i8080_ill,
	[0xee] = i8080_xri,
	[0xef] = i8080_rst,
	[0xf0] = i8080_rccc,
	[0xf1] = i8080_pop,
	[0xf2] = i8080_jccc,
	[0xf3] = i8080_di,
	[0xf4] = i8080_cccc,
	[0xf5] = i8080_push,
	[0xf6] = i8080_ori,
	[0xf7] = i8080_rst,
	[0xf8] = i8080_rccc,
	[0xf9] = i8080_sphl,
	[0xfa] = i8080_jccc,
	[0xfb] = i8080_ei,
	[0xfc] = i8080_cccc,
	[0xfd] = i8080_ill,
	[0xfe] = i8080_cpi,
	[0xff] = i8080_rst
};

void i8080_cycle(intel8080_t *cpu)
{
	cpu->registers.status.m1 = 1;
	if(cpu->interrupt_request)
	{
		cpu->interrupt_request = false;
		cpu->data_bus = cpu->interrupt_instruction;
		cpu->registers.status.interrupt = false;
		if(memory[cpu->registers.sp] != 0x76) // hlt opcode
		{
			// if we are not currently sitting on a
			// hlt instruction, make sure we continue
			// executing on the current instruction
			cpu->registers.pc -= 1;
		}
		else
		{
            
		}
	}
	else
	{
		i8080_fetch_next_op(cpu);
	}
    
	cpu->current_op_code = cpu->data_bus;
#ifdef CPU_DEBUG_ENABLED
	if(cpu->breakpoint_enabled && cpu->address_bus == cpu->breakpoint)
	{
		printf("BREAKPOINT HIT\n");
		cpu->trace_instr = true;
		cpu->stepping = true;
	}
    
	if(cpu->trace_instr)
	{
		i8080_print_state(cpu);
		i8080_disasm(cpu);
		if(cpu->stepping)
		{
			if(getc(stdin) == 'g')
			{
				cpu->stepping = false;
			}
		}
	}
#endif
	i8080_opcodes[cpu->current_op_code](cpu);
    
	cpu->registers.flags_byte &= 0xD7;
	cpu->registers.flags_byte |= 0x2;
	cpu->cached_status = cpu->registers.status_byte;
	bool interrupt_status = cpu->registers.status.interrupt;
	cpu->registers.status_byte = 0;
	cpu->registers.status.interrupt = interrupt_status;
}
