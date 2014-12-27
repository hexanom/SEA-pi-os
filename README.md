pi-os
=====

A simple Raspberry PI / ARM toy operating system written in C and ARM assembly.

Goals
-----

This project was intended for educational purposes only. **You shouldn't use it in production !** (you won't go far with it anyway, stick with a GNU/Linux distribution instead !)

Here are the parts already implemented:

- Preemptive scheduler
- Memory Pagination management via ARM's MMU.
- Virtual Memory pager
- System calls

Here are the parts that will be implemented in the future:

- Memory protection between processes and kernel (Ring 0/Ring 1 separation)
- `fork()` syscall

Here are the parts that might be implemented in the future:

- Simple init system (pid 1)
- Basic FS
- Simple CLI

Usage
-----

To make the development easier, you should use [Vagrant](https://www.vagrantup.com/):

```shell
vagrant up
```

Then you can ssh yourself into the vm that will be running the emulator.

```shell
vagrant ssh
```

![http://i.imgur.com/ltB0YyI.jpg](Yo dawg, I heard you like emulation; So I put an emulator inside a VM inside your host machine)

Now it's compilation time !

```shell
cd /vagrant
make
```

And then, let's start qemu and gdb to control how the execution works...

```shell
cd /vagrant/debug
sh run-qemu.sh &
sh run-gdb.sh
```

At this point you can input `c` into gdb to go to the first breakpoint inside the kernel.

Getting to the next line will be `n` and entering inside a function call will be `s`.
