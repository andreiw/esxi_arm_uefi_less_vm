/*
 * Copyright (C) 2020 Andrei Warkentin <andrey.warkentin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <defs.h>
#include <libfdt.h>
#include <vsprintf.h>
#include <serial.h>

static void dcc_puts(const char *s)
{
  while (*s != '\0') {
    asm volatile("msr dbgdtrtx_el0, %0" : : "r" (*s++));
  }
}

static void printk(char *fmt, ...)
{
  va_list list;
  char buf[512];
  va_start(list, fmt);
  vsnprintf(buf, sizeof(buf), fmt, list);
  dcc_puts(buf);
  serial_puts(buf);
  va_end(list);
}

static uint64_t getprop64_or_0(void *fdt, int node, char *prop)
{
  const uint64_t *be64_data = fdt_getprop(fdt, node, prop, NULL);
  if (be64_data == NULL) {
    return 0;
  }

  return be64_to_cpu(*be64_data);
}

void demo(void *fdt)
{
  int node;
  char *initrd_start, *initrd_end;
  extern void *image_start, *image_end;

  dcc_puts("\nHello DCC!\n\n");

  node = fdt_path_offset(fdt, "serial0");
  if (node >= 0) {
    void *base = (void *) getprop64_or_0(fdt, node, "reg");
    printk("UART base %p\n", base);
    serial_init(base);
    serial_puts("\nHello UART!\n\n");
  }

  printk("We are 0x%lx bytes at %p\n",
	 (uint64_t) &image_end - (uint64_t) &image_start, &image_start);

  node = fdt_path_offset(fdt, "/");
  if (node >= 0) {
    printk("Running on a '%s', '%s'\n",
	   fdt_getprop(fdt, node, "compatible", NULL),
	   fdt_getprop(fdt, node, "model", NULL));
  }

  node = fdt_path_offset(fdt, "/hypervisor");
  if (node >= 0) {
    printk("Hypervisor: '%s'\n", fdt_getprop(fdt, node, "compatible", NULL));
  }

  node = fdt_path_offset(fdt, "/chosen");
  if (node < 0) {
    printk("No /chosen\n");
    return;
  }

  printk("Passed command line: '%s'\n", fdt_getprop(fdt, node, "bootargs", NULL));

  initrd_start = (void *) getprop64_or_0(fdt, node, "linux,initrd-start");
  initrd_end = (void *) getprop64_or_0(fdt, node, "linux,initrd-end");
  printk("initrd-start = %p\n", initrd_start);
  printk("initrd-end = %p\n", initrd_end);

  printk("Powering off...\n\n");
}
