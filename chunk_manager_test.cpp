#include <iostream>
#include "chunk_manager_sys.h"
#include <cassert>
#include <sstream>

template <class ChunkManagerType>
void basic_tests(ChunkManagerType &strings)
{
    std::cout << "Basic Testing Policy: " << ChunkManagerType::storage_policy()
              << ", ChunkSize: " << ChunkManagerType::chunk_size() << "\n";
    const char *str1 = strings.store("Apple");
    assert(std::strcmp(str1, "Apple") == 0);

    const char *str2 = strings.store(std::string("Banana"));
    assert(std::strcmp(str2, "Banana") == 0);

    const char *str3 = strings.store("Mango", 5);
    assert(std::strcmp(str3, "Mango") == 0);
}

template <class ChunkManagerType>
void stress_test(ChunkManagerType &strings)
{
    std::cout << "Stress Testing Policy: " << ChunkManagerType::storage_policy()
              << ", ChunkSize: " << ChunkManagerType::chunk_size() << "\n";

    const size_t NUM_TEST_CASES = 1000;

    std::vector<const char *> large_cstr_vect;
    std::vector<const char *> small_cstr_vect;
    large_cstr_vect.reserve(NUM_TEST_CASES);
    small_cstr_vect.reserve(NUM_TEST_CASES);

    for (std::size_t i = 0; i < NUM_TEST_CASES; ++i)
    {
        const char *large_cstr = strings.store("abcdefghi");
        assert(large_cstr != nullptr);
        large_cstr_vect.emplace_back(large_cstr);
    }
    for (std::size_t i = 0; i < NUM_TEST_CASES; ++i)
    {
        const char *small_cstr = strings.store("abcde");
        assert(small_cstr != nullptr);
        small_cstr_vect.emplace_back(small_cstr);
    }
    for (auto cstr : large_cstr_vect)
    {
        assert(std::strcmp(cstr, "abcdefghi") == 0);
    }
    for (auto cstr : small_cstr_vect)
    {
        assert(std::strcmp(cstr, "abcde") == 0);
    }

    std::cout << "Total allocated bytes: " << strings.total_allocated()
              << ", num chunks: " << strings.num_chunks() << "\n";
}

int main()
{
    gen::StringChunkManager<16, gen::ConservativeStoragePolicy> con_strings;
    gen::StringChunkManager<16, gen::BalancedStoragePolicy> bal_strings;
    gen::StringChunkManager<16, gen::GreedyStoragePolicy> gre_strings;

    basic_tests(con_strings);
    basic_tests(bal_strings);
    basic_tests(gre_strings);

    stress_test(con_strings);
    stress_test(bal_strings);
    stress_test(gre_strings);

    return 0;
}