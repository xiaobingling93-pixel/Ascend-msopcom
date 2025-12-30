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


#include "utils/ElfLoader.h"
#include "utils/Serialize.h"
#include "stub/FakeElf.h"

#include <elf.h>
#include <gtest/gtest.h>

TEST(ElfLoader, load_from_empty_buffer_expect_return_false)
{
    ElfLoader loader;
    Buffer buffer;
    ASSERT_FALSE(loader.FromBuffer(buffer));
}

TEST(ElfLoader, load_from_buffer_with_too_short_elf_header_expect_return_false)
{
    ElfLoader loader;
    Buffer buffer(sizeof(Elf64_Ehdr) - 1);
    ASSERT_FALSE(loader.FromBuffer(buffer));
}

TEST(ElfLoader, load_from_buffer_with_elf_header_of_invalid_elf_magic_expect_return_false)
{
    ElfLoader loader;
    Elf64_Ehdr header = CreateValidElfHeader();
    // 依次校验前 4 位 magic
    header.e_ident[EI_MAG3] = ELFMAG3;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
    header.e_ident[EI_MAG2] = ELFMAG2;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
    header.e_ident[EI_MAG1] = ELFMAG1;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
    header.e_ident[EI_MAG0] = ELFMAG0;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
}

TEST(ElfLoader, load_from_buffer_with_elf_header_of_non_64bits_format_expect_return_false)
{
    ElfLoader loader;
    Elf64_Ehdr header = CreateValidElfHeader();
    header.e_ident[EI_CLASS] = ELFCLASS32;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
}

TEST(ElfLoader, load_from_buffer_with_elf_header_of_big_endian_format_expect_return_false)
{
    ElfLoader loader;
    Elf64_Ehdr header = CreateValidElfHeader();
    header.e_ident[EI_DATA] = ELFDATA2MSB;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
}

TEST(ElfLoader, load_from_buffer_with_elf_header_of_too_many_sections_expect_return_false)
{
    ElfLoader loader;
    Elf64_Ehdr header = CreateValidElfHeader();
    header.e_shnum = 1;
    header.e_shstrndx = 2;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
}

TEST(ElfLoader, load_from_buffer_with_buffer_of_too_short_section_header_expect_return_false)
{
    // data structure: <Elf64_Ehdr>
    ElfLoader loader;
    Elf64_Ehdr header = CreateValidElfHeader();
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(header)));
}

TEST(ElfLoader, load_from_buffer_with_buffer_of_too_short_name_buffer_expect_return_false)
{
    // data structure: <Elf64_Ehdr> -- <Elf64_Shdr>
    ElfLoader loader;
    Elf64_Ehdr elfHeader = CreateValidElfHeader();
    Elf64_Shdr nameSectionHeader {};
    nameSectionHeader.sh_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Shdr);
    nameSectionHeader.sh_size = 64;
    ASSERT_FALSE(loader.FromBuffer(Serialize<Buffer>(elfHeader) + Serialize<Buffer>(nameSectionHeader)));
}

TEST(ElfLoader, load_from_buffer_with_valid_buffer_expect_return_true)
{
    // data structure: <Elf64_Ehdr> -- <Elf64_Shdr> -- <Elf64_Shdr> -- <.shstrtab>
    ElfLoader loader;
    ASSERT_TRUE(loader.FromBuffer(CreateValidElf()));
}

TEST(Elf, get_section_headers_from_valid_elf_expect_return_correct_section_headers)
{
    ElfLoader loader;
    loader.FromBuffer(CreateValidElf());
    Elf elf = loader.Load();
    auto const &sectionHeaders = elf.GetSectionHeaders();
    ASSERT_EQ(sectionHeaders.size(), 2ULL);
    ASSERT_NE(sectionHeaders.find(".shstrtab"), sectionHeaders.cend());
    ASSERT_NE(sectionHeaders.find(".dummy"), sectionHeaders.cend());
}

TEST(Elf, read_raw_data_with_not_exists_section_name_expect_return_empty_data)
{
    ElfLoader loader;
    loader.FromBuffer(CreateValidElf());
    Elf elf = loader.Load();
    auto buffer = elf.ReadRawData(".not_exists");
    ASSERT_TRUE(buffer.empty());
}

TEST(Elf, read_raw_data_with_exists_section_name_expect_return_correct_raw_data)
{
    ElfLoader loader;
    loader.FromBuffer(CreateValidElf());
    Elf elf = loader.Load();
    auto buffer = elf.ReadRawData(".shstrtab");
    ASSERT_EQ(buffer.size(), 18);
}

TEST(Elf, read_string_table_with_not_exists_section_name_expect_return_empty_list)
{
    ElfLoader loader;
    loader.FromBuffer(CreateValidElf());
    Elf elf = loader.Load();
    auto strings = elf.ReadStringTable(".not_exists");
    ASSERT_TRUE(strings.empty());
}

TEST(Elf, read_string_table_from_shstrtab_section_expect_return_section_names)
{
    ElfLoader loader;
    loader.FromBuffer(CreateValidElf());
    Elf elf = loader.Load();
    auto names = elf.ReadStringTable(".shstrtab");
    ASSERT_EQ(names.size(), 2UL);
    ASSERT_EQ(names[0], ".shstrtab");
    ASSERT_EQ(names[1], ".dummy");
}
