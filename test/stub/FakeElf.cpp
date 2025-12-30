/* -------------------------------------------------------------------------
 * This file is part of the MindStudio project.
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * MindStudio is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * ------------------------------------------------------------------------- */


#include "utils/Serialize.h"

#include "FakeElf.h"

constexpr Elf64_Half SECTION_NUM = 2;
constexpr Elf64_Xword SECTION_SIZE = 18;
constexpr Elf64_Word SHSTRTAB_OFFSET = 1;
constexpr Elf64_Word DUMMY_OFFSET = 11;

Elf64_Ehdr CreateValidElfHeader(void)
{
    Elf64_Ehdr header {};
    header.e_ident[EI_MAG0] = ELFMAG0;
    header.e_ident[EI_MAG1] = ELFMAG1;
    header.e_ident[EI_MAG2] = ELFMAG2;
    header.e_ident[EI_MAG3] = ELFMAG3;
    header.e_ident[EI_CLASS] = ELFCLASS64;
    header.e_ident[EI_DATA] = ELFDATA2LSB;
    header.e_shnum = SECTION_NUM;
    header.e_shstrndx = 0;
    header.e_shentsize = sizeof(Elf64_Shdr);
    header.e_shoff = sizeof(Elf64_Ehdr);
    return header;
}

Buffer CreateValidNameSection(void)
{
    std::vector<std::string> sectionNames = {
        ".shstrtab",
        ".dummy"
    };
    // string table starts with NUL
    Buffer section(1, '\0');
    for (auto const &name : sectionNames) {
        std::copy(name.cbegin(), name.cend(), std::back_inserter(section));
        section.push_back('\0');
    }
    return section;
}

Buffer CreateValidElf(void)
{
    Elf64_Ehdr elfHeader = CreateValidElfHeader();
    Elf64_Shdr nameSectionHeader {};
    nameSectionHeader.sh_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Shdr) + sizeof(Elf64_Shdr);
    nameSectionHeader.sh_size = SECTION_SIZE;
    nameSectionHeader.sh_name = SHSTRTAB_OFFSET;
    Elf64_Shdr dummySectionHeader {};
    dummySectionHeader.sh_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Shdr) + sizeof(Elf64_Shdr);
    dummySectionHeader.sh_size = SECTION_SIZE;
    dummySectionHeader.sh_name = DUMMY_OFFSET;
    return Serialize<Buffer>(elfHeader) + Serialize<Buffer>(nameSectionHeader) +
        Serialize<Buffer>(dummySectionHeader) + CreateValidNameSection();
}
