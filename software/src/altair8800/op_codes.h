#ifndef _OP_CODES_H_
#define _OP_CODES_H_

#define DESTINATION(x)     (x >> 3 & 7)
#define SOURCE(x)          (x & 7)
#define CONDITION(x)       (x >> 3 & 7)
#define VECTOR(x)          (x >> 3 & 7)
#define RP(x)              (x >> 4 & 3)

#define REGISTER_A         7
#define REGISTER_B         0
#define REGISTER_C         1
#define REGISTER_D         2
#define REGISTER_E         3
#define REGISTER_H         4
#define REGISTER_L         5
#define MEMORY_ACCESS      6

#define PAIR_BC            0
#define PAIR_DE            1
#define PAIR_HL            2
#define PAIR_SP            3

#define CONDITION_NZ       0
#define CONDITION_Z		1
#define CONDITION_NC       2
#define CONDITION_C        3
#define CONDITION_PO       4
#define CONDITION_PE       5
#define CONDITION_P        6
#define CONDITION_M        7

#endif
