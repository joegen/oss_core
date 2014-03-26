#ifndef _CHIMERA_BASE_H_
#define _CHIMERA_BASE_H_

#include <stdio.h>
void convert_base2 (unsigned char num, char *out)
{
    unsigned char mask = 128;
    int i = 0;
    for (i = 0; i < 8; i++)
	{
	    if (num & mask)
		sprintf (out, "1");
	    else
		sprintf (out, "0");
	    mask = mask >> 1;
	    out++;
	}
    *out = '\0';
}

void convert_base4 (unsigned char num, char *out)
{
    unsigned char mask = 3;
    int i = 0;
    for (i = 3; i >= 0; i--)
	{
	    int digit = num >> (i * 2);
	    sprintf (out, "%d", digit & mask);
	    out++;
	}
    *out = '\0';
}

void convert_base16 (unsigned char num, char *out)
{
    unsigned char mask = 15;
    int i = 0;
    for (i = 1; i >= 0; i--)
	{
	    int digit = num >> (i * 4);
	    sprintf (out, "%x", digit & mask);
	    out++;
	}
    *out = '\0';
}

void hex_to_base2 (char *hexstr, char *binstr)
{
    int i = 0;
    memset (binstr, 0, sizeof (binstr));
    for (i = 0; i < strlen (hexstr); i++)
	{
	    switch (hexstr[i])
		{
		case '0':
		    strcat (binstr, "0000");
		    break;
		case '1':
		    strcat (binstr, "0001");
		    break;
		case '2':
		    strcat (binstr, "0010");
		    break;
		case '3':
		    strcat (binstr, "0011");
		    break;
		case '4':
		    strcat (binstr, "0100");
		    break;
		case '5':
		    strcat (binstr, "0101");
		    break;
		case '6':
		    strcat (binstr, "0110");
		    break;
		case '7':
		    strcat (binstr, "0111");
		    break;
		case '8':
		    strcat (binstr, "1000");
		    break;
		case '9':
		    strcat (binstr, "1001");
		    break;
		case 'a':
		    strcat (binstr, "1010");
		    break;
		case 'b':
		    strcat (binstr, "1011");
		    break;
		case 'c':
		    strcat (binstr, "1100");
		    break;
		case 'd':
		    strcat (binstr, "1101");
		    break;
		case 'e':
		    strcat (binstr, "1110");
		    break;
		case 'f':
		    strcat (binstr, "1111");
		    break;
		default:
		    printf ("Some wierd number is in the key \n");
		    break;
		}
	}
}

void hex_to_base4 (char *hexstr, char *base4str)
{
    int i = 0;
    memset (base4str, 0, sizeof (base4str));
    for (i = 0; i < strlen (hexstr); i++)
	{
	    switch (hexstr[i])
		{
		case '0':
		    strcat (base4str, "00");
		    break;
		case '1':
		    strcat (base4str, "01");
		    break;
		case '2':
		    strcat (base4str, "02");
		    break;
		case '3':
		    strcat (base4str, "03");
		    break;
		case '4':
		    strcat (base4str, "10");
		    break;
		case '5':
		    strcat (base4str, "11");
		    break;
		case '6':
		    strcat (base4str, "12");
		    break;
		case '7':
		    strcat (base4str, "13");
		    break;
		case '8':
		    strcat (base4str, "20");
		    break;
		case '9':
		    strcat (base4str, "21");
		    break;
		case 'a':
		    strcat (base4str, "22");
		    break;
		case 'b':
		    strcat (base4str, "23");
		    break;
		case 'c':
		    strcat (base4str, "30");
		    break;
		case 'd':
		    strcat (base4str, "31");
		    break;
		case 'e':
		    strcat (base4str, "32");
		    break;
		case 'f':
		    strcat (base4str, "33");
		    break;
		default:
		    printf ("Some wierd number is in the key \n");
		    break;
		}
	}
}

// assuming that the input string is only 4 bytes long, convert this number to a hex digit
char *get_hex_digit_from_bin (char *binstr)
{
    char *ret = (char *) malloc (3);
    int num = 0;
    num =
	(binstr[0] - 48) * 8 + (binstr[1] - 48) * 4 + (binstr[2] - 48) * 2 +
	(binstr[3] - 48) * 1;
    sprintf (ret, "%01x", num);
    return ret;
}

// assuming that the input string is only 2 bytes long, convert this number to a hex digit
char *get_hex_digit_from_base4 (char *base4str)
{
    char *ret = (char *) malloc (3);
    int num = 0;
    num = (base4str[0] - 48) * 4 + (base4str[1] - 48) * 1;
    sprintf (ret, "%01x", num);
    return ret;
}

void base2_to_hex (char *binstr, char *hexstr)
{
    int i = 0;
    int j = 0;
    memset (hexstr, 0, sizeof (hexstr));
    for (i = 0; i < strlen (binstr); i += 4, j++)
	{
	    char *temp = get_hex_digit_from_bin (binstr + i);
	    strcat (hexstr, temp);
	    free (temp);
	}
    hexstr[j] = '\0';
}

void base4_to_hex (char *base4str, char *hexstr)
{
    // assume that the hexstr is properly initialized
//      memset(hexstr, 0, sizeof(hexstr));
    int i = 0;
    int j = 0;
    for (i = 0; i < strlen (base4str); i += 2, j++)
	{
	    char *temp = get_hex_digit_from_base4 (base4str + i);
	    strcat (hexstr, temp);
	    free (temp);
	}
    hexstr[j] = '\0';
}
#endif /* _CHIMERA_BASE_H_ */
