.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot_data, "aw"
    .long MAGIC
    .long FLAGS
    .long CHECKSUM