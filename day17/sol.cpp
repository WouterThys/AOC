
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h> // strstr

#include <fstream>
#include <sstream>
#include <iostream>

#define BUFSIZE 100
#define MAX_LEN 16

static intmax_t reg_A, reg_B, reg_C;
static uint_fast8_t in[MAX_LEN], out[MAX_LEN];
static uint_fast8_t ipc, in_len, out_len;

void parse_input(const char *buffer)
{
	// Ignore all register values as we're going to write them anyway.
	// Per the program description B and C could be set, but they're always 0.
	buffer = strstr(buffer, "Program: ") + sizeof("Program:");
	while (*buffer) {
		in[in_len++] = *buffer - '0';
		buffer += 2;
	}

        // This code with -O3 has a bizarre optimisation bug where it happily
        // ignores the while (*buffer) and sets in_len to a fixed MAX_LEN+1.
        // Rather than debug it, I'm doing this. LOL
        if (in_len > 16) in_len = 16;
}

intmax_t combo(int_fast8_t operand)
{
	switch (operand) {
	case 0:
	case 1:
	case 2:
	case 3:
		return operand;
	case 4:
		return reg_A;
	case 5:
		return reg_B;
	case 6:
		return reg_C;
	default:
		assert(0);
	}
}

void opcode_adv(int_fast8_t operand)
{
	reg_A = reg_A >> combo(operand);
}

void opcode_bdv(int_fast8_t operand)
{
	reg_B = reg_A >> combo(operand);
}

void opcode_cdv(int_fast8_t operand)
{
	reg_C = reg_A >> combo(operand);
}

void opcode_bxl(int_fast8_t operand)
{
	reg_B = reg_B ^ operand;
}

void opcode_bst(int_fast8_t operand)
{
	reg_B = combo(operand) & 7;
}

void opcode_jnz(int_fast8_t operand)
{
	if (reg_A != 0) ipc = operand;
}

void opcode_bxc(int_fast8_t _)
{
	reg_B = reg_B ^ reg_C;
}

void opcode_out(int_fast8_t operand)
{
	out[out_len++] = combo(operand) & 7;
}

static int_fast8_t next()
{
	return in[ipc++];
}

void run()
{
	int_fast8_t opcode = next();
	int_fast8_t operand = next();

	switch (opcode) {
	case 0:
		opcode_adv(operand);
		break;
	case 1:
		opcode_bxl(operand);
		break;
	case 2:
		opcode_bst(operand);
		break;
	case 3:
		opcode_jnz(operand);
		break;
	case 4:
		opcode_bxc(operand);
		break;
	case 5:
		opcode_out(operand);
		break;
	case 6:
		opcode_bdv(operand);
		break;
	case 7:
		opcode_cdv(operand);
		break;
	default:
		assert(0);
	}
}

char eval(intmax_t a)
{
	reg_A = a; reg_B = 0; reg_C = 0;
	ipc = 0; out_len = 0;
	while (ipc < in_len) {
		run();
	}
	//dbgln_arr("sim", "%i", out, out_len);
	return out[0];
}

intmax_t dfs(intmax_t in_pos, intmax_t curr)
{
	if (in_pos < 0) return curr;

	curr <<= 3;
	uint_fast8_t target = in[in_pos];
	for (uint_fast8_t i = 0; i < 8; ++i) {
		intmax_t n = curr + i;
		if (eval(n) == target) {
			//dbgln("hit %i (%o)", n, n);
			intmax_t out = dfs(in_pos - 1, n);
			if (out >= 0) return out;
		}
	}
	return -1;
}

int main()
{
#ifdef TIMER
	timer_start();
#endif

	parse_input("Program: 2,4,1,3,7,5,0,3,1,5,4,4,5,5,3,0");
	// dbgln("A: %ji, B: %ji, C: %ji", reg_A, reg_B, reg_C);
	// dbgln_arr("in:", "%i", in, in_len);

	printf("%ji\n", dfs(in_len - 1, 0));
#ifdef TIMER
	printf("Time: %.0f us\n", timer_stop_us());
#endif
}

