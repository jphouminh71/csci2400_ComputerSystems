/* 
 * CS:APP Data Lab 
 * 
 * <Jonathan Phouminh  106054641>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
  return ~(~x & ~y);    // this is making use of demorgans the law the compliment of x and y is the same as saying or 
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */

/*
    A word is 8 BYTES, which is 32 bits, in this system.
    
    
    other systems could have 16 BYTE words which is 64 bits. 
    Need to create a word. 32 Bits! then make sure all the even positions are set to 1. 
    start with hexadecimal 5 (0101) as this is gives us the 4 starting bits fitting the criteria, need to just 
    continue this trend up to 32 bits. 
        Remember when you shift bit elements (left shift) the tracing positions are all filled with 0. 
    1. make a variable that holds
*/
int evenBits(void) {    /* return a single word with single bits starting with one */
    /*int value = 0x55555555;   0101 0101 0101 0101 0101 0101 0101 0101, 8 hexdecimal 5's,  why doesn't this work, oh wait, exceed 
    size of 0xFF*/ 
    int currentByte = 0x55;  /* this currently gives us 0101 0101, so we need 8 more positions to fill
                          take the current byte (8 bits) and then double the amount of positions by shifting it left */
    currentByte = currentByte | currentByte << 8;  /* giving us 0101 0101 0000 0000 0000 , now we need to fix the even positions 
                                                   using |. The left shift operator gets executed first, think of it as a function                                                    we now should have 0101 0101 0101 0101 0101, 2 BYTES, 
                                                   */ 
    currentByte = currentByte | currentByte << 16;  /* doing the same process here, open up more bits with left shift
                                                        by adding 16 more zeros then doing the same process of using our previous
                                                        value (of its bits) and using the | operator to flip all the even positions
                                                    */
    return currentByte;
    
}
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */

    /* 
        This functions is asking us to return a DECIMAL value of -1. 
        1. since we can only use the bitwise operators we have to essentially work from hexDecimal to Decimal
        2. To go from hexdecimal to decimal the process is adding right to left by increases powers of 2 for the values
        that are not zeros. 
        3. we also have to worry about the negative so we know that this hexdecimal will be represented with sign 
                        ( far left digit tells us the sign of the decimal value, 1 = negative, 0 = positive. )
        Things we know must have to happen. 
            1. The far left digit must be value of 1 and the decimal values must add up to 1. 
            1001: Decimal value for this is 9. to get the decimal value for 1 the only 1 in the binary should be position 0. 
        since the the last signifigant digit is what will represent the sign. It must also be 1. 
        
        the hexDigit for -1 is -1111 by the 2 compliment 
            
    */
int minusOne(void) {
    int bits = 0x0; 
    bits = ~(bits);
    return bits;
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */

/*
    Takes in a parameter, x , need to check for alternating positions starting from 2^0 to the remaing length if they are 1's
    3 has hex value of 0011, 
    
    01010101  this would make the function return  1 , how would you check this? we would need to compare against every bit, at
                                                                                least the even indexes.
    
    Data Lab hints pptx
    1. what is the integer that has all even numbered bits set to 1? 
            5 and F, 
            5, 0101
            F, 1111 
    
    2. How can we represent that large integer with constants in [0,0xff] and bit operations? 
    
    
*/
int allEvenBits(int x) {       /* 
                                    1. construct a mask that will be a reference to all the UNEVEN bits doing this will 
                                    guarentee that if all even bits are set the given word it will be all 1's if we combine 
                                    2. so after constructing the bitstring , compare it agains the given value. 
                                    3. return the reverse of !! to get a single value for 1 or 0. 
                                    
                                */
    int returnValue;
    int mask = 0xaa;
	mask = mask | mask << 8;
	mask = mask | mask << 16;  /* mask construction of all odd bits set to 1. */ 
    returnValue = x | mask;  /* will either be 111111(even bits were set) or something like 101010(even bits were not set) */ 
    returnValue = ~returnValue;  /* 000000(F) or 01010101(T) */ 
    return !returnValue;   /* return the opposite 1 or 0 */ 
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 
 take the same approach to as the last function as it seems like it will be similar. 
 1. create a mask, with the odd positions set to 0, 0xaa -> 1010 1010
 2. extend the mask to cover all possible indexes, 32 bits then mimic across the values for A for the new zeros. 
 now we have a whole 32bit line of all odd indexes set to 1 to compare with the passed parameter x. 
 */
int anyOddBit(int x) {
                         /* . 1. create a mask, with the odd positions set to 0, 0xaa -> 1010 1010
              2. extend the mask to cover all possible indexes, 32 bits then mimic across the values for A for the new zeros. 
              now we have a whole 32bit line of all odd indexes set to 1 to compare with the passed parameter x.    */ 
    int mask; 
    mask = 0xaa;         
    mask = mask | mask << 8;
    mask = mask | mask << 16; 
    
    return !!(x&mask);  /* can only be  0000... or some positive value if there is not an odd bit set similar idea to checking even                         bits */ 
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {   /*tricky part about this problem was having to figure out how to shift n and m to positions*/  
    int ByteN = 0xFF << (n << 3);     /* creating a reference for byte n (1111 1111 0000 0000 0000...)*/
    int ByteM = 0xFF << (m << 3);        /* doing the same for getting the getting a reference for byte m (0000 0000... 11111111)*/
    int mask = ByteN & x;         /* now we have the mask for byte N (1010 1010 00000 ... )*/ 
    int mask1 = ByteM & x;       /* doing the same for byte M */ 
    int combination = ByteN | ByteM;   /* combing the two masks together to use to flip the bits. */ 
    int shift_right_N = mask >> (n << 3);   /* getting the byte information for the byte N */ 
    int shift_right_M = mask1 >> (m << 3);  /* getting the byte information for the byte M*/ 
    int maskShiftN = shift_right_N & 0xFF;   /* copying the byte information at byte N */ 
    int maskShiftM = shift_right_M & 0xFF;   /* copying the information at byte M the FF is there to clear the other bits*/ 
    maskShiftN = maskShiftN << (m << 3);    /* swapping the positions for byte M and byte N */ 
    maskShiftM = maskShiftM << (n << 3);
    combination = ~combination & x;              /* clearing positions M and N and getting the all other orignal bytes set */ 

    return combination | maskShiftN | maskShiftM;  /* finally put all 3 together as positions M and N should all be 0's */ 
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 
 
     overflow is when you have the decimal translation from binary too large to fit in the given bit string size. 
     The convention for detecting an overflow is looking at the sum of the integers and assuming both integers are positive
     if the sum comes out negative then you would return True for overflow. negative numbers return true
         calculating the sum can result in 2 ways. 
             1. you get a positive value meaning you can add the digits without overflow
             2. you get a negative value meaning you did indeed get overflow
                 Either way the sum will give you a number (negative or positive) so you need to figure out a way to 
                manipulate the bits (32) to either make them all zeros or ones THEN use the ! operator to return 
                a single digit one or zero. 
                
                ex_ max of 4 bit is 15 
                
      method of approach was to look at the most signifigant digit of both and determine if they 
      
      ask michael 
      
      We have to look at the sign change if the sign changes then we know that OF has occurred. 
      
      0       <- msd after addition of x and y 
      1       <- where the msd of x is 1
      1       <- where the msd of the msd of y is 1
      
      1       <- XvsSum = 0^1 so it comes out as 1. 
      0 .     <- YvsSum = 0 so this is also 1.    
      
      two possible cases 
      return !(0 & 1)   returns true so that means it can be added 
      return !(1 & 1)  returns 0 which means it cant be added.
      
 */
int addOK(int x, int y) {
    int sum = x + y;   /* need this to determine the MSD of the sum */ 
    int sumMSD = sum >> 31;    /* gives us the MSD for the sum of x and y, this will ultimately tell us if OF is possible*/
    int xMSD = x >> 31; /* gives us the MSD of our x integer */ 
    int yMSD = y >> 31;  /* gives us the MSD of our y integer */ 
    
    int XvsSum = xMSD ^ sumMSD;   /* this will be set to 1 if 'sum' overflows to all 0's */ 
    int YvsX = xMSD ^ yMSD;    /* this will be set to 0 if they are both 1's meaning OF, or if it's 1 it means its ok  */ 
    return !(~YvsX & XvsSum);  /* return the not because if it evaluates to one then that means the sum indeed OF, if it's 0
                                than that means it did not OF */ 
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 
 
    
     First needed to build a 4 bit integer that would represent the integer x, it could either be 0000 or 1111
     then created a variable for z, order didn't matter but essentially flipped the TOF var we made so that we could use it to mask
     integer value z. 
     then created a mask for the integer value y. 
     returned the OR of the two created variables because one of the two masks would have ultimately been 0000 since we used
     a ~operator for one of the masks forcing 1 of the two possibilities. 
     
     
     int x = 0110  int z = 1010 int y = 1101
     !x = 0   so !x ~0 means TOF = 1111                 in the other case we would get 0000 because 1+1111 overflows 
     
     maskZ = ~TOF & z;   0000     <-- either x or y will be 0000 as it depends on the sign of the TOF bit
     maskY = TOF & y;   1101
     
     ex
             now we need to return one of them by taking the negation 
     return 
 */
int conditional(int x, int y, int z) {
   
    int trueOrFalseBit = !x + ~0; /*add negation 0 because it guarentees either 1111 or 0000 */   
    int maskZ = ~trueOrFalseBit & z;
    int maskY = trueOrFalseBit & y;
    return maskY | maskZ;
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
/*
    need to look at the integer sign for the upper and lower bounds so we get the variable integer sign first to use it to get the bounds 
    int upperBound and lowerBound are  setting up the actual bounds for where x should fit. 
    
    have michael explain this to you 
    
*/
    int integerSign = 1 << 31;   /* used to check the overflow */ 
    int upperBound = ~(integerSign | 0x39);   /* sets the upper bound for finding the range */ 
    int lowerBound = ~0x30;    /* sets the lower bound for the ascii digit */ 
    int xMax = upperBound + x;
    int xMin = lowerBound + x + 1;
    upperBound = integerSign & xMax >> 31;   /* this should be 0 if it fits in the upper bound */ 
    lowerBound = integerSign & xMin >> 31;   /* this should also be zero if it fits in the lower bound */ 
    return !(upperBound | lowerBound);  /* if 1 then it will be in the ascii of 0-9, if 0 then it will NOT be in the range */ 
}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
    int y = 0xFF << (n << 3);    /* this line finds the position of the byte we are looking to replace */ 
    int mask = y & x;      /* then we need to create a mask for that byte */ 
    int newBitString = mask ^ x; /* what this line is doing is using the mask we just created of the position to clear that spot!*/
    int moveC = c << (n << 3); /* then we move the byte into position that we just cleared */   
    return newBitString ^ moveC;  /* then using the xor we fill in that position with the given byte and return the new bitstring*/ 
}
/* reverseBits - reverse the bits in a 32-bit integer,
              i.e. b0 swaps with b31, b1 with b30, etc
 *  Examples: reverseBits(0x11111111) = 0x88888888
 *            reverseBits(0xdeadbeef) = 0xf77db57b
 *            reverseBits(0x88888888) = 0x11111111
 *            reverseBits(0)  = 0
 *            reverseBits(-1) = -1
 *            reverseBits(0x9) = 0x90000000
 *  Legal ops: ! ~ & ^ | + << >> and unsigned int type
 *  Max ops: 90
 *  Rating: 4
 
     Recitation Tips. 
     step 1: swap each bits next to eachother 0101 0110 -> 1010 0101 adjacent bits
     shift adjacent 2 bits 
     then swap adjacent 4 bits
     then swap the adjacent 8 bits 
     then swap the adjacent 16 bits 
     
     4 bits = 1 nibble 
 */
int reverseBits(int x) {
    return 2; 
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
    int sum = x + y;
    int a = sum ^ x; /* a is the same as y and b us tbe same as x*/
    int b = sum ^ y;
    int overflow = (a & b) >> 31;
    int c = (sum >> overflow) ^ (overflow << 31);
    return c;
}
/*
 * Extra credit
 */
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
  return 2;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
  return 2;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
  return 2;
}


/* working out problems on your own 

                CLEARING BITS 
value = 1010 1100   < clear byte
mask = 1111 1111  < use this mask to change all the values of the oringal byte to 1.
value = value | mask;    now value will be change to 1111 1111
value = ~value;   now the value will be 0000 0000




                CLEAR AND THEN REPLACING BITS 
ok so what if we were given 0x1234

value = 0001 0010 0011 0100  <-- now we want to change the bit set 1  ( 0011 ) to all ones .
mask =  0000 0000 0000 1111   <- mask = 0xf, now we need to bit shift this to the desired bit 1.
mask = mask << (n<<2)       <-- n = 1 so that means n<<1 == 0100 so this will shift the mask 4 bits over.
mask = 0000 0000 1111 0000   <-- yes now it it is position one, what if we wanted to shift it into position 3?
mask = mask << (n<2);     <--- 0011 becomes 1100 which means that the bits will shift 12 bits over
mask = 1111 0000 0000 0000;    <--- yes so for this 16 bit system shifting 2 over works

back to the question
value = 0001 0010 0011 0100 <-- change the second byte to 0x2, so the method of approach is to maybe clear that bit section first.
replacementMask = 0x2 << (n<<2);   <-- this gives us the replacementMask that we will be using to fill in section 2 once we clear it
clear_mask = 0xf << (n<<2);  <-- puts the clear mask right over 2.

0001 0010 0011 0100 <- value
0000 0000 1111 0000  <- clear mask

clearMask = clearMask & value;  <-- figuring out which bits are set to 1.

0001 0010 0011 0100   <- value
0000 0000 0011 0000  <- clear mask !

value = value ^ clearMask;   <- this should now have cleared the position

0001 0010 0011 0100  <- value
0000 0000 0011 0000  <- clearMask
0001 0010 0000 0100   <- this is now the value given with the position we want cleared to 0000 , now we just have to implement the replacement Mask

0001 0010 0000 0100  <- this is value
0000 0000 0010 0100 <- this is the replacementMask , and but utilizing the | operator should allow us to fill in the blank spot

return (replacementMask | value)

0001 0010 0010 0100   0x1224  from 0x1234    Yes this solution works. the tricky part about this one was just to figure out how to always get the correct shifting position

                    SWAPPING BITS 

*/ 
