﻿#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "../include/sib.h"
#include <math.h>
#include "../include/customtypes.h"
#include "../include/get_instructions.h"
#include "../include/syntax_checker.h"
#include "../include/utilities.h"
// This file needs to go through line by line and check for tokens. It will just look for whitespace and endlines
// text holds the actual instructions and registers
// the other sections have to look through the declared variables and stuff
typedef struct memory_op_info regular_memory_operand;
typedef struct sib sib_pieces;

bool check_sizes(int size_in, struct available_sizes available)
{
  switch (size_in)
    {
    case 8:
      return available.one_byte;
    case 16:
      return available.byte_2;
    case 32:
      return available.byte_4;
    case 64:
      return available.byte_8;
    case 128:
      return available.byte_16;
    default:
      return 0;
      
    }
}

enum section_types check_for_section_label(const char * input_string, int * iterator_in)
{
  int start_iterator = 0;
  char temp_string[30] = {0};
  for (; input_string[start_iterator] == ' ' || input_string[start_iterator] == 9; start_iterator++);

  if (input_string[start_iterator] != '.') //start iterator may be 0
    {
      *iterator_in = 0; // Just start from the beginning
      return -1;
    }

  int string_iterator = 0;
  for (; input_string[start_iterator] != ' '; ++start_iterator, ++string_iterator)
    {
      temp_string[string_iterator]= input_string[start_iterator];
    }

  if (compare_strings(temp_string, ".text"))
    {
      *iterator_in = start_iterator;
      return text;
    }

  else if (compare_strings(temp_string, ".bss"))
    {
      *iterator_in = start_iterator;
      return bss;
    }

  else if (compare_strings(temp_string, ".data"))
    {
      *iterator_in = start_iterator;
      return data;
    }
  else
    {
      *iterator_in = 0;
      return invalid_section_label;
    }
  
}


void init_op_info(struct memory_op_info *in)
{
  in->disp_length = 0;
  in->disp_offset = -1;
  in->reg1_off = -1;
  in->reg2_off = -1;
  in->sib = 0;
  in->sib_scale = 0;
}



  
int ascii_to_int( char * in, int *returned_index)
{
  int temp = 0;
  int sum = 0;

  while (in[temp] != '(' )
    {
      sum *= 10;
      sum += (in[temp]-48);
      ++temp;
    }
  *returned_index = temp; // The last one will be the parentheses
  return sum;
}

void search_line(FILE * file_in, struct instruction_pieces *arguments, enum section_types current_type)
{
  // The fp has information on where in the file we are, this assumes that we
  // are at the beginning of the line and that the file is opened already
  // If the beginning of the line is not a space then it should be a label
  int line_iterator = 0;
  char input_string[250] = {0};// Allocate 250 bytes for the string
  size_t line_length = fill_string_with_line(250, input_string, file_in);
  enum section_types type = check_for_section_label(input_string);

  if (type == invalid_section_label)
    {
      fprintf(stderr, "Invalid section label");
      assert(0 == 1);
    }

  // Make sure that everything is part of the text section
  

  


}
  
  
  // TODO: Make the array of instructions from the table and the structs of instructions
  // Along with the IDs of the instructions
int binary_lookup(unsigned  int in, unsigned  int* array_in)
{
  int high = 1073;
  int low = 0;
  int mid = 0; // returned
  int counter = 12; // pretty close to log 2 1074 

  while (counter-- &&  array_in[mid] != in)
    {
      mid = (high + low)/2;
      if (array_in[mid] < in) // need to make the bottom move up search higher range
	low = mid;
      else // move the top down to search the smaller numbers
	high = mid;

    }
  if (counter == 0)  // failed
    return -1;

  else
    return mid;
}

int rip_suffix(char *instruction_mnemonic, int depth)
{
  //  suffixes
  int length = 0;
  while (instruction_mnemonic[length])
    {
      length++;
    }
  
  for (int i = 0; i < depth; ++i)
    {
      instruction_mnemonic[length - i] = 0;
    }


}

  
int search_for_mnemonic (unsigned long int mnemonic_ID, unsigned long *array)
{
  int high = 1073; int low = 0; int mid = 0;

  int searches = 0;
  

  while (searches++ < 20 && array[searches] != mnemonic_ID)
    {
      mid = (high + low)/2;
      if (mnemonic_ID > array[mid]) // We are too low currently
	{
	  low = mid;
	}
      else if (mnemonic_ID < array[mid])
	{
	  high = mid;
	}
      else
	return mid;
    }

  return -1;
  
}
    



 inline int check_for_offset(char * string, int *start_parentheses, int *disp_value)
{
  // Should return the number of bytes for the offset
  // Modified should be the index at which the parentheses start
  int length_counter = 2;
  int total = 0;
  if (string[0] == '0' && string[1] == 'x') // Hexadecimal number
    {
      // Hexadecimal offset or a number
      for (; string[length_counter] >= 48 && string[length_counter] < 58; ++length_counter)
	{
	  char temp = string[length_counter] - 48;
	  total |= temp;
	  total <<= 4;
	}
      *disp_value = total;
      *start_parentheses = length_counter;

      if (length_counter - 2 > 8) // Too big
	{
	  fprintf(stderr, "Disp is greater than 4 bytes");
	  assert(0 == 1);
	}
      
      if ((length_counter -2) > 2) // Need more than one byte to process this
	return 4;
      
      else
	return 1;
    }

  else if  (string[0] >= 48 && string[0] < 58)
    {
      // This is just a decimal offset
      *disp_value = ascii_to_int(string, start_parentheses);
      if (*disp_value> 255)
	{
	  return 1;
	}

      else
	return 4;    
    }
  
  else
    return 0;
}

      
// Don't write yet, we need to know whether it is an sib,etc
regular_memory_operand check_memory_operand(char * memory_instruction_in) // Should be a given that this is a memroy type
 {
   // Needs to check for an offset first
   // SIB -----> offset(base, index, scale)
   // This does just a regular memory
   
   regular_memory_operand mem_operand;
   mem_operand.disp = displacement_value;
   mem_operand.disp_length = check_for_offset(memory_instruction_in, &mem_operand.first_paren_offset, &mem_operand.disp);
   return mem_operand;
 }

sib_pieces sib_from_string(char *sib_instruction_in)
{
  // This just does the whole sib, knowing that it is an sib
  sib_pieces sib_returned;
  sib_returned.disp_length_in_bytes = check_for_offset(sib_instruction_in, &sib_returned.start_parentheses_index, &sib_returned.disp_value);

  int iterator = sib_returned.start_parentheses_index + 1;

  iterator = move_to_general(sib_instruction_in, iterator, '%');
  sib_returned.base_index = iterator;

  iterator = move_to_general(sib_instruction_in, iterator, '%');
  sib_returned.index_index = iterator;

  iterator = move_to_general(sib_instruction_in, iterator, ',');
  while (sib_instruction_in[iterator] == ' ')
    {
      iterator++
    }

  if (sib_instruction_in[iterator] < 48 || sib_instruction_in[iterator] > 57)
    {
      fprintf(stderr, "Failure with the scale index base byte" );
    }
  
  sib_returned.scale = sib_instruction_in[iterator] - 48;
  sib_returned.operand = sib_instruction_in;

  return sib_returned;
  
}



void check_instruction(struct instruction_pieces *in, unsigned long int *shorter_mnemonics, unsigned long int *longer_mnemonics, struct dependencies *dep)
{
  /* Now some requirements for instructions will be listed
     - < 2 memory operands 
     - No direction in ModRM, rather in the instruction  
    - < 2 immediates
     - If a dependency check fails for a mnemonic, try one of the nearest ones. Likely, lots of them will fail as of now
   */
  int ID = name_to_id(in->instruction_mnemonic);
  int index = -1;
  if (!ID)
    {
      ID = name_to_id((in->instruction_mnemonic +8));
        for (int i = 0; i < 3; ++i)
	  {
	    rip_suffix(in->instruction_mnemonic, i);
	    search_for_mnemonic(ID, longer_mnemonics );
	    if (index != -1)
	      break;
	  }
    }

  else
    {
      for (int i = 0; i < 3; ++i)
	  {
	    rip_suffix(in->instruction_mnemonic, i);
	    search_for_mnemonic(ID, shorter_mnemonics );
	    if (index != -1)
	      break;
	  }
      
    }

  
  if (index == -1)
    {
      fprintf(stderr, "Failure to find a matching mnemonic for the opcode");
      assert(0 == 1);
    }
  
  else if (ID)
    {

      // Found in the shorter ones, look at the dependencies for this one and its neighbors
      int valid_neighbors[16] = { 0 };
      int valid_neighbors_number = 0;
      // Check below
      for (int i = 0; shorter_mnemonics[index - i] == ID; ++i) // look below thje index
      {
          valid_neighbors[i] = index - i;
          valid_neighbors_number++;
      }
      
      for (int i = 0; shorter_mnemonics[index + i] == ID; ++i) // look above 
      {
          valid_neighbors[i] = index + i;
          valid_neighbors_number++;
      }


       for (int iterator = 0; iterator < valid_neighbors_number; ++iterator)
        {
          // NOT DONE YET
	  // Will use the dependencies check function for this one
          if (assert_dependencies(in, dep[(valid_neighbors[valid_neighbors_number])]));
	  {
	    return valid_neighbors_number; // THE TRUTH
	  }
        }

      
    }

  else
    {
      // Found in the longer ones, look at the dependencies for this one and its neighbors
      int valid_neighbors[16] = { 0 };
      int valid_neighbors_number = 0;
      
      // Check below
      for (int i = 0; longer_mnemonics[index - i] == ID; ++i)
      {
          valid_neighbors[valid_neighbors_number++] = index - i;
          
      }
      // Check above 
      for (int i = 0; longer_mnemonics[index + i] == ID; ++i)
      {
          valid_neighbors[valid_neighbors_number++] = index + i;
      }
      

      bool match_found = 0;
      for (int iterator = 0; iterator < valid_neighbors_number; ++iterator)
        {
          // NOT DONE YET
	  // Will use the dependencies check function for this one
          if (assert_dependencies(in, dep[(valid_neighbors[valid_neighbors_number])]));
	  {
	    return valid_neighbors_number; // THE TRUTH
	  }
        }



    }
  
  
} 
bool assert_dependencies(struct instruction_pieces *user_in, struct dependencies *table_in)
{
  /*
    - Pass the same enum operands
    - Pass the lock check 
    - Pass the size check 
 
   */
  if (user_in->wants_lock && !(table_in->lockable)) // Failure, return a zero
    {
      return 0;
    }

  if (user_in->op1 != table_in->one)
    {
      return 0;
    }

  if (user_in->op2 != table_in->two)
    {
      return 0;
    }

  if (!check_sizes(user_in->op1_size, table_in->allowed_sizes))
    {
      return 0;
    }
  
  if (!check_sizes(user_in->op2_size, table_in->allowed_sizes))
    {
      return 0;
    }

  
  
  return 1;
}

 
enum Basic_Operands user_string_to_operand(const char *string_in, int start_index, struct memory_op_info *in_case)
{
  // Needs to be called after the offset is checked for
  
  int initial_start = start_index;
  int temp_zero = 0;
  in_case->disp_length = check_for_offset(string_in, &temp_zero, &start_index);
  

  
  if (string_in[start_index] == '(')
    {
      int reg_counter = 0;
      for (; string_in[start_index] != ')'; ++start_index)
	{
	  if (string_in[start_index] == '%')
	    {
	      ++reg_counter;
	    }
	}

      if (reg_counter == 2)
	{
	  return sib;
	}

      else if (reg_counter == 1)
	{
	  return memory;
	}

      else
	{
	  fprintf(stderr, "Failure to parse the memory addressing operand");
	  assert( 0 == 1);
	}
    }

  else if (string_in[start_index] == '%')
    {
      return reg;
    }


  else if (string_in[start_index] == '$')
    {
      return immediate;
    }

  else
    {
      fprintf(stderr, "operand not found" );
      assert( 0 == 1);
    }
  
}

void _text_checker(FILE * in);
  // Need to look for the text or the other sections in the beginning
  // For now there needs to be some type of variable that when this thing is run it keeps
  // track of the current section
  // should have some types of variable as well as some read only stuff
  // also there needs to be a .text section for this
  // First look for the first instruction and then the two registers

  /* Instruction format: 
     movq %rax, %rbx \n */
