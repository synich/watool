#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "walib.h"

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
          { \
          a += F(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GG(a,b,c,d,x,s,ac) \
          { \
          a += G(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define HH(a,b,c,d,x,s,ac) \
          { \
          a += H(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define II(a,b,c,d,x,s,ac) \
          { \
          a += I(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }

unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

static void MD5Transform(unsigned int state[4],unsigned char block[64]);

static void MD5Init(MD5_CTX *context)
{
     context->count[0] = 0;
     context->count[1] = 0;
     context->state[0] = 0x67452301;
     context->state[1] = 0xEFCDAB89;
     context->state[2] = 0x98BADCFE;
     context->state[3] = 0x10325476;
}
static void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
       context->count[1]++;
    context->count[1] += inputlen >> 29;

    if(inputlen >= partlen)
    {
       memcpy(&context->buffer[index],input,partlen);
       MD5Transform(context->state,context->buffer);
       for(i = partlen;i+64 <= inputlen;i+=64)
           MD5Transform(context->state,&input[i]);
       index = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}
static void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
         output[j] = input[i] & 0xFF;
         output[j+1] = (input[i] >> 8) & 0xFF;
         output[j+2] = (input[i] >> 16) & 0xFF;
         output[j+3] = (input[i] >> 24) & 0xFF;
         i++;
         j+=4;
    }
}
static void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
     unsigned int i = 0,j = 0;
     while(j < len)
     {
           output[i] = (input[j]) |
                       (input[j+1] << 8) |
                       (input[j+2] << 16) |
                       (input[j+3] << 24);
           i++;
           j+=4;
     }
}
static void MD5Final(MD5_CTX *context,unsigned char digest[16])
{
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits,context->count,8);
    MD5Update(context,PADDING,padlen);
    MD5Update(context,bits,8);
    MD5Encode(digest,context->state,16);
}
static void MD5Transform(unsigned int state[4],unsigned char block[64])
{
     unsigned int a = state[0];
     unsigned int b = state[1];
     unsigned int c = state[2];
     unsigned int d = state[3];
     unsigned int x[64];
     MD5Decode(x,block,64);
     FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
 FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
 FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
 FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
 FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
 FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
 FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
 FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
 FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
 FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
 FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
 FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
 FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
 FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
 FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
 FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

 /* Round 2 */
 GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
 GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
 GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
 GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
 GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
 GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
 GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
 GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
 GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
 GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
 GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
 GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
 GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
 GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
 GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
 GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

 /* Round 3 */
 HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
 HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
 HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
 HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
 HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
 HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
 HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
 HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
 HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
 HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
 HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
 HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
 HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
 HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
 HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
 HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */

 /* Round 4 */
 II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
 II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
 II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
 II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
 II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
 II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
 II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
 II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
 II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
 II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
 II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
 II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
 II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
 II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
 II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
 II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
     state[0] += a;
     state[1] += b;
     state[2] += c;
     state[3] += d;
}

void wa_md5(char* src, char* dst){
	int i;
	unsigned char decrypt[16];
	MD5_CTX md5;
    dst[0] = 0;
    if (!src) return;

    MD5Init(&md5);
	MD5Update(&md5,(unsigned char*)src,strlen((char *)src));
	MD5Final(&md5,decrypt);
	for(i=0;i<16;i++)
	{
      char tmp[4]={0};
      sprintf(tmp, "%02x", decrypt[i]);
      strcat(dst, tmp);
	}
}



/*
* If you do not have the ISO standard stdint.h header file, then you
* must typdef the following:
* name meaning
* uint32_t unsigned 32 bit integer
* uint8_t unsigned 8 bit integer (i.e., unsigned char)
* int_least16_t integer of >= 16 bits
*
*/
#ifndef _SHA_enum_
#define _SHA_enum_
enum
{
	shaSuccess = 0,
	shaNull,         /* Null pointer parameter */
	shaInputTooLong, /* input data too long */
	shaStateError    /* called Input after Result */
};
#endif
#define SHA1HashSize 20
/*
* This structure will hold context information for the SHA-1
* hashing operation
*/
typedef struct SHA1Context
{
	uint32_t Intermediate_Hash[SHA1HashSize/4]; /* Message Digest */
	uint32_t Length_Low; /* Message length in bits */
	uint32_t Length_High; /* Message length in bits */
	/* Index into message block array */
	int_least16_t Message_Block_Index;
	uint8_t Message_Block[64]; /* 512-bit message blocks */
	int Computed; /* Is the digest computed? */
	int Corrupted; /* Is the message digest corrupted? */
} SHA1Context;


/*
* Function Prototypes
*/

/*
* sha1.c
*
* Description:
* This file implements the Secure Hashing Algorithm 1 as
* defined in FIPS PUB 180-1 published April 17, 1995.
*
* The SHA-1, produces a 160-bit message digest for a given
* data stream. It should take about 2**n steps to find a
* message with the same digest as a given message and
* 2**(n/2) to find any two messages with the same digest,
* when n is the digest size in bits. Therefore, this
* algorithm can serve as a means of providing a
* "fingerprint" for a message.
*
* Portability Issues:
* SHA-1 is defined in terms of 32-bit "words". This code
* uses <stdint.h> (included via "sha1.h" to define 32 and 8
* bit unsigned integer types. If your C compiler does not
* support 32 bit unsigned integers, this code is not
* appropriate.
*
* Caveats:
* SHA-1 is designed to work with messages less than 2^64 bits
* long. Although SHA-1 allows a message digest to be generated
* for messages of any number of bits less than 2^64, this
* implementation only works with messages with a length that is
* a multiple of the size of an 8-bit character.
*
*/

/*
* Define the SHA1 circular left shift macro
*/
#define SHA1CircularShift(bits,word) \
	(((word) << (bits)) | ((word) >> (32-(bits))))
/* Local Function Prototyptes */
static void SHA1PadMessage(SHA1Context *);
static void SHA1ProcessMessageBlock(SHA1Context *);
/*
* SHA1Reset
*
* Description:
* This function will initialize the SHA1Context in preparation
* for computing a new SHA1 message digest.
*
* Parameters:
* context: [in/out]
* The context to reset.
*
* Returns:
* sha Error Code.
*
*/
static int SHA1Reset(SHA1Context *context)
{
	if (!context)
	{
		return shaNull;
	}
	context->Length_Low = 0;
	context->Length_High = 0;
	context->Message_Block_Index = 0;
	context->Intermediate_Hash[0] = 0x67452301;//HASH(intermedia data)
	context->Intermediate_Hash[1] = 0xEFCDAB89;
	context->Intermediate_Hash[2] = 0x98BADCFE;
	context->Intermediate_Hash[3] = 0x10325476;
	context->Intermediate_Hash[4] = 0xC3D2E1F0;
	context->Computed = 0;
	context->Corrupted = 0;
	return shaSuccess;
}


/*
* SHA1Result
*
* Description:
* This function will return the 160-bit message digest into the
* Message_Digest array provided by the caller.
* NOTE: The first octet of hash is stored in the 0th element,
* the last octet of hash in the 19th element.
*
* Parameters:
* context: [in/out]
* The context to use to calculate the SHA-1 hash.
* Message_Digest: [out]
* Where the digest is returned.
*
* Returns:
* sha Error Code.
*
*/
static int SHA1Result( SHA1Context *context,uint8_t Message_Digest[SHA1HashSize])
{
	int i;
	if (!context || !Message_Digest)
	{
		return shaNull;
	}
	if (context->Corrupted)
	{
		return context->Corrupted;
	}
	if (!context->Computed)
	{
		SHA1PadMessage(context);
		for(i=0; i<64; ++i)
		{
			/* message may be sensitive, clear it out */
			context->Message_Block[i] = 0;
		}
		context->Length_Low = 0; /* and clear length */
		context->Length_High = 0;
		context->Computed = 1;
	}
	for(i = 0; i < SHA1HashSize; ++i)
	{
		Message_Digest[i] = context->Intermediate_Hash[i>>2]
		>> 8 * ( 3 - ( i & 0x03 ) );
	}
	return shaSuccess;
}


/*
* SHA1Input
*
* Description:
* This function accepts an array of octets as the next portion
* of the message.
*
* Parameters:
* context: [in/out]
* The SHA context to update
* message_array: [in]
* An array of characters representing the next portion of
* the message.
* length: [in]
* The length of the message in message_array
*
* Returns:
* sha Error Code.
*
*/

static int SHA1Input( SHA1Context *context,const uint8_t *message_array,unsigned length)
{
	if (!length)
	{
		return shaSuccess;
	}
	if (!context || !message_array)
	{
		return shaNull;
	}
	if (context->Computed)
	{
		context->Corrupted = shaStateError;
		return shaStateError;
	}
	if (context->Corrupted)
	{
		return context->Corrupted;
	}
	while(length-- && !context->Corrupted)
	{
		context->Message_Block[context->Message_Block_Index++] =
			(*message_array & 0xFF);
		context->Length_Low += 8;
		if (context->Length_Low == 0)
		{
			context->Length_High++;
			if (context->Length_High == 0)
			{
				/* Message is too long */
				context->Corrupted = 1;
			}
		}
		if (context->Message_Block_Index == 64)
		{
			SHA1ProcessMessageBlock(context);
		}
		message_array++;
	}
	return shaSuccess;
}

/*
* SHA1ProcessMessageBlock
*
* Description:
* This function will process the next 512 bits of the message
* stored in the Message_Block array.
*
* Parameters:
* None.
*
* Returns:
* Nothing.
*
* Comments:
* Many of the variable names in this code, especially the
* single character names, were used because those were the
* names used in the publication.
*
*/

static void SHA1ProcessMessageBlock(SHA1Context *context)
{
	const uint32_t K[] = { /* Constants defined in SHA-1 */
		0x5A827999,
		0x6ED9EBA1,
		0x8F1BBCDC,
		0xCA62C1D6
	};
	int t; /* Loop counter */
	uint32_t temp; /* Temporary word value */
	uint32_t W[80]; /* Word sequence */
	uint32_t A, B, C, D, E; /* Word buffers */
	/*
	* Initialize the first 16 words in the array W
	*/
	for(t = 0; t < 16; t++)
	{
		W[t] = context->Message_Block[t * 4] << 24;
		W[t] |= context->Message_Block[t * 4 + 1] << 16;
		W[t] |= context->Message_Block[t * 4 + 2] << 8;
		W[t] |= context->Message_Block[t * 4 + 3];
	}
	for(t = 16; t < 80; t++)
	{
		W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
	}
	A = context->Intermediate_Hash[0];
	B = context->Intermediate_Hash[1];
	C = context->Intermediate_Hash[2];
	D = context->Intermediate_Hash[3];
	E = context->Intermediate_Hash[4];
	for(t = 0; t < 20; t++)
	{
                temp = SHA1CircularShift(5,A) +
                        ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		E = D;
		D = C;
		C = SHA1CircularShift(30,B);
		B = A;
		A = temp;
	}
	for(t = 20; t < 40; t++)
	{
		temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
		E = D;
		D = C;
		C = SHA1CircularShift(30,B);
		B = A;
		A = temp;
	}
	for(t = 40; t < 60; t++)
	{
		temp = SHA1CircularShift(5,A) +
			((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
		E = D;
		D = C;
		C = SHA1CircularShift(30,B);
		B = A;
		A = temp;
	}
	for(t = 60; t < 80; t++)
	{
		temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
		E = D;
		D = C;
		C = SHA1CircularShift(30,B);
		B = A;
		A = temp;
	}
	context->Intermediate_Hash[0] += A;
	context->Intermediate_Hash[1] += B;
	context->Intermediate_Hash[2] += C;
	context->Intermediate_Hash[3] += D;
	context->Intermediate_Hash[4] += E;
	context->Message_Block_Index = 0;
}


/*
* SHA1PadMessage
*
* Description:
* According to the standard, the message must be padded to an even
* 512 bits. The first padding bit must be a '1'. The last 64
* bits represent the length of the original message. All bits in
* between should be 0. This function will pad the message
* according to those rules by filling the Message_Block array
* accordingly. It will also call the ProcessMessageBlock function
* provided appropriately. When it returns, it can be assumed that
* the message digest has been computed.
*
* Parameters:
* context: [in/out]
* The context to pad
* ProcessMessageBlock: [in]
* The appropriate SHA*ProcessMessageBlock function
* Returns:
* Nothing.
*
*/

static void SHA1PadMessage(SHA1Context *context)
{
	/*
	* Check to see if the current message block is too small to hold
	* the initial padding bits and length. If so, we will pad the
	* block, process it, and then continue padding into a second
	* block.
	*/
	if (context->Message_Block_Index > 55)
	{
		context->Message_Block[context->Message_Block_Index++] = 0x80;
		while(context->Message_Block_Index < 64)
		{
			context->Message_Block[context->Message_Block_Index++] = 0;
		}
		SHA1ProcessMessageBlock(context);
		while(context->Message_Block_Index < 56)
		{
			context->Message_Block[context->Message_Block_Index++] = 0;
		}
	}
	else
	{
		context->Message_Block[context->Message_Block_Index++] = 0x80;
		while(context->Message_Block_Index < 56)
		{
			context->Message_Block[context->Message_Block_Index++] = 0;
		}
	}

	/*
	* Store the message length as the last 8 octets
	*/
	context->Message_Block[56] = context->Length_High >> 24;
	context->Message_Block[57] = context->Length_High >> 16;
	context->Message_Block[58] = context->Length_High >> 8;
	context->Message_Block[59] = context->Length_High;
	context->Message_Block[60] = context->Length_Low >> 24;
	context->Message_Block[61] = context->Length_Low >> 16;
	context->Message_Block[62] = context->Length_Low >> 8;
	context->Message_Block[63] = context->Length_Low;
	SHA1ProcessMessageBlock(context);
}


void wa_sha1(char* src, char* dst) {
	SHA1Context sha;
	int i, err=0;
	uint8_t Message_Digest[20];
    dst[0] = 0;
    if (!src) return;
	err |= SHA1Reset(&sha);
	err |= SHA1Input(&sha,
			(const unsigned char *) src,
			strlen(src));
    err |= SHA1Result(&sha, Message_Digest);
    if (0==err) {
      for(i = 0; i < 20 ; ++i)
      {
        char tmpbuf[4]={0};
		snprintf(tmpbuf, sizeof(tmpbuf)-1, "%02x", Message_Digest[i]);
        strcat(dst, tmpbuf);
	  }
    }
    return ;
}


// global definition
static const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char padding_char = '=';

/*编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int wa_base64enc(char * sourcedata, char * base64)
{
    int i=0, j=0;
    unsigned char trans_index=0;    // 索引是8位，但是高两位都为0
    const int datalength = strlen((const char*)sourcedata);
    for (; i < datalength; i += 3){
        // 每三个一组，进行编码
        // 要编码的数字的第一个
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // 第二个
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength){
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   // 超出总长度，可以直接break
        }
        // 第三个
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength){ // 有的话需要编码2个
            trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
            base64[j++] = base64char[(int)trans_index];

            trans_index = sourcedata[i + 2] & 0x3f;
            base64[j++] = base64char[(int)trans_index];
        }
        else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            break;
        }
    }

    base64[j] = '\0'; 

    return 0;
}


/** 在字符串中查询特定字符位置索引
* const char *str ，字符串
* char c，要查找的字符
*/
static inline int num_strchr(const char *str, char c) // 
{
    const char *pindex = strchr(str, c);
    if (NULL == pindex){
        return -1;
    }
    return pindex - str;
}
/* 解码
* const char * base64 码字
* unsigned char * dedata， 解码恢复的数据
*/
int wa_base64dec(char * base64, char * dedata)
{
    int i = 0, j=0;
    int trans[4] = {0,0,0,0};
    for (;base64[i]!='\0';i+=4){
        // 每四个一组，译码成三个字符
        trans[0] = num_strchr(base64char, base64[i]);
        trans[1] = num_strchr(base64char, base64[i+1]);
        // 1/3
        dedata[j++] = ((trans[0] << 2) & 0xfc) | ((trans[1]>>4) & 0x03);

        if (base64[i+2] == '='){
            continue;
        }
        else{
            trans[2] = num_strchr(base64char, base64[i + 2]);
        }
        // 2/3
        dedata[j++] = ((trans[1] << 4) & 0xf0) | ((trans[2] >> 2) & 0x0f);

        if (base64[i + 3] == '='){
            continue;
        }
        else{
            trans[3] = num_strchr(base64char, base64[i + 3]);
        }

        // 3/3
        dedata[j++] = ((trans[2] << 6) & 0xc0) | (trans[3] & 0x3f);
    }

    dedata[j] = '\0';

    return 0;
}

static unsigned char chtonum(char c) {
  unsigned char r=0;
  if (('0'<=c) && (c<='9') ) {
    r = c-'0';
  } else if (('A'<=c) && (c<='F') ) {
    r = c- 'A'+10;
  } else if (('a'<=c) &&(c<='f') ) {
    r = c- 'a'+10;
  }
  return r;
}

int wa_base16dec(char* src, char* dst){
  int l = strlen(src) / 2;
  for (int i=0; i<l; i++ ){
    unsigned char j;
    j = (chtonum(str[2*i])<<4 ) + chtonum(str[2*i+1]);
    dst[i] = (char*)j;
  }
  return 0;
}

int wa_base16enc() {
  return 0;
}
