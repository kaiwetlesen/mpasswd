#include <stdio.h>
#include <stdint.h>
#define BUFFER 32

int main(void) {
	char buffer[BUFFER], *mover;
	printf("Type something: ");
	fgets(buffer, BUFFER, stdin);
	
	for (mover = buffer; *mover; mover++) {
		printf("0x%02X ", (uint8_t)*mover);
	}
	return 0;
}
