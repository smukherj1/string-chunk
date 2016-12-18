#ifndef CHUNK_MANAGER_SYS_H
#define CHUNK_MANAGER_SYS_H

#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace gen
{
namespace detail
{

template <std::size_t ChunkSize>
class ChunkNode
{
  public:
    ChunkNode()
    {
        m_buf.reset(new char[ChunkSize]);
        m_cur = m_buf.get();
    }

    // Suboptimal so deleted to detect
    // inadvertent copying
    ChunkNode(const ChunkNode &rhs) = delete;
    ChunkNode &operator=(const ChunkNode &rhs) = delete;

    ChunkNode(ChunkNode &&rhs)
    {
        m_buf = std::move(rhs.m_buf);
        m_cur = rhs.m_cur;
    }

    ChunkNode &operator=(ChunkNode &&rhs)
    {
        m_buf = std::move(rhs.m_buf);
        m_cur = rhs.m_cur;
        return *this;
    }

    size_t capacity() const { return m_buf.get() + ChunkSize - m_cur; }

    const char *store(const char *const str, std::size_t len)
    {
        char *cur = m_cur;
        std::memcpy(m_cur, str, len);
        m_cur += len;
        return cur;
    }

  private:
    std::unique_ptr<char[]> m_buf;
    char *m_cur;
};

} /* namespace detail */

template <std::size_t ChunkSize, class NodeType>
class GreedyStoragePolicy
{
  protected:
    const char *execute(const char *str, std::size_t len, std::vector<NodeType> &chunks) const
    {
        auto &last = chunks.back();
        if (last.capacity() >= len)
        {
            return last.store(str, len);
        }
        else
        {
            chunks.emplace_back(NodeType());
            return chunks.back().store(str, len);
        }
    }

    static const char *policy() { return "Greedy"; }
};

template <std::size_t ChunkSize, class NodeType>
class BalancedStoragePolicy
{
  protected:
    const char *execute(const char *str, std::size_t len, std::vector<NodeType> &chunks) const
    {
        auto &last = chunks.back();
        if (last.capacity() >= len)
        {
            return last.store(str, len);
        }
        for (auto reverse_it = chunks.rbegin(); reverse_it != chunks.rend(); ++reverse_it)
        {
            if (reverse_it->capacity() >= len)
            {
                return reverse_it->store(str, len);
            }
        }
        chunks.emplace_back(NodeType());

        return chunks.back().store(str, len);
    }

    static const char *policy() { return "Balanced"; }
};

template <std::size_t ChunkSize, class NodeType>
class ConservativeStoragePolicy
{
  protected:
    const char *execute(const char *str, std::size_t len, std::vector<NodeType> &chunks) const
    {
        for (auto reverse_it = chunks.rbegin(); reverse_it != chunks.rend(); ++reverse_it)
        {
            if (reverse_it->capacity() >= len)
            {
                return reverse_it->store(str, len);
            }
        }
        chunks.emplace_back(NodeType());

        return chunks.back().store(str, len);
    }

    static const char *policy() { return "Conservative"; }
};

template <std::size_t ChunkSize,
          template <std::size_t SizeType, class NodeType> class StoragePolicy = GreedyStoragePolicy>
class StringChunkManager : public StoragePolicy<ChunkSize, detail::ChunkNode<ChunkSize>>
{
    using ChunkNodeType = detail::ChunkNode<ChunkSize>;
    using ConcreteStoragePolicy = StoragePolicy<ChunkSize, ChunkNodeType>;

  public:
    StringChunkManager() { m_chunks.emplace_back(ChunkNodeType()); }

    const char *store(const char *str, std::size_t len)
    {
        if ((len + 1) > ChunkSize)
        {
            return nullptr;
        }
        else
        {
            return ConcreteStoragePolicy::execute(str, len + 1, m_chunks);
        }
    }

    const char *store(const char *str) { return store(str, std::strlen(str)); }

    const char *store(const std::string &str) { return store(str.c_str(), str.size()); }

    std::size_t num_chunks() const { return m_chunks.size(); }
    std::size_t total_allocated() const
    {
        return num_chunks() * (sizeof(ChunkNodeType) + ChunkSize);
    }

    static const char *storage_policy() { return ConcreteStoragePolicy::policy(); }
    static std::size_t chunk_size() { return ChunkSize; }

  private:
    std::vector<ChunkNodeType> m_chunks;
};
} /* namespace gen */

#endif
