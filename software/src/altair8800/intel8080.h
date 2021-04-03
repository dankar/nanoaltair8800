#ifndef _INTEL8080_H_
#define _INTEL8080_H_

#include <stdint.h>
#include <stdbool.h>

#define FLAGS_CARRY   0x01
#define FLAGS_PARITY  0x04
#define FLAGS_H       0x10
#define FLAGS_ZERO    0x40
#define FLAGS_SIGN    0x80

#if defined(DEBUG_BUILD) || defined(TEST_BUILD)
#define CPU_DEBUG_ENABLED
#endif

typedef struct registers_t
{
	union
	{
		uint16_t af;
        
		struct 
        {
			union
			{
				uint8_t flags_byte;
				struct 
				{
					bool carry      : 1;
					bool unused_0   : 1;
					bool parity     : 1;
					bool unused_1   : 1;
					bool half_carry : 1;
					bool unused_2   : 1;
					bool zero       : 1;
					bool sign       : 1;
				} flags;
			};
			uint8_t a;
		};
	};
    
	union
	{
		uint16_t bc;
		struct
		{
			uint8_t c;
			uint8_t b;
		};
	};
    
	union
	{
		uint16_t de;
		struct
		{
			uint8_t e;
			uint8_t d;
            
		};
	};
    
	union
	{
		uint16_t hl;
		struct
		{
			uint8_t l;
			uint8_t h;
		};
	};
    
	union
	{
		uint8_t status_byte;
		struct 
		{
			bool interrupt : 1;
			bool wo        : 1;
			bool stack     : 1;
			bool hlta      : 1;
			bool out       : 1;
			bool m1        : 1;
			bool inp       : 1;
			bool memr      : 1;
		} status;
	};
    
	uint16_t sp;
	uint16_t pc;
} registers_t;


typedef void (*port_out)(uint8_t b);
typedef bool (*port_in)(uint8_t *b);
typedef uint8_t (*read_sense_switches)();

typedef struct port_io_t
{
	port_in in;
	port_out out;
} port_io_t;

typedef struct intel8080_t
{
	uint8_t     data_bus;
	uint16_t    address_bus;
	uint8_t     current_op_code;
	registers_t registers;
	port_io_t   ports[0x100];
	bool        run_state;
	uint8_t     interrupt_instruction;
	bool        interrupt_request;
	uint8_t     cached_status;
#ifdef CPU_DEBUG_ENABLED
	bool        trace_instr;
	uint16_t    breakpoint;
	bool        breakpoint_enabled;
	bool        stepping;
#endif
} intel8080_t;

void i8080_reset(intel8080_t *cpu);
void i8080_assign_port(intel8080_t *cpu, uint8_t port, port_io_t *io);

void i8080_deposit(intel8080_t *cpu, uint8_t data);
void i8080_deposit_next(intel8080_t *cpu, uint8_t data);
void i8080_examine(intel8080_t *cpu, uint16_t address);
void i8080_examine_next(intel8080_t *cpu);
void i8080_cycle(intel8080_t *cpu);

#ifdef CPU_DEBUG_ENABLED
void i8080_set_breakpoint(intel8080_t *cpu, uint16_t address);
void i8080_disable_breakpoint(intel8080_t *cpu);
#endif
// Safe to call from interrupts
void i8080_interrupt(intel8080_t *cpu, uint8_t rst_vector);

#endif
