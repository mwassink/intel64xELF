#include <stdio.h>
#include "../include/modrm.h"
#include "../include/char_to_instructions.h"
#include "../include/get_instructions.h"
#include "../include/get_text_tokens.h"
#include <string.h>
#include <assert.h>

// NOTE: The file should be preprocessed before to standardize the inpu t registers, too bad fileio is slow

int ascii_to_int_hex(char * input)
{
  int returned = 0;
  int i = 0;
  while (input)
    {
      returned |= (input[i] - 48);
      returned <<= 4;
    }

  return returned;
}


struct __instruction encode_registers_rm( char * op1,  char * op2, bool op1_rm)
{
      // The second operand is encoded with the registers
      // The first operand is encoded with the rm
  fprintf(stderr, "Function is not complete yet");

}



bool reg_string_compare (char  * in, char  * in2) // before, just cast the address to one of type int, compiler will know what to do 
{
  bool equal = 0;
   asm ("movl (%1), %%r13d \n"
	"movl (%2), %%r14d \n"
         "cmp  %%r13d, %%r14d \n"
         "jne  done \n"
	"movl 1, %0 \n"
         "done: movl %0, %%eax;"
	: "=r" (equal)
	: "r" (in), "c" (in2)
	:
        );
   return equal;
}


void reg_table_register_new(struct temprm * temp, char * reg, bool is_first)
{
  // This is for the new reigsters 
  int returned = atoi(reg+1);
  if (is_first)
    temp->first = returned - 8;

  else
    temp->second = returned - 8;

  
  
}


int ret_mod(char * input)
{
  if (input[0] == '0' && input[1] == 'x' ) // This will be some sort of offset
    {
      if ((input[3] == 'r' || input[3] == 'e') && input[4] == 'i' && input[5] == 'p')
	return 0;

      int length_disp = 0;
      while(input[length_disp++] != '(');

      if (--length_disp <= 4) //0xXX disp 8
	{
	  return 1;
	}
      return 2;
    }

  if (*input == '(')
    {
      return 0;
    }

  // We have already decided that there is no offset and there is no immediate addressing, must be a register
  return 3;
  
}

inline bool needs_rex_r(char * reg)
{
  return (reg[1] > 47 && reg[1] < 58);
}


void reg_table_register_extension(struct temprm * temp, char * reg, bool is_first, bool rex_r)
{
  // With extensions there are usually eight registers all ending with their number
  // So we can easily get the right number by subtracting


  int sum = 0;
  while (*reg++ && sum++);

  if (is_first && !rex_r)
    temp->first = atoi(reg+sum);

  else if (is_first && rex_r)
    temp->first = atoi(reg+sum) - 8;

  else if (!is_first && !rex_r)
    temp->second = atoi(reg+sum);

  
  else
    temp->second = atoi(reg+sum) - 8;
}


void reg_table_byte_no_rex(struct temprm * temp, char * reg, bool is_first)
{
  if (reg[0] == 'a')
    {
      if (reg[1] == 'l')
	{
	  if (is_first)
	    temp->first = 0;
	  else
	    temp->second = 0;
	}
      else 
	{
	  if (is_first)
	    temp->first = 4;
	  else
	    temp->second = 4;
	}
    }

  if (reg[0] == 'c')
    {
      if (reg[1] == 'l')
	{
	  if (is_first)
	    temp->first = 1;
	  else
	    temp->second = 1;
	}
      else
	{
	  if (is_first)
	    temp->first = 5;
	  else
	    temp->second = 5;
	}
    }

  if (reg[0] == 'd')
    {
      if (reg[1] == 'l')
	{
	  if (is_first)
	    temp->first = 2;
	  else
	    temp->second = 2;
	}
      else
	{
	  if (is_first)
	    temp->first = 6;
	  else
	    temp->second = 6;
	}
    }

  else
    {
      if (reg[1] == 'l')
	{
	  if (is_first)
	    temp->first = 3;
	  else
	    temp->second = 3;
	}
      else
	{
	  if (is_first)
	    temp->first = 7;
	  else
	    temp->second = 7;
	  
	}
    }
}

void reg_table_byte_rex(struct temprm * temp, char * reg, bool is_first)
{
  int length = 0;
  while (*reg++ && length++);

  if (length == 3)
    {
      if (reg[0] == 's' && reg[2] == 'l')
	{
	  if (is_first)
	    temp->first = 4;
	  else
	    temp->second = 4;
	}
      if (reg[0] == 'b'  && reg[2] == 'l')
	{
	  if (is_first)
	    temp->first = 5;
	  else
	    temp->second = 5;
	}

      if (reg[0] == 's'  && reg[2] == 'l')
	{
	  if (is_first)
	    temp->first = 6;
	  else
	    temp->second = 6;
	}

      if (reg[0] == 'd'  && reg[2] == 'l')
	{
	  if (is_first)
	    temp->first = 7;
	  else
	    temp->second = 7;
	}
      
    }

  else
    {
      if (reg[0] == 'a')
	{
	  if (is_first)
	    temp->first = 0;
	  else
	    temp->second = 0;
	}
      if (reg[0] == 'c')
	{
	  if (is_first)
	    temp->first = 1;
	  else
	    temp->second  = 1;
	}
      if (reg[0] == 'd')
	{
	  if (is_first)
	    temp->first = 2;
	  else
	    temp->second = 2;	  
	}
      if (reg[0] == 'b')
	{
	  if (is_first)
	    temp->first = 3;
	  else
	    temp->second = 3;
	}

      else
	fprintf(stderr, "The given arguments cannot be assembled");
    }

}

void table_segment_register(struct temprm * temp, char * reg, bool is_first)
{
 switch (reg[0])
    {
    case 'e':
      if (is_first)
	temp->first = 0;
      else
	temp->second = 0;
      break;
    case 'c':
      if (is_first)
	temp->first = 1;
      else
	temp->second = 1;
      break;
    case 's':
      if (is_first)
	temp->first = 2;
      else
	temp->second = 2;
      break;
    
    case 'd':
      if (is_first)
	temp->first = 3;
      else
	temp->second = 3;
      break;
    case 'f':
      if (is_first)
	temp->first = 4;
      else
	temp->second = 4;
      break;
    case 'g':
      if (is_first)
	temp->first = 5;
      else
	temp->second = 5;
    default:
      fprintf(stderr, "No segment register for the given argument");
      
    }
}

void table_rm_mod00(struct temprm *temp, char * reg, bool is_first, bool rex_r) // Essentially this just means we have a rip pointer or no displacement
{
}


void table_rm_mod01(struct temprm *temp, char * reg, bool is_first, bool rex_r);


void table_rm_mod10(struct temprm * temp, char * reg, bool is_first)
{
  // For all of these they should have a 4 byte offset that is an immediate. If they can be expressed as one byte
  // then what's the point?

  int disp_length = 0;

  while ((reg[disp_length] != '(' ) && disp_length);
  // Disp length is now at the first parenthesis. Needs to be moved past it and the percent
  disp_length += 2;

  
  

  
}









