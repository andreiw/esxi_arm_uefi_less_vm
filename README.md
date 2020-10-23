UEFI-less VM in ESXi-Arm Fling
==============================

This is a simple example of running code in a "raw" VM without UEFI.

You start with:
- MMU off
- DTB in x0

Note: ESXi-Arm Fling is not a product and as such, anything can change
without notice: virtual machine layout, virtual devices, hypervisor
interfaces, etc.

Requirements
============

ESXi-Arm Fling 1.1.

Building
========

You need GCC, make, etc. Tested with gcc version 8.3.0 (Debian 8.3.0-2).

On Arm Linux:

`$ make`

On non-Arm Linux:

`$ CROSS_COMPILE=aarch64-linux-gnu- make`

Running
=======

I just put the whole directory on an NFS exported path, and register `demo.vmx` directly. Of course,
you could just copy `demo.vmx`, `Image.bin` and `dummy.initrd` to wherever you want.

If you do register the VM within the source directory, the `make cleaner` target will remove all
the VMware stuff.

Now power on the VM. The VM will power itself off. The VM doesn't perform any video output,
it just logs stuff to `serial.0.log` (corresponding to the serial port) and `vmware.log`.

    $ cat serial.0.log
   
    Hello UART!
   
    We are 0xee98 bytes at 0000000080080000
    Running on a 'arm,vmware-vbsa', 'linux,dummy-virt'
    Hypervisor: 'vmware'
    Passed command line: 'bootargs for Image.bin'
    initrd-start = 0000000087ffffd8
    initrd-end = 0000000088000000
    Powering off...

    $ _
