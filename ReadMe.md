hellomoto_kmod
==============

Example of Linux (hardhat) kernel module for MotoMAGX phones.

## Requirements

* [MotoMAGX Toolchain & SDK](TODO).
* [Linux (hardhat) kernel source code](https://sourceforge.net/projects/motozinezn5.motorola/files/MOTOZINE%20ZN5/R6637_G_81.11.2CR_128/hardhat-R6637_G_81.11.2CR_128.tgz/download).
* [Mini-Platform build system framework](https://sourceforge.net/projects/motozinezn5.motorola/files/MOTOZINE%20ZN5/R6637_G_81.11.2CR_128/platform-R6637_G_81.11.2CR_128.tgz/download).

## Build

1. Create directory for experiments.

    ```bash
    mkdir -p ~/Projects/MagxKernel
    cd ~/Projects/MagxKernel
    ```

2. Download and install MotoMAGX Toolchain & SDK, setup it.

    TODO

3. Download MotoMAGX mini-platform build system framework, unpack it and setup.

    ```bash
    curl -LOJ "https://master.dl.sourceforge.net/project/motozinezn5.motorola/MOTOZINE%20ZN5/R6637_G_81.11.2CR_128/platform-R6637_G_81.11.2CR_128.tgz"
    tar -xvzf platform-R6637_G_81.11.2CR_128.tgz

    # Fix wrong file rights and set executable flag on necessary script.
    find platform -type f ! -path "*/.git/*" -exec chmod 0644 {} \;
    chmod 0755 platform/tools/bin/api_manager

    mv platform/Makefile.top Makefile

    # Just set "COMPILERTOOLPATH" variable to our MotoMAGX Toolchain & SDK.
    sed -i 's/COMPILERTOOLPATH = \/vobs\/jem\/cee4_lsp\/mobilinux\/devkit\/arm\/v6_vfp_le\/bin/COMPILERTOOLPATH = \/arm-eabi\/bin/g' Makefile
    ```

4. Download Linux (hardhat) sources, unpack it, setup and compile.

    ```bash
    curl -LOJ "https://master.dl.sourceforge.net/project/motozinezn5.motorola/MOTOZINE%20ZN5/R6637_G_81.11.2CR_128/hardhat-R6637_G_81.11.2CR_128.tgz"
    tar -xvzf hardhat-R6637_G_81.11.2CR_128.tgz

    # Fix some compilation error via patching vanilla file.
    # Additional information: https://github.com/GodFox/magx_kernel_xpixl/pull/1
    sed -i 's/+ =/+=/g' hardhat/linux-2.6.x/drivers/media/dvb/b2c2/Makefile

    make hardhat.dir V=1
    ```

    *Alternatively*, you can use the already patched and modern kernel sources from [magx_kernel_xpixl](https://github.com/GodFox/magx_kernel_xpixl) by GodFox and [motomagx-kernel_linux_hardhat](https://github.com/fillsa/motomagx-kernel_linux_hardhat) by fill.sa projects.

    ```bash
    # 1. https://github.com/fillsa/motomagx-kernel_linux_hardhat by fill.sa.
    git clone https://github.com/fillsa/motomagx-kernel_linux_hardhat hardhat
    cd hardhat
    ln -s properties.xpixl.11.2cr properties
    cd ..

    # 2. https://github.com/GodFox/magx_kernel_xpixl by GodFox.
    git clone https://github.com/GodFox/magx_kernel_xpixl hardhat
    ```

5. Download this Linux (hardhat) kernel module sources and compile it.

    ```bash
    git clone https://github.com/EXL/hellomoto_kmod
    make hellomoto_kmod.dir V=1
    ```

6. Get compiled and ready to use Linux (hardhat) kernel module `hellomoto.ko` from `build/hellomoto_kmod/lib/modules/2.6.10_dev/extra/` directory.

## Use

```bash
insmod hellomoto.ko
dmesg

# Simulate pressing the "1" button on keypad.
# See all keycodes here: https://github.com/fillsa/motomagx-kernel_linux_hardhat/blob/master/linux-2.6.x/include/linux/keypad.h
echo "1" > /proc/hellomoto
dmesg

# Get status of camera lens cover by GPIO pin.
# See GPIO signals here: https://github.com/fillsa/motomagx-kernel_linux_hardhat/blob/master/linux-2.6.x/include/asm-arm/mot-gpio.h
cat /proc/hellomoto # Get status of camera lens cover.
```

## Information

See additional information in the [The Linux Kernel Module Programming Guide | 5.2. Read and Write a /proc File](https://linux.die.net/lkmpg/x769.html) article, [TODO](https://exlmoto/TODO) article, and [Сборка ядра для ZN5. Compiling your own kernel | TODO](https://forum.motofan.ru/index.php?showtopic=178297) (in Russian) thread post.

// TODO: Fix link, add exlmoto.ru article
