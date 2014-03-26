
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include "key.h"
#include "base.h"
#include "log.h"

void key_print (Key k)
{
    int i;
    char hexstr[KEY_SIZE];	// this is big just to be safe
    char base4str[KEY_SIZE];	// 

    for (i = 4; i >= 0; i--)
	sprintf (hexstr, "%08x", (unsigned int) k.t[i]);

    if (IS_BASE_16)
	{
	    for (i = 0; i < strlen (hexstr); i++)
		{
		    if (i % 8 == 0)
			printf (" ");
		    printf ("%c", hexstr[i]);
		}
	}
    else if (IS_BASE_4)
	{
	    hex_to_base4 (hexstr, base4str);

	    for (i = 0; i < strlen (base4str); i++)
		{
		    if (i % 16 == 0)
			printf (" ");
		    printf ("%c", base4str[i]);
		}
	}
    else
	{
	    printf ("key.c: Unknown base \n");
	}
    printf ("\n");
}

void key_to_str (Key * k)
{
    char temp[KEY_SIZE];

    k->valid = 1;

    if (IS_BASE_16)
	{
	    memset (k->keystr, 0, KEY_SIZE / BASE_B + 1);
	    sprintf (k->keystr, "%08x%08x%08x%08x%08x",
		     (unsigned int) k->t[4], (unsigned int) k->t[3],
		     (unsigned int) k->t[2], (unsigned int) k->t[1],
		     (unsigned int) k->t[0]);
	}
    else
	// if we need base4, then convert base16 to base4
    if (IS_BASE_4)
	{
	    sprintf (temp, "%08x%08x%08x%08x%08x", (unsigned int) k->t[4],
		     (unsigned int) k->t[3], (unsigned int) k->t[2],
		     (unsigned int) k->t[1], (unsigned int) k->t[0]);

	    hex_to_base4 (temp, k->keystr);
	}
    else
	{
	    printf ("key.c: Unknown base \n");
	}
}

void str_to_key (char *strOrig, Key * k)
{
    int i, len;
    char key_str[KEY_SIZE / BASE_B + 1];

    char str[KEY_SIZE / BASE_B + 1];
    char tempString[KEY_SIZE];


    // This loop below is required, though Patrik L. from sparta recommended against it
    for (i = 0; i < KEY_SIZE / BASE_B + 1; i++)
	key_str[i] = '0';
    memset (str, 0, KEY_SIZE / BASE_B + 1);
    if (strlen (strOrig) < KEY_SIZE / BASE_B)
	{
	    strcpy (str, strOrig);
	}
    else
	{
	    strncpy (str, strOrig, KEY_SIZE / BASE_B);
	    str[KEY_SIZE / BASE_B] = '\0';
	}

    // Now, if str is in a different base than hex, replace the str contents with corresponding hex contents
    if (IS_BASE_4)
	{
	    strcpy (tempString, str);
	    memset (str, 0, strlen (tempString));
	    base4_to_hex (tempString, str);
	}

    // By now, the string should be in base 16
    len = strlen (str);
    if (len == 0)
	{
	    fprintf (stderr, "str_to_key: Warning:Empty input string\n");
	}
    else if (len > BASE_16_KEYLENGTH)
	{
	    strncpy (key_str, str, BASE_16_KEYLENGTH);
	    //  key_str[KEY_SIZE/BASE_B]='\0';
	}

    else if (len <= BASE_16_KEYLENGTH)
	{
	    for (i = 0; i < len; i++)
		key_str[i + (BASE_16_KEYLENGTH) - len] = str[i];
	}

    key_str[BASE_16_KEYLENGTH] = '\0';

    for (i = 0; i < 5; i++)
	sscanf (key_str + (i * 8 * sizeof (char)), "%08x", &(k->t[(4 - i)]));

    key_to_str (k);
}

void key_assign (Key * k1, Key k2)
{
    int i;
    for (i = 0; i < 5; i++)
	k1->t[i] = k2.t[i];

    key_to_str (k1);
}

void key_assign_ui (Key * k, unsigned long ul)
{
    int i;
    for (i = 1; i < 5; i++)
	k->t[i] = 0;
    k->t[0] = ul;
    key_to_str (k);
}

int key_equal (Key k1, Key k2)
{
    int i;
    for (i = 0; i < 5; i++)
	if (k1.t[i] != k2.t[i])
	    return (0);
    return (1);
}

int key_equal_ui (Key k, unsigned long ul)
{
    int i;
    if (k.t[0] != ul)
	return (0);
    for (i = 1; i < 5; i++)
	if (k.t[i] != 0)
	    return (0);
    return (1);
}

int key_comp (const Key * const k1, const Key * const k2)
{

    int i;

    for (i = 4; i >= 0; i--)
	{
	    if (k1->t[i] > k2->t[i])
		return (1);
	    else if (k1->t[i] < k2->t[i])
		return (-1);
	}
    return (0);
}

void key_add (Key * result, const Key * const op1, const Key * const op2)
{

    double tmp, a, b;
    int i;
    a = b = tmp = 0;

    for (i = 0; i < 5; i++)
	{
	    a = op1->t[i];
	    b = op2->t[i];
	    tmp += a + b;

	    if (tmp > ULONG_MAX)
		{
		    result->t[i] = (unsigned long) tmp;
		    tmp = 1;
		}
	    else
		{
		    result->t[i] = (unsigned long) tmp;
		    tmp = 0;
		}
	}
    result->valid = 0;
}

void key_sub (void *logs, Key * result, const Key * const op1,
	      const Key * const op2)
{
    int i;
    double tmp, a, b, carry;

    carry = 0;

    if (key_comp (op1, op2) < 0)
	{
	    if (LOGS)
	      log_message (logs, LOG_ERROR,
			   "key_sub: Operation is not allowed %s < %s \n",
			   op1->keystr, op2->keystr);
	    return;
	}

    for (i = 0; i < 5; i++)
	{
	    a = op1->t[i] - carry;
	    b = op2->t[i];

	    if (b <= a)
		{
		    tmp = a - b;
		    carry = 0;
		}
	    else
		{
		    a = a + ULONG_MAX + 1;
		    tmp = a - b;
		    carry = 1;
		}
	    result->t[i] = (unsigned long) tmp;
	}

    result->valid = 0;
}

char *sha1_keygen (char *key, size_t digest_size, char *digest)
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    unsigned char *md_value;
    int md_len, i;
    char digit[10];
    char *tmp;

    md_value = (unsigned char *) malloc (EVP_MAX_MD_SIZE);

    OpenSSL_add_all_digests ();

    md = EVP_get_digestbyname ("sha1");

    EVP_MD_CTX_init (&mdctx);
    EVP_DigestInit_ex (&mdctx, md, NULL);
    EVP_DigestUpdate (&mdctx, key, digest_size);
    EVP_DigestFinal_ex (&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup (&mdctx);

    digest = (char *) malloc (KEY_SIZE / BASE_B + 1);

    tmp = digest;
    *tmp = '\0';
    for (i = 0; i < md_len; i++)
	{
	    if (power (2, BASE_B) == BASE_16)
		{
		    convert_base16 (md_value[i], digit);
		}
	    else if (power (2, BASE_B) == BASE_4)
		{
		    convert_base4 (md_value[i], digit);
		}
	    else if (power (2, BASE_B) == BASE_2)
		{
		    convert_base2 (md_value[i], digit);
		}

	    strcat (tmp, digit);
	    tmp = tmp + strlen (digit);
	}

    free (md_value);

    tmp = '\0';
    return (digest);
}

void key_makehash (void *logs, Key * hashed, char *s)
{

    key_make_hash (hashed, s, strlen (s) * sizeof (char));

    if (logs)
	{
	    if (LOGS)
	      log_message (logs, LOG_KEYDEBUG,
			   "key_makehash: HASH( %s ) = [%s]\n", s,
			   get_key_string (hashed));
	}
}

void key_make_hash (Key * hashed, char *s, size_t size)
{
    char *digest;
    int i;

    digest = sha1_keygen (s, size, NULL);
    str_to_key (digest, hashed);

    //for(i=0; i <5; i++) sscanf(digest+(i*8*sizeof(char)),"%08x",&hashed->t[(4-i)]);
    //key_to_str(hashed->keystr,*hashed);

    free (digest);
}

void key_init ()
{
    int i;
    for (i = 0; i < 5; i++)
	{
	    Key_Max.t[i] = ULONG_MAX;
	    Key_Half.t[i] = ULONG_MAX;
	}
    Key_Half.t[4] = Key_Half.t[4] / 2;

    key_to_str (&Key_Max);
    key_to_str (&Key_Half);

}

void key_distance (void *logs, Key * diff, const Key * const k1,
		   const Key * const k2)
{

    int comp;

    comp = key_comp (k1, k2);

    /* k1 > k2 */
    if (comp > 0)
	key_sub (logs, diff, k1, k2);
    else
	key_sub (logs, diff, k2, k1);

    comp = key_comp (diff, &Key_Half);

    /* diff > Key_Half */
    if (comp > 0)
	key_sub (logs, diff, &Key_Max, diff);


    diff->valid = 0;
    //log_message(logs, LOG_KEYDEBUG, "key_distance: %s  and %s is %s\n", get_key_string(k1), get_key_string(k2), get_key_string(diff));
}


int key_between (void *logs, const Key * const test, const Key * const left,
		 const Key * const right)
{

    int complr = key_comp (left, right);
    int complt = key_comp (left, test);
    int comptr = key_comp (test, right);

    /* it's on one of the edges */
    if (complt == 0 || comptr == 0)
	return (1);


    if (complr < 0)
	{
	    if (complt < 0 && comptr < 0)
		return (1);
	    return (0);
	}
    else if (complr == 0)
	{
	    return (0);
	}
    else
	{
	    if (complt < 0 || comptr < 0)
		return (1);
	    return (0);

	}
}

// Return the string representation of key
// This function should be used instead of directly accessing the keystr field
char *get_key_string (Key * key)
{
    if (!key->valid)
	{
	    key_to_str (key);
	}
    return key->keystr;
}

void key_midpoint (void *logs, Key * mid, Key key)
{

    if (key_comp (&key, &Key_Half) < 0)
	key_add (mid, &key, &Key_Half);
    else
	key_sub (logs, mid, &key, &Key_Half);

    mid->valid = 0;
}

int key_index (void *logs, Key mykey, Key k)
{

    int max_len, i;
    char mystr[KEY_SIZE / BASE_B + 1];
    char kstr[KEY_SIZE / BASE_B + 1];
    max_len = KEY_SIZE / BASE_B;
    strcpy (mystr, get_key_string (&mykey));
    strcpy (kstr, get_key_string (&k));

    for (i = 0; (mystr[i] == kstr[i]) && (i < max_len); i++);
    if (i == max_len)
	i = max_len - 1;

    if (LOGS)
      log_message (logs, LOG_KEYDEBUG,
		   "key_index:%d\n me:%s\n lookup_key:%s \n", i, mykey.keystr,
		   k.keystr);

    return (i);
}
