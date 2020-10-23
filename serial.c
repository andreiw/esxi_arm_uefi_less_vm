/*
 * Copyright (C) 2017 Andrei Evgenievich Warkentin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <defs.h>

static volatile char *regs = (void *) NULL;

enum regs {
  DR = 0,     // Data Register
  IER = 1,    // Interrupt Enable Register
  FCR = 2,    // FIFO Control Register
  LCR = 3,    // Line Control Register
  MCR = 4,    // Modem Control Register
  LSR = 5,    // Line Control Register
  MSR = 6,    // Modem Status Register
  SCR = 7,    // Scratch Register
  DLL = 0,    // Divisor Latch LSB Register
  DLM = 1,    // Divisor Latch MSB Register
};

enum lcr {
	// When bit 7 (DLAB) of LCR is set to 1, the two registers 0 and 1
	// change their meaning and become two bytes controlling the baud rate
	DLAB     = 0x80,    // Divisor Latch Access Bit in LCR register
	LEN_8BIT = 3,
};

// Various bits of the Line Status Register
enum lsr {
	RECEIVE_DATA_READY  = 0x1,
	OVERRUN             = 0x2,
	PARITY_ERROR        = 0x4,
	FRAME_ERROR         = 0x8,
	BREAK_INTERRUPT     = 0x10,
	TRANSMIT_HOLD_EMPTY = 0x20,
	TRANSMIT_EMPTY      = 0x40,
	FIFO_ERROR          = 0x80,
};

// Various bits of the Modem Control Register
enum mcr {
  DTR           = 0x1,
  RTS           = 0x2,
  AUX_OUTPUT_1  = 0x4,
  AUX_OUTPUT_2  = 0x8,
  LOOPBACK_MODE = 0x16,
};

void serial_init(void *p)
{
  regs = p;
  /*
   * Assumes PC-style UART clock.
   */
  regs[LCR] = LEN_8BIT | DLAB;
  regs[DLL] = 1;
  regs[DLM] = 0;
  regs[LCR] = LEN_8BIT;
  regs[FCR] = 0;
  regs[IER] = 0;
}

void serial_putc(char ch)
{
  uint8_t val;

  if (regs == NULL) {
    return;
  }

  do {
    val = regs[LSR];
  } while (!(val & TRANSMIT_HOLD_EMPTY));

  regs[DR] = ch;
}

void serial_puts(const char *s)
{
  if (regs == NULL) {
    return;
  }

  while (*s != '\0') {
    serial_putc (*s++);
  }
}
