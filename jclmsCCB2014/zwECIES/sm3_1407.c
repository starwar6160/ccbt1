
#include "SM3.h"
#include <math.h>

//消息扩展与压缩函数
static void SM3_transform(SM3 * sm)
{
	unsigned int a, b, c, d, e, f, g, h;
	unsigned int w2[64];
	unsigned int t1, t2, s1, s2;
	int j;

	//消息扩展

	for (j = 16; j < 68; j++) {
		sm->w[j] =
		    P1(sm->
		       w[j - 16] ^ sm->w[j - 9] ^ L_R(sm->w[j - 3],
						      15)) ^ L_R(sm->w[j - 13],
								 7) ^ sm->w[j -
									    6];
		/*
		   printf("%08x ",sm->w[j]);
		   if(((j+1) % 8) == 0) printf("\n");
		 */
	}
	for (j = 0; j < 64; j++) {
		w2[j] = sm->w[j] ^ sm->w[j + 4];
		/*
		   printf("%08x ",w2[j]);
		   if(((j+1) % 8) == 0) printf("\n");
		 */
	}

	a = sm->h[0];
	b = sm->h[1];
	c = sm->h[2];
	d = sm->h[3];
	e = sm->h[4];
	f = sm->h[5];
	g = sm->h[6];
	h = sm->h[7];

	//压缩函数
	for (j = 0; j < 16; j++) {
		s1 = L_R((L_R(a, 12) + e + L_R(Tj_0_to_15, j)), 7);
		s2 = s1 ^ L_R(a, 12);
		t1 = FF_j_0_to_15(a, b, c) + d + s2 + w2[j];
		t2 = GG_j_0_to_15(e, f, g) + h + s1 + sm->w[j];
		d = c;
		c = L_R(b, 9);
		b = a;
		a = t1;
		h = g;
		g = L_R(f, 19);
		f = e;
		e = P0(t2);
		//       printf("%02d %08x %08x %08x %08x %08x %08x %08x %08x\n", j, a, b, c, d, e, f, g, h);
	}
	for (j = 16; j < 64; j++) {
		s1 = L_R((L_R(a, 12) + e + L_R(Tj_16_to_63, j)), 7);
		s2 = s1 ^ L_R(a, 12);
		t1 = FF_j_16_to_63(a, b, c) + d + s2 + w2[j];
		t2 = GG_j_16_to_63(e, f, g) + h + s1 + sm->w[j];
		d = c;
		c = L_R(b, 9);
		b = a;
		a = t1;
		h = g;
		g = L_R(f, 19);
		f = e;
		e = P0(t2);
		//      printf("%02d %08x %08x %08x %08x %08x %08x %08x %08x\n", j, a, b, c, d, e, f, g, h);
	}
	sm->h[0] ^= a;
	sm->h[1] ^= b;
	sm->h[2] ^= c;
	sm->h[3] ^= d;
	sm->h[4] ^= e;
	sm->h[5] ^= f;
	sm->h[6] ^= g;
	sm->h[7] ^= h;

}

//SM3初始化
void SM3_init(SM3 * sm)
{
	int i;
	sm->length[0] = 0;
	sm->length[1] = 0;
	sm->h[0] = H0;
	sm->h[1] = H1;
	sm->h[2] = H2;
	sm->h[3] = H3;
	sm->h[4] = H4;
	sm->h[5] = H5;
	sm->h[6] = H6;
	sm->h[7] = H7;
	for (i = 0; i < 68; i++) {
		sm->w[i] = 0;
	}
}

void SM3_process(SM3 * sm, int byte)
{
	int cnt;
	cnt = (int)((sm->length[0] / 32) % 16);
	sm->w[cnt] <<= 8;
	sm->w[cnt] |= (unsigned int)(byte & 0xFF);
	sm->length[0] += 8;
	if (sm->length[0] == 0) {
		sm->length[1]++;
		sm->length[0] = 0;
	}
	if ((sm->length[0] % 512) == 0) {
		SM3_transform(sm);
	}
}

void SM3_hash(SM3 * sm, char hash[HASHLEN])
{
	unsigned int len0, len1;
	int i;
	len0 = sm->length[0];
	len1 = sm->length[1];
	SM3_process(sm, PAD);
	while ((sm->length[0] % 512) != 448)
		SM3_process(sm, ZERO);
	sm->w[14] = len1;
	sm->w[15] = len0;

	SM3_transform(sm);
	for (i = 0; i < 32; i++) {
		hash[i] = (char)((sm->h[i / 4] >> (8 * (3 - i % 4))) & 0xFF);
	}
	SM3_init(sm);
}
