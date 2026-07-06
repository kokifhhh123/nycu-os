
### inspect ELF
```shell
aarch64-linux-gnu-nm -n kernel8.elf
aarch64-linux-gnu-readelf -S kernel8.elf
```

### inspect bss
```shell
aarch64-linux-gnu-nm -n kernel8.elf | grep -E '__bss_start|__bss_end|__bss_size'
grep -A20 '\.bss' kernel8.map
```

### run QEMU
```shell
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio
```

