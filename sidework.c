#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define buff_MAX 100
void readStrings(char input[buff_MAX]) {
	char sub[5];
	memcpy(sub, &input[0], 4);
	sub[4]='\0';
	if (strcmp(sub, "PLAY")==0) {
		printf("p");
	}
	else if (strcmp(sub, "MOVE") == 0) {
		printf("m");
	}
	else if (strcmp(sub, "RSGN") == 0) {
		printf("r");
	}
	else if (strcmp(sub, "DRAW") == 0) {
		printf("d");
	}
	else {
		printf("cmd not found");
	}
	return NULL;


}




int main() {
	char a[6] = "DRAWsd";
	readStrings(a);
	return 0;
}
