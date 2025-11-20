#include <elf.h>
#include <stdio.h>

// Define the ELF header structure
Elf64_Ehdr elf_hdr = {
    .e_ident = {
        ELFMAG0,
        ELFMAG1,
        ELFMAG2,
        ELFMAG3,
        ELFCLASS64,
        ELFDATA2LSB,
        EV_CURRENT,
        ELFOSABI_LINUX,
        0, 0, 0, 0, 0, 0, 0, 0},
    .e_type = ET_EXEC,
    .e_machine = EM_X86_64,
    .e_version = EV_CURRENT,
    .e_entry = 0x40007f,
    .e_phoff = 64,
    .e_shoff = 0,
    .e_flags = 0,
    .e_ehsize = 64,
    .e_phentsize = 56,
    .e_phnum = 1,
    .e_shentsize = 64,
    .e_shnum = 0,
    .e_shstrndx = SHN_UNDEF,
};

// Define program header
// Actually this Prgm Header will only contain one section 
// that holds the object code to run

Elf64_Phdr prgm_hdr = {
    .p_type = PT_LOAD,
    .p_offset = 0x78, // since we will put the section containing instructions
                      //  after just after the elf header (size = 64) and the program header (size = 56 )
                      //  so it will be puted after 64 + 56 = 120 => 0x78 in hexadecimal.
    .p_vaddr = 0x400078, // base@ +  p_offset ( we choose 0x40000 in our case )
    .p_paddr = 0x400078,
    .p_filesz = 44,
    .p_memsz = 44,
    .p_flags = PF_X | PF_R,
    .p_align = 0x8,
};

int main()
{

    FILE *fptr;

    unsigned char objectcode[] = {
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x0a,                   // Hello! string
        0xb8, 0x01, 0x00, 0x00, 0x00,                               // mov rax, 1 : write syscall number
        0xbf, 0x01, 0x00, 0x00, 0x00,                               // mov rdi, 1 : stdout
        0x48, 0xbe, 0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rsi, @hello string : @hello string is p_vaddr since it
                                                                    //  is in the start of the section
        0xba, 0x07, 0x00, 0x00, 0x00, // mov rdx, 7 : size of hello!\n 
        0x0f, 0x05,                   // syscall
        0xb8, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60 : exit syscall number 
        0x48, 0x31, 0xff,             // xor rdi, rdi
        0x0f, 0x05                    // syscall
    };
    fptr = fopen("elf", "w");
    if (!fptr)
    {
        perror("Failed to create/open the output elf file");
        return 1;
    }
    size_t elf_header_size = fwrite(&elf_hdr, 1, sizeof(elf_hdr), fptr);
    if (elf_header_size != sizeof(elf_hdr))
    {
        perror("Failed to write elf header into the output elf file");
        return 1;
    }

    size_t prgrm_header_size = fwrite(&prgm_hdr, 1, sizeof(prgm_hdr), fptr);
    if (prgrm_header_size != sizeof(prgm_hdr))
    {
        perror("Failed to write program header into the output elf file");
        return 1;
    }

    size_t object_size = fwrite(&objectcode, 1, sizeof(objectcode), fptr);
    if (object_size != sizeof(objectcode))
    {
        perror("Failed to write elf object code into the output elf file");
        return 1;
    }

    return 0;
}
