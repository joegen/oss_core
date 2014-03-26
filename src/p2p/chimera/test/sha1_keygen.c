/* Rama Alebouyeh */

#include <stdio.h>
#include <openssl/evp.h>
#include <stdlib.h>
#include "key.h"
#include "route.h"

char *sha1_keygen1 (char *key, char *digest, int size, int base);

main (int argc, char *argv[])
{
    // unsigned char *digest = (unsigned char*)malloc(40);
    // unsigned char *digest1 = (unsigned char*)malloc(80);
    // unsigned char *digest2 = (unsigned char*)malloc(160);

    unsigned char *digest;
    unsigned char *digest1;
    unsigned char *digest2;
    char input[128];
    int i, count, port;
    int keys = 0, base4 = 0;
    int base = MAX_COL;
    int key_size = MAX_ROW;

    port = 11111;

    if (argc < 3 || argc > 5)
	{
	    printf
		("Usage: sha1_keygen count hostname [KEYS_ONLY] [BASE4|BASE16]\n");
	    exit (0);
	}

    if (argc >= 4 && (strcmp (argv[3], "KEYS_ONLY") == 0))
	keys = 1;
    if (argc == 5 && (strcmp (argv[4], "BASE4") == 0))
	{
	    base4 = 1;
	    base = 4;
	    key_size = 80;
	}
    else if (strcmp (argv[4], "BASE16") == 0)
	{
	    base = 16;
	    key_size = 40;
	}

    count = atoi (argv[1]);

    for (i = 0; i < count; i++)
	{
	    sprintf (input, "%s:%d", argv[2], port + i);
	    digest = sha1_keygen1 (input, digest, key_size, base);
	    if (keys)
		printf ("%s\n", digest);
	    else
		printf ("%s %d %s\n", argv[2], port + i, digest);

	    /*      // this is there only temporarily
	       unsigned char *binstr = (unsigned char*)malloc(160);
	       unsigned char *base4str = (unsigned char*)malloc(80);

	       hex_to_base2(digest, binstr);
	       printf("converted binstr is: %s \n", binstr);

	       digest1=sha1_keygen1(input, digest1, 160, 2);
	       printf("generated binstr is: %s\n",digest1);

	       hex_to_base4(digest, base4str);
	       printf("converted base4str is: %s \n", base4str);

	       digest2=sha1_keygen1(input, digest2, 80, 4);
	       printf("generated base4str is: %s\n",digest2);

	       char temp[100];
	       char temp1[100];
	       base2_to_hex(binstr, temp);
	       printf("converted hex is: %s \n", temp);
	       base4_to_hex(base4str, temp1);
	       printf("converted hex is: %s \n", temp1);
	     */
	}
}

char *sha1_keygen1 (char *key, char *digest, int size, int base)
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    unsigned char *md_value;
    int md_len, i;
    char digit[10];
    char *tmp;

    md_value = (unsigned char *) malloc (EVP_MAX_MD_SIZE);
    digest = (char *) malloc (size + 1);
    memset (digest, 0, sizeof (digest));

    OpenSSL_add_all_digests ();

    md = EVP_get_digestbyname ("sha1");

    EVP_MD_CTX_init (&mdctx);
    EVP_DigestInit_ex (&mdctx, md, NULL);
    EVP_DigestUpdate (&mdctx, key, strlen (key));
    EVP_DigestFinal_ex (&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup (&mdctx);

    tmp = digest;
    for (i = 0; i < md_len; i++)
	{
	    if (base == BASE_16)
		{
		    convert_base16 (md_value[i], digit);
		}
	    else if (base == BASE_4)
		{
		    convert_base4 (md_value[i], digit);
		}
	    else if (base == BASE_2)
		{
		    convert_base2 (md_value[i], digit);
		}

	    strcat (tmp, digit);
	    tmp = tmp + strlen (digit);
	}
    tmp = '\0';
    //  printf("Digest : %s\n",digest);
    digest[size] = '\0';

    return (digest);
}
