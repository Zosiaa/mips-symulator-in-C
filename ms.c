#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//some definitions
#define FALSE 0
#define TRUE 1
//clock cycles
long long cycles = 0;
// registers
int regs[32];
// program counter
unsigned long pc = 0;
// memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
unsigned long inst_mem[INST_MEM_SIZE]; //instruction memory
unsigned long data_mem[DATA_MEM_SIZE]; //data memory

//misc. function
int init(char* filename);
void print_cycles();
void print_reg();
void print_pc();
int fetch();
void decode(unsigned long instr);
void addidec(unsigned long instr);
void adddec(unsigned long instr);
void beqdec(unsigned long instr);
void jdec(unsigned long instr);
void jrdec(unsigned long instr);
void jaldec(unsigned long instr);
void sltidec(unsigned long instr);
void swdec(unsigned long instr);
void lwdec(unsigned long instr);
void execute(char* funct, int rt, int rs, int x);
void update_pc(int imm, int multi, int rs);
int mem(int adr);
void wb(char* funct, int rt, int rs, int x);
void update_mem(int rt, int addr);

char* long_to_binary(unsigned long k);
int binary_to_decimal(char* n);
int bianry_to_decimal_neg(char* n);

int main(int ac, char *av[])
{
	if (ac <3 )
	{
		printf("./mips_sim filename mode\n");
        return -1;
	}
	
	char done = FALSE;
	if(init(av[1])!=0)
        return -1;
	int counter = 0;
	while (!done)
	{
 
		unsigned long instr = fetch();     //fetch an instruction from the instruction memory
		decode(instr);    //decode the instruction and read data from register file
		  /* Todo
		exe();       //perform an appropriate operation 
		mem();       //access the data memory
		wb();        //write result of arithmetic operation or data read from the data memory to register file
		update_pc(); //update the pc to the address of the next instruction
    */

		cycles++;    //increase clock cycle

		//if debug mode, print clock cycle, pc, reg
		if(*av[2]=='0'){
					 print_cycles();  //print clock cycles
					 print_pc();		 //print pc
					 print_reg();	 //print registers
        }
        // check the exit condition 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = TRUE;
		counter++;
	}

    if(*av[2]=='1')
    {
        print_cycles();  //print clock cycles
        print_pc();		 //print pc
        print_reg();	 //print registers
    }

	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/

int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}
	/*reset the registers*/
	for (i = 0; i < 32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
	/*reset clock cycles*/
	cycles = 0;
	return 0;
}


void wb(char* funct, int rt, int rs, int x)
{
	if (!strcmp(funct, "addi"))
	{
		regs[rt] = regs[rs] + x;
	}
	if (!strcmp(funct, "add"))
	{
		regs[x] = regs[rs] + regs[rt];
	}
	if (!strcmp(funct, "slti"))
	{
		regs[rt] = (regs[rs] < x);
	}
}
int mem(int adr)
{
	return data_mem[adr];
}
void update_mem(int rt, int addr)
{
	data_mem[addr] = regs[rt];
}

void execute(char* funct, int rt, int rs, int x)
{
	if (!strcmp(funct, "addi"))
	{
		wb("addi", rt, rs, x);
		update_pc(0, 1, 0);
	}
	if (!strcmp(funct, "add"))
	{
		wb("add", rt, rs, x);
		update_pc(0, 1, 0);
	}
	if (!strcmp(funct, "beq"))
	{
		if (regs[rt] == regs[rs])
			update_pc(x, 1, 0);
		else update_pc(0, 1, 0);
	}
	if (!strcmp(funct, "slti"))
	{
		wb("slti", rt, rs, x);
		update_pc(0, 1, 0);
	}
	if (!strcmp(funct, "lw"))
	{
		int addr = regs[rs] + x;
		regs[rt] = mem(addr / 4);
		update_pc(0, 1, 0);

	}
	if (!strcmp(funct, "swdec"))
	{
		int addr = regs[rs] + x;
		update_mem(rt, addr / 4);
		update_pc(0, 1, 0);
	}

}
int fetch()
{	
	unsigned long number = pc / 4;
	pc = pc + 4;
	return inst_mem[number];
}

void decode(unsigned long instr)
{      
	size_t buffersize = 256; 
	unsigned long opcode;
	opcode = instr >> 24;
	
	char* bin = long_to_binary(instr);
	char imm[16];
	char rt[5];
	char rs[5];
  char* opc = malloc(buffersize);
  char* func = malloc(buffersize);
	memcpy(func, bin + 58 /* Offset */, 6 /* Length */);
	memcpy(opc, bin + 32 /* Offset */, 6 /* Length */);

	int decim = strtol(imm, NULL, 2);

	if (!strcmp(opc, "001000"))
		addidec(instr);
	else if (!strcmp(opc, "000010"))
		jdec(instr);
	else if (!strcmp(opc, "000100"))
		beqdec(instr);
	else if (!strcmp(opc, "001010"))
		sltidec(instr);
	else if (!strcmp(opc, "100011"))
		lwdec(instr);
	else if (!strcmp(opc, "101011"))
		swdec(instr);
	else if (!strcmp(opc, "000000"))
	{	if (!strcmp(func, "001000"))
		jrdec(instr);
	else adddec(instr);
	}
	else if (!strcmp(opc, "000011"))
		jaldec(instr);
}

void addidec(unsigned long instr)
{
	//iformat
	//printf("\n ADDI! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256; 
	char* imm = malloc(buffersize);
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);

	memcpy(imm, bin + 48 /* Offset */, 16 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int immint, rtint, rsint;
	if (imm[0]=='0')
	{ 
		immint = binary_to_decimal(imm);
	}
	else immint = bianry_to_decimal_neg(imm);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);

	execute("addi", rtint, rsint, immint);
}


void adddec(unsigned long instr)
{
	//R format
	//printf("\n ADD! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256; 
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);
	char* rd = malloc(buffersize);

	memcpy(rd, bin + 48 /* Offset */, 5 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int rdint, rtint, rsint;
	rdint = binary_to_decimal(rd);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);

	execute("add", rtint, rsint, rdint);
}
void beqdec(unsigned long instr)
{
	//printf("\n BEQ! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256; 
	char* off = malloc(buffersize);
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);
	memcpy(off, bin + 48 /* Offset */, 16 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int offint, rtint, rsint;
	offint = binary_to_decimal(off);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);

	execute("beq", rtint, rsint, offint);

}
void jdec(unsigned long instr)
{
	//printf("\nJUMP! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256;
	char* imm = malloc(buffersize);

	memcpy(imm, bin + 38 /* Offset */, 26 /* Length */);
	int immint;
	char* pc_bin = long_to_binary(pc);

	//execute
	char new_pc[65];
	new_pc[64] = '\0';
	for (int z = 0; z < 32; z++)
		new_pc[z] = '0';

	for (int i = 0; i < 4; i++)
		new_pc[32 + i] = pc_bin[32 + i];

	for (int i = 4; i < 30; i++)
		new_pc[32 + i] = imm[i - 4];
	new_pc[62] = '0';
	new_pc[63] = '0';

	int newPC = binary_to_decimal(new_pc);

	pc = newPC;

}

void jrdec(unsigned long instr)
{
	//printf("\nJUMP REGISTER! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256; 
	char* rs = malloc(buffersize);
	
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);
	int rsint;
	rsint = binary_to_decimal(rs);
	update_pc(0, 1, rsint); //update X4 plus PC
}

void jaldec(unsigned long instr)
{
	//printf("\nJUMP AND LINK! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256;
	char* imm = malloc(buffersize);
	memcpy(imm, bin + 38 /* Offset */, 26 /* Length */);
	// r31=pc; pc=target<<2
	int immint;
	immint = binary_to_decimal(imm);
	regs[31] = pc;
	update_pc(immint, 0 , 0); 

}
void sltidec(unsigned long instr)
{
	//printf("\n SLTI! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256; 
	char* imm = malloc(buffersize);
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);

	memcpy(imm, bin + 48 /* Offset */, 16 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int immint, rtint, rsint;
	immint = binary_to_decimal(imm);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);
	
	execute("slti", rtint, rsint, immint);

}
void swdec(unsigned long instr)
{
	//printf("\n swdec! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256;
	char* off = malloc(buffersize);
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);

	memcpy(off, bin + 48 /* Offset */, 16 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int offint, rtint, rsint;
	offint = binary_to_decimal(off);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);
	
	execute("swdec", rtint, rsint, offint);
}

void lwdec(unsigned long instr)
{
	//printf("\n LW! \n");
	char* bin = long_to_binary(instr);

	size_t buffersize = 256;
	char* off = malloc(buffersize);
	char* rt = malloc(buffersize);
	char* rs = malloc(buffersize);
	memcpy(off, bin + 48 /* Offset */, 16 /* Length */);
	memcpy(rt, bin + 43 /* Offset */, 5 /* Length */);
	memcpy(rs, bin + 38 /* Offset */, 5 /* Length */);

	int offint, rtint, rsint;
	offint = binary_to_decimal(off);
	rtint = binary_to_decimal(rt);
	rsint = binary_to_decimal(rs);
	
	execute("lw", rtint, rsint, offint);
}

char* long_to_binary(unsigned long input)
{
	static char c[33];
	c[0] = '\0';
	unsigned long val;

	for (val = 1UL << (sizeof(unsigned long) * 8 - 1); val > 0; val >>= 1)
	{
		strcat(c, ((input & val) == val) ? "1" : "0");
	}
	return c;
}

int binary_to_decimal(char* n)
{
		int dec = 0;
		int base = 1;
		int len = strlen(n);

		for (int i = len - 1; i >= 0; i--) {
			if (n[i] == '1')
				dec += base;
			base = base * 2;
		}
		return dec;
}

int bianry_to_decimal_neg(char* value)
{

	int length = strlen(value);
	unsigned int result = 0;
	unsigned int signExtend;
	unsigned int negResult = 0;


	for (int i = 0; i < length; i++) {
		result = result << 1;
		if (value[i] == '1')
			result += 1;
	}

	if (value[0] == '1') {
		signExtend = (1 << (length - 1));
		signExtend -= 1;
		signExtend = ~signExtend;
		result |= signExtend;

		negResult = ~result;
		negResult += 1;
		return result;
	}
	else {
		return result;
	}
}


void update_pc(int imm, int multi, int rs)
{
	if (rs != 0)
	{
		pc = regs[rs];
	}
	if (multi == 0)
	{
		pc = imm*4;
	}
	else (pc = pc + 4* imm);
}


void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}


void print_pc()
{
	printf("PC	   = %ld\n\n", pc);
}


void print_reg()
{
	printf("R0   [r0] = %d\n", regs[0]);
	printf("R1   [at] = %d\n", regs[1]);
	printf("R2   [v0] = %d\n", regs[2]);
	printf("R3   [v1] = %d\n", regs[3]);
	printf("R4   [a0] = %d\n", regs[4]);
	printf("R5   [a1] = %d\n", regs[5]);
	printf("R6   [a2] = %d\n", regs[6]);
	printf("R7   [a3] = %d\n", regs[7]);
	printf("R8   [t0] = %d\n", regs[8]);
	printf("R9   [t1] = %d\n", regs[9]);
	printf("R10  [t2] = %d\n", regs[10]);
	printf("R11  [t3] = %d\n", regs[11]);
	printf("R12  [t4] = %d\n", regs[12]);
	printf("R13  [t5] = %d\n", regs[13]);
	printf("R14  [t6] = %d\n", regs[14]);
	printf("R15  [t7] = %d\n", regs[15]);
	printf("R16  [s0] = %d\n", regs[16]);
	printf("R17  [s1] = %d\n", regs[17]);
	printf("R18  [s2] = %d\n", regs[18]);
	printf("R19  [s3] = %d\n", regs[19]);
	printf("R20  [s4] = %d\n", regs[20]);
	printf("R21  [s5] = %d\n", regs[21]);
	printf("R22  [s6] = %d\n", regs[22]);
	printf("R23  [s7] = %d\n", regs[23]);
	printf("R24  [t8] = %d\n", regs[24]);
	printf("R25  [t9] = %d\n", regs[25]);
	printf("R26  [k0] = %d\n", regs[26]);
	printf("R27  [k1] = %d\n", regs[27]);
	printf("R28  [gp] = %d\n", regs[28]);
	printf("R29  [sp] = %d\n", regs[29]);
	printf("R30  [fp] = %d\n", regs[30]);
	printf("R31  [ra] = %d\n", regs[31]);
	printf("\n");
}
