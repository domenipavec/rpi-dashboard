# Building form source

Choose your Linux distribution and follow the steps:
* [Raspbian](build_on_raspbian.md)
* [Pidora](build_on_pidora.md)
* [Arch Linux](build_on_arch_linux.md)

After dudac finishes building duda-raspbarry, the service starts. Close dudac with CTRL-C.

Monkey and duda are installed in ~/.dudac/stage/monkey. To start the Monkey server daemon, run:
```
~/.dudac/stage/monkey/bin/monkey -D
```

To start the Monkey server on boot on Raspbian or Pidora, use [init.d script](initd_script.md).

If you want to use i2c, you have to [enable](../system_configuration/enable_i2c.md) it.
