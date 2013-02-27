/*
 * (incomplete) commandline ChipTAN flickerer
 *
 * Copyright (C) 2011 by Steve Markgraf <steve@steve-m.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

//#define DEBUG

void put_seq(uint8_t data)
{
	int i, j;
	uint8_t mask;
#ifdef DEBUG
	if (data & 1)
		printf(" z");
	else
		printf(" n");

	printf("%01x", data>>1);
	fflush(stdout);
	return;
#endif
	//fputs(
	printf("\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A\033[2K");
	

	for (j = 0; j < 4; j++) {
		mask = 1;
		printf("    ");
		for (i = 0; i < 5; i++) {
			if (data & mask) {
				printf("\e[5;34;47m   \e[0m  ");
			} else
				printf("     ");
			mask <<= 1;
		}
		printf("\n");
	}
	printf("\r Data: %01x\r", data>>1);

	fflush(stdout);
	rewind(stdout);
}

#define DELAY 45000

int main(int argc, char **argv)
{
	int i;
	uint8_t j = 0;
	uint8_t data;
	char *optstr;

	char seq[38] = "0FFF1000000000000000000000000000000150";

	if (argc > 1){
		optstr = argv[1];
		for (i = 0; i < 37; i++)
			seq[i] = optstr[i];
	}

	printf("     V                   V \n\n\n\n\n");
	fflush(stdout);

	/* convert ASCII-string to binary */
	for(i = 0; i < 38; i++) {
		if (seq[i] > 0x39) /* A-F */
			seq[i] = (seq[i] - (0x30 + 7));
		else
			seq[i] = (seq[i] - 0x30);
	}

	int sum_even = 0;
	int sum_odd = 0;
	int checksum = 0;
	/* calculate checksum */
	for (i = 10; i < 36; i++) {//i=5
		/* odd */
		if (seq[i] % 2) {
			sum_odd += seq[i];
		} else {
		/* even */
			sum_even += seq[i];
		}
	}

	checksum =  (1*(sum_even % 10)) + (2*(sum_odd % 10));
	checksum = checksum % 10;

	char lrz = 0;
	/* calculate XOR-LRZ (byte/nibble 37)*/
	for(i = 4; i < 36; i++)
		lrz ^= seq[i];

	seq[37] = lrz;

	char tmp;
	/* swap bytes for transmission*/
	for (i = 4; i < 38; i+=2) {
		tmp  = seq[i];
		seq[i] = seq[i+1];
		seq[i+1] = tmp;
	}

	while (1) {
		if (j == 38) {
			j = 0;
#ifdef DEBUG
			printf("end\n");
#endif
		}
		data = seq[j] << 1;

		if (j == 3) {
			put_seq(data | 1);
			usleep(DELAY);
		} else {
			put_seq(data | 1);
			usleep(DELAY);
			put_seq(data);
			usleep(DELAY);
		}
		j++;
	}

	return 0;
}
