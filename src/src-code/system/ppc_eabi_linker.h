#ifndef __PPC_EABI_LINKER_H
#define __PPC_EABI_LINKER_H

__declspec(section ".init") extern char _f_init[];			   /* .init dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_init[];			   /* .init dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_text[];			   /* .text dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_text[];			   /* .text dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_rodata[];		   /* .rodata dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_rodata[];		   /* .rodata dest. (RAM) address after end byte */
__declspec(section ".init") extern char _fextab[];			   /* extab dest. (RAM) address at beginning */
__declspec(section ".init") extern char _eextab[];			   /* extab dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_data[];			   /* .data dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_data[];			   /* .data dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_bss[];			   /* .bss dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_bss[];			   /* .bss dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_sdata[];			   /* .sdata dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_sdata[];			   /* .sdata dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_sbss[];			   /* .sbss dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_sbss[];			   /* .sbss dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_sdata2[];		   /* .sdata2 dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_sdata2[];		   /* .sdata2 dest. (RAM) address after end byte */
__declspec(section ".init") extern char _f_sbss2[];			   /* .sbss2 dest. (RAM) address at beginning */
__declspec(section ".init") extern char _e_sbss2[];			   /* .sbss2 dest. (RAM) address after end byte */

#endif