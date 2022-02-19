#include "types.h"

static char *reverse(char *buffer, size_t len)
{
    char tmp, *src, *dst;
    if (buffer != NULL)
    {
        if (len > 1) {
            src = buffer;
            dst = src + len - 1;
            while (src < dst) {
                tmp = *src;
                *src++ = *dst;
                *dst-- = tmp;
            }
        }
    }
    return buffer;
}

// Implementation of itoa() 
char* itoa(int num, char *buffer, int base) 
{
    int i = 0; 
    bool isNegative = false; 
  
    /* Handle 0 explicitely, otherwise empty buffering is printed for 0 */
    if (num == 0) 
    { 
        buffer[i++] = '0'; 
        buffer[i] = '\0';
        return buffer; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) 
    { 
        isNegative = true; 
        num = -num; 
    } 
  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % base; 
        buffer[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        buffer[i++] = '-'; 
  
    buffer[i] = '\0'; // Append buffering terminator 
  
    // Reverse the buffering 
    reverse(buffer, i); 
  
    return buffer; 
} 