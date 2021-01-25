#pragma once
#include "QCat/Renderer/Buffer.h"
#include <optional>
#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Mat4) \
	X(Bool)	  \
	X(Int)

namespace QCat
{
	template<ShaderDataType type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template<> struct Map<ShaderDataType::Float>
	{
		using SysType = float;	// CPU ������ Ÿ��
		static constexpr size_t hlslSize = sizeof(SysType); //GPU ������ Ÿ���� ũ��
		static constexpr const char* code = "F1"; //���̾ƿ��� �ñ״�ó�� ��������� ���ڿ� �ڵ�
		static constexpr bool	valid = true;
	};
	template<> struct Map<ShaderDataType::Float2>
	{
		using SysType = glm::vec2;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F2";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Float3>
	{
		using SysType = glm::vec3;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F3";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Float4>
	{
		using SysType = glm::vec4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F4";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Mat4>
	{
		using SysType = glm::mat4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "M4";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Bool>
	{
		using SysType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr const char* code = "BL";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Int>
	{
		using SysType = int;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Int2>
	{
		using SysType = glm::vec2;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN2";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Int3>
	{
		using SysType = glm::vec3;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN3";
		static constexpr bool valid = true;
	};
	template<> struct Map<ShaderDataType::Int4>
	{
		using SysType = glm::vec4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "IN4";
		static constexpr bool valid = true;
	};
	// ��� Leaf type ����Ʈ�� ��Ʈ���� ������ �ֵ��� �Ѵ�.
#define X(el) static_assert(Map<ShaderDataType::el>::valid,"Missing map implementation for "#el);
	LEAF_ELEMENT_TYPES
#undef X

		template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
#define X(el) \
	template<> struct ReverseMap<typename Map<ShaderDataType::el>::SysType> \
	{ \
		static constexpr ShaderDataType type = ShaderDataType::el; \
		static constexpr bool valid = true; \
	};
	LEAF_ELEMENT_TYPES
#undef X

	struct ElementLayout
	{
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		
		ElementLayout();
		ElementLayout(ShaderDataType type, uint32_t offset = 0);

		// For Struct Member accessing
		ElementLayout& operator[](const std::string& key);
		const ElementLayout& operator[](const std::string& key) const;

		//For Array
		ElementLayout& ElementLayout::Array();
	public:
		void Add(ShaderDataType addtype, std::string name, uint32_t offset=0,uint32_t size=0);
		void Set(ShaderDataType addtype, uint32_t count);


		std::pair<uint32_t, const ElementLayout*> CalculateIndexingOffset(size_t offset, size_t index) const;
		size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		bool CrossBoundary(uint32_t offset, uint32_t size) noexcept;
		uint32_t CalculateSize(uint32_t offsetIn);
		uint32_t AdvanceBoundary(uint32_t size)const;

		uint32_t GetOffsetBegin() const;
		uint32_t GetOffsetEnd() const;
		uint32_t GetSizeInBytes() const;
		ShaderDataType GetDataType() const { return type; }
		// Leaf Ÿ���� �������� �б�/���� �������� ��ȯ�Ѵ�. w/�� ����׿��� Ÿ��üũ�̴�.
		template<typename T>
		size_t Resolve() const
		{
			switch (type)
			{
#define X(el) case ShaderDataType::el: QCAT_CORE_ASSERT(typeid(Map< ShaderDataType::el>::SysType)==typeid(T),"Type Reslove Failed! : type isnt same!"); return *offset;
				LEAF_ELEMENT_TYPES
#undef X
			default:
				QCAT_CORE_ASSERT(false, "Tried to resolve non-leaf element");
				return 0u;
			}
		}

	private:
		std::optional<uint32_t> offset;
		uint32_t size;
		ShaderDataType type = ShaderDataType::None;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		ElementRef()
			:offset(0),pLayout(nullptr),pBytes(nullptr)
		{}
		ElementRef(const ElementLayout* pLayout, char* pbytes, uint32_t offset);
		bool Exists() const noexcept;
		ElementRef operator[](const std::string& key)const;
		ElementRef operator[](size_t index) const;

		// �ش� SysType���� �б�/���⸦ ���� ��ȯ
		template<typename T>
		operator T& () const 
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			size_t elementOffset = pLayout->Resolve<T>();
			return *reinterpret_cast<T*>(pBytes + offset + elementOffset);
		}
		// �ش� SysType���� ���⸦ ���� �Ҵ���
		template<typename T>
		T& operator=(const T& rhs) const
		{
			// remove_const<T> �� T�� const���� ������ T �� �ǹ��Ѵ�
			// remove_const_t ���ŵ� T�� �����ϱ����� ����̴�. 
			// �� ReverseMap<> ���� const�� ������ T�� ���Եȴ�.
			// �����ϴ� ���Ҹ� T�� ���;��ϹǷ� ReverseMap���� ���ø� Ư��ȭ�� Systype�� ���� Ÿ���̾ƴϸ� valid�� False�� �����Եȴ�
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
		void SetData(const ElementRef& ref)
		{
			offset = ref.offset;
			pLayout = ref.pLayout;
			pBytes = ref.pBytes;
		}
		ShaderDataType GetDataType() const { return pLayout->GetDataType(); }
	private:
		uint32_t offset;
		const ElementLayout* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer()
		{
		}
		Buffer(ElementLayout& layout)
			:pLayoutRoot(std::move(layout)),data(pLayoutRoot.GetOffsetEnd())
		{
			
		}
		Buffer(ElementLayout&& layout);
		Buffer(Buffer&&) noexcept;

		ElementRef operator[](const std::string& key);
		char* GetData() noexcept;
		uint32_t GetSizeInBytes() const;

		void Copy(const Buffer&);
	private:
		ElementLayout pLayoutRoot;
		std::vector<char> data;
	};

}