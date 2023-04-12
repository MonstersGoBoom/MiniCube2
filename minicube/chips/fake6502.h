void reset6502();
void exec6502(uint32_t tickcount);
void step6502();
void irq6502();
void nmi6502();

extern uint16_t pc;
extern uint8_t sp, a, x, y, status;
extern char irq_active;
extern uint32_t clockticks6502, clockgoal6502;
extern uint32_t instructions;

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);
int disasm6502(uint16_t pc, char *line, unsigned int max_line);
void hookexternal(void *funcptr);

void Label_Add(const char *fname,uint16_t value);
void Labels_Dispose();
