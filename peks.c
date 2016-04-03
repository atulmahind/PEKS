/*
 * peks.c
 *
 *  Created on: Jul 7, 2015
 *      Author: Atul Mahind
 */

#include "peks.h"

void sha512(const char *word, int word_size, 
		char hashed_word[SHA512_DIGEST_LENGTH*2+1]) 
{
	int i;
	unsigned char digest[SHA512_DIGEST_LENGTH];

	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, word, word_size);
	SHA512_Final(digest, &ctx);

	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
		sprintf(&hashed_word[i*2], "%02x", (unsigned int)digest[i]);
}

void get_n_bits(char* src, char* out, int bitswanted)
{
	char * ptr = out;
	char byte;
	int i;

	while(bitswanted)
	{
		byte = *src++;
		for(i = 7; i >= 0 && bitswanted > 0 ; i--, bitswanted--)
			*ptr++ = '0' + ((byte >> i) & 0x01);
		if(bitswanted == 0)
			break;
	}
}

void key_printf(key key)
{
	element_printf("α %B\n", key.priv);
	element_printf("g %B\n", key.pub.g);
	element_printf("h %B\n", key.pub.h);
}

void peks_printf(peks peks)
{
	element_printf("A %B\n", peks.A);
	printf("B %s\n", peks.B);
}

void init_pbc_param_pairing(pbc_param_t param, pairing_t pairing)
{
	int rbits = 160;
	int qbits = 512;

	/* Generate type A pairing parameters */
	pbc_param_init_a_gen(param, rbits, qbits);

	/* Initialize pairing */
	pairing_init_pbc_param(pairing, param);
#if defined(DEBUG)
	printf("Symmetric %d\n", pairing_is_symmetric(pairing));
#endif
}

void KeyGen(key *key, pbc_param_t param, pairing_t pairing)
{
	/* Private key - α */
	element_init_Zr(key->priv, pairing);
	element_random(key->priv);

	/* Public key - Apub = [g, h=g^α] */
	element_init_G1(key->pub.g, pairing);
	element_random(key->pub.g);
	element_init_G1(key->pub.h, pairing);
	element_pow_zn(key->pub.h, key->pub.g, key->priv);
}

void PEKS(peks *peks, key_pub *pub, pairing_t pairing,
		element_t H1_W, int bitswanted)
{
	char *H2_t = peks->B;
	element_t r, hR, t;

	/* hR = h^r */
	element_init_Zr(r ,pairing);
	element_random(r);
	element_init_G1(hR, pairing);
	element_pow_zn(hR, pub->h, r);

	/* t = hasedW1 X hR */
	element_init_GT(t, pairing);
	pairing_apply(t, H1_W, hR, pairing);

	/* gR = g^r */
	element_init_G1(peks->A, pairing);
	element_pow_zn(peks->A, pub->g, r);

	/* H2(t) */
	char *char_t = malloc(sizeof(char)*element_length_in_bytes(t));
	char *buffer = malloc(sizeof(char)*SHA512_DIGEST_LENGTH*2+1);
	element_snprint(char_t, element_length_in_bytes(t), t);
	sha512(char_t, element_length_in_bytes(t), buffer);
	get_n_bits(buffer, H2_t, bitswanted);

	free(char_t); char_t = NULL;
	free(buffer); buffer = NULL;
}

void Trapdoor(element_t Tw, pairing_t pairing, element_t alpha,
		element_t H1_W)
{
	/* H1(W)^α = hashedW1^alpha */
	element_init_G1(Tw, pairing);
	element_pow_zn(Tw, H1_W, alpha);
}

int Test(char *W2, int lenW2, key_pub *pub, element_t Tw, pairing_t pairing)
{
	/* PEKS for W2S */

	/* PEKS = [A, B] i.e. A=g^r and B=H2(t) */
	peks peks;
	
	element_t H1_W2;
	element_t temp;

	double P = mpz_get_d(pairing->r);
#if defined(DEBUG)
	printf("P %lf\n", P);
#endif	
	int nlogP = log2(P);
#if defined(DEBUG)
	printf("log2(P) %d\n", nlogP);
#endif
	/* H1(W2S) */
	char *hashedW2 = malloc(sizeof(char)*SHA512_DIGEST_LENGTH*2+1);
	sha512(W2, lenW2, hashedW2);
	element_init_G1(H1_W2, pairing);
	element_from_hash(H1_W2, hashedW2, strlen(hashedW2));
#if defined(DEBUG)
	element_printf("H1_W2 %B\n", H1_W2);
#endif

	/* PEKS(key_pub, W2) */
	peks.B = malloc(sizeof(char)*(nlogP));
	PEKS(&peks, pub, pairing, H1_W2, nlogP);
#if defined(DEBUG)
	element_printf("A %B\n", peks.A);
#endif
	element_init_GT(temp, pairing);
	pairing_apply(temp, Tw, peks.A, pairing);

	/* H2(temp) */
	char *char_temp = malloc(sizeof(char)*element_length_in_bytes(temp));
	char *hashed_temp = malloc(sizeof(char)*SHA512_DIGEST_LENGTH*2+1);
	element_snprint(char_temp, element_length_in_bytes(temp), temp);
	sha512(char_temp, element_length_in_bytes(temp), hashed_temp);
	char *H2_lhs = malloc(sizeof(char)*(nlogP));
	get_n_bits(hashed_temp, H2_lhs, nlogP);

	/* Check for equality */
#if defined(DEBUG)
	int i;
	printf("lhs ");
	for(i = 0; i < nlogP; i++)
		printf("%c", H2_lhs[i]);
	printf("\nrhs ");
	for(i = 0; i < nlogP; i++)
		printf("%c", peks.B[i]);
	printf("\n"); 
#endif
	int match;
	if(!memcmp(H2_lhs, peks.B, nlogP))
		match = 1;
	else 
		match = 0;

	/* Free the memory */
	free(H2_lhs); H2_lhs = NULL;
	free(peks.B); peks.B = NULL;
	free(char_temp); char_temp = NULL;
	free(hashed_temp); hashed_temp = NULL;
	free(hashedW2); hashedW2 = NULL;

	return match;
}

int peks_scheme(char* W1, char *W2)
{
	/* Order of group G1 and G2 */
	double P;

	/* Apriv = α and Apub = [g, h=g^α] */
	key key;

	/* Trapdoor */
	element_t Tw;

	/* H1(W) */
	element_t H1_W1;

	/* PBC data types */
	pbc_param_t param;
	pairing_t pairing;

	/* Initialize pairing */
	init_pbc_param_pairing(param, pairing);

	/* Get the order of G1 */
	P = mpz_get_d(pairing->r);
#if defined(DEBUG)
	printf("P %lf\n", P);
#endif
	//int nlogP = log2(P);

	/* KeyGen */
	KeyGen(&key, param, pairing);
#if defined(DEBUG)
	element_printf("α %e\n", key.priv);
#endif

	/* H1(W) */
	char *hashedW = malloc(sizeof(char)*SHA512_DIGEST_LENGTH*2+1);
	sha512(W1, (int)strlen(W1), hashedW);
	element_init_G1(H1_W1, pairing);
	element_from_hash(H1_W1, hashedW, (int)strlen(hashedW));
#if defined(DEBUG)
	element_printf("H1_W1 %B\n", H1_W1);
#endif

	/* Trapdoor */
	Trapdoor(Tw, pairing, key.priv, H1_W1);

	int match;
	match =	Test(W2, (int)strlen(W2), &key.pub, Tw, pairing);

	free(hashedW); hashedW = NULL;
	//free(peks.B); peks.B = NULL;
	pbc_param_clear(param);

	return match;
}
