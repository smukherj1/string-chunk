#ifndef CHUNK_MANAGER_SYS_H
#define CHUNK_MANAGER_SYS_H

#include <vector>
#include <memory>
#include <string>
#include <cstring>

namespace gen
{
	namespace detail
	{
		class ChunkNode
    	{
    	public:
    		ChunkNode(const size_t len)
    		{
    			m_buf.reset(new char[len]);
    			m_end = m_buf.get() + len;
    			m_cur = m_buf.get();
    		}

    		// Suboptimal so deleted to detect
    		// inadvertent copying
    		ChunkNode(const ChunkNode& rhs) = delete;
    		ChunkNode& operator = (const ChunkNode& rhs) = delete;


    		ChunkNode(ChunkNode&& rhs) = default;
    		ChunkNode& operator = (ChunkNode&& rhs) = default;

    		size_t space() const { return m_end - m_cur; }

    		const char* store(const char * const str, std::size_t len)
    		{
    			char *cur = m_cur;
    			std::memcpy(m_cur, str, len);
    			m_cur += len;
    			return cur;
    		}

    	private:
    		std::unique_ptr<char[]> m_buf;
    		char *m_end;
    		char *m_cur;
    	};

	} /* namespace detail */

    template <std::size_t ChunkSize>
	class GreedyStoragePolicy
	{
	protected:
		const char* execute(const char *str, std::size_t len, std::vector<detail::ChunkNode>& chunks ) const
		{
			std::cout << "Greedy\n";
			return nullptr;
		}
	};

	template <std::size_t ChunkSize>
	class BalancedStoragePolicy
	{
	protected:
		const char* execute(const char *str, std::size_t len, std::vector<detail::ChunkNode>& chunks ) const
		{
			std::cout << "Balanced\n";
			return nullptr;
		}
	};

	template <std::size_t ChunkSize>
	class ConservativeStoragePolicy
	{
	protected:
		const char* execute(const char *str, std::size_t len, std::vector<detail::ChunkNode>& chunks ) const
		{
			std::cout << "Conservative\n";
			return nullptr;
		}
	};

	template <std::size_t ChunkSize, template <std::size_t SizeType> class StoragePolicy = GreedyStoragePolicy>
    class StringChunkManager : public StoragePolicy<ChunkSize>
    {
    public:
    	const char* store(const char *str, std::size_t len)
    	{
    		return this->execute(str, len, m_chunks);
    	}

    	const char* store(const char *str)
    	{
    		return store(str, std::strlen(str));
    	}

    	const char* store(const std::string& str)
    	{
    		return store(str.c_str(), str.size());
    	}

    private:
    	std::vector<detail::ChunkNode> m_chunks;
    };
} /* namespace gen */

#endif
