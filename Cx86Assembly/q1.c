#include <stdio.h>
#include <conio.h>

__declspec(align(16)) float vi1[100];
__declspec(align(16)) float vi2[100];
__declspec(align(16)) float vo1[100];
__declspec(align(16)) float vo2[100];

float read_float(void);
void multiply(void);

int main()
{
	float a;
	float b;
	int i;
	float x;

	printf("enter a b: ");
	a = read_float();
	b = read_float();

	for(i = 0; i < 100; i++) {
	x = i*0.1f;
	vi1[i] = x + a;
	vi2[i] = x + b;
	vo1[i] = x*x + (x*(a+b)) + a*b;
	}

	multiply();

	for(i = 0; i < 100; i++) {
	printf("i=%d vi1=%.3f vi2=%.3f vo1=%.3f vo2=%.3f\n",i,vi1[i],vi2[i],vo1[i],vo2[i]);
	}


	printf("Press any key to continue...");
	_getch();
	return 0;
}


void multiply() {
	_asm{
		mov ecx, 0
	again:
		movaps xmm0, oword ptr vi1[ecx]
		movaps xmm1, oword ptr vi2[ecx]
		mulps xmm0, xmm1
		movaps oword ptr vo2[ecx], xmm0
		add ecx, 16
		cmp ecx, 400
		jnz again
	};

}

float read_float()
{
	int c;
	short sign;
	float result;
	float after_point;
	sign = 1;
	result = 0.0;
	after_point = 0.0;
	c = _getche();
	while(c != '\r' && c != ' '){
		if(c == '-') {

			sign = -1;

		} else if (c == '.') {

			after_point = 0.1f;

		} else if( c >= '0' && c <= '9'){

			if(after_point){

				result += (c - '0') * after_point;
				after_point /= 10;

			}else {

				result *= 10;
				result += c - '0';

			}
		}

		c = _getche();
	}

	if(c == '\r') printf("\n");



	return result*sign;
}