#include <iostream>
#include <string>
#include <cstring>
#include <type_traits>
#include <algorithm>
#include <random>
#include <vector>
#include <array>

namespace
{
	constexpr void print(const std::string& message)
	{
		std::cout << message << '\n';
	}
}

namespace enum_with_attribute
{
	using full_name [[attribute]] = const char*;
	using description [[attribute]] = const char*;

	enum class [[.full_name = "error_codes", .description = "System independent error codes"]] errc
	{
		MY_OK,
		MY_EBADF [[.full_name = "bad_file_descriptor", .description = "The argument is an invalid descriptor"]],
		MY_EBADMSG [[.full_name = "bad_message", .description = "Bad message"]]
	};

	template<typename Enum>
	constexpr void print_enum_with_attribute()
	{
		print(
				std::string{"Feel the magical power of "} +
				@type_string(Enum) + "(" + std::to_string(@enum_count(Enum)) + ")[" +
				@attribute(Enum, full_name) + "](" +
				@attribute(Enum, description) + ") in function " +
				__func__ + "!");

		print("magic for-index-loop:");
		@meta for(decltype(@enum_count(Enum)) i = 0; i < @enum_count(Enum); ++i)
		{
			std::string message_fil{@enum_name(Enum, i)};
			message_fil += " - ";

			if constexpr(@enum_has_attribute(Enum, i, full_name))
			{
				message_fil += "fullname: ";
				message_fil += @enum_attribute(Enum, i, full_name);
			}
			else
			{
				message_fil += "<unknown full name>";
			}

			message_fil += " - ";

			if constexpr(@enum_has_attribute(Enum, i, description))
			{
				message_fil += "description: ";
				message_fil += @enum_attribute(Enum, i, description);
			}
			else
			{
				message_fil += "no description";
			}

			message_fil += '.';
			print(message_fil);
		}

		print("magic ranged-for-loop:");
		@meta for enum(Enum e : Enum)
		{
			// todo: cannot have the same name as the 'message' variable above, even though they don't appear to be in the same scope.
			std::string message_rfl{@enum_name(e)};
			message_rfl += @enum_name(e);
			message_rfl += " - ";

			if constexpr(@enum_has_attribute(e, full_name))
			{
				message_rfl += "fullname: ";
				message_rfl += @enum_attribute(e, full_name);
			}
			else
			{
				message_rfl += "<unknown full name>";
			}

			message_rfl += " - ";

			if constexpr(@enum_has_attribute(e, description))
			{
				message_rfl += "description: ";
				message_rfl += @enum_attribute(e, description);
			}
			else
			{
				message_rfl += "no description";
			}

			message_rfl += '.';
			print(message_rfl);
		}
	}
}

namespace enum_with_type
{
	enum typename boolean_type
	{
		bool,
	};

	enum typename char_type
	{
		char,
		wchar_t,
		char8_t,
		char16_t,
		char32_t,
	};

	enum typename char_type_but_reversed
	{
		@enum_types(char_type)...[::-1]...;
	};

	enum typename int_type
	{
		short,
		int,
		long,
		long long,
	};

	enum typename int_type_but_const
	{
		@meta for enum(int_type e: int_type)
		{
			std::add_const_t<@enum_type(e)>;
		}
	};

	enum typename integral_type
	{
		@enum_types(boolean_type)...;
		@enum_types(char_type)...;
		@enum_types(int_type)...;
	};

	enum typename floating_point_type
	{
		float,
		double,
		long double,
	};

	enum typename arithmetic_type
	{
		@enum_types(integral_type)...;
		@enum_types(floating_point_type)...;
	};

	enum typename arithmetic_but_const_lvalue_reference
	{
		@meta for enum(auto e: arithmetic_type)
		{
			// There is some problem with the name printed here, but the type is correct.
			// std::add_const<T>::type :(
			std::add_const_t<std::add_lvalue_reference_t<@enum_type(e)>>;
		}
	};

	enum typename arithmetic_but_only_floating_point_and_reversed
	{
		// long double --> -1
		// double --> -2
		// float --> -3
		@enum_types(arithmetic_type)...[:-4:-1]...;
	};

	template<typename Enum>
	constexpr void print_enum_with_type()
	{
		print(
				std::string{"Feel the magical power of "} +
				@type_string(Enum) + "(" + std::to_string(@enum_count(Enum)) + ") in function " +
				__func__ + "!");

		print(std::string{"\t-> "} + @enum_type_strings(Enum))...;
	}

	constexpr void check_const_lvalue_reference()
	{
		print("checking <" + @type_string(arithmetic_type) + "> and <" + @type_string(arithmetic_but_const_lvalue_reference) + "> ...");
		@meta for(decltype(@enum_count(arithmetic_type)) i = 0; i < @enum_count(arithmetic_type); ++i)
		{
			print(
					"checking std::add_const_t<std::add_lvalue_reference_t<" +
					@enum_type_string(arithmetic_type, i) + "> == " +
					@enum_type_string(arithmetic_but_const_lvalue_reference, i) + " ...... " +
					((std::add_const_t<std::add_lvalue_reference_t<@enum_type(arithmetic_type, i)>> == @enum_type(arithmetic_but_const_lvalue_reference, i)) ?? "passed." : "failed."));
		}
	}
}

namespace enum_with_algorithm
{
	template<typename Container>
	constexpr void shuffle(Container& container)
	{
		auto engine = std::default_random_engine{};
//		std::ranges::shuffle(container, engine);
		std::shuffle(std::begin(container), std::end(container), engine);
	}

	template<bool NeedStable = true, typename Container>
	constexpr auto unique(Container& container)
	{
		if constexpr(NeedStable)
		{
			auto begin = std::begin(container);
			auto end = begin;
			for(auto& v: container)
			{
				if(end == std::find(begin, end, v))
				{
					*end++ = std::move(v);
				}
			}
			container.resize(std::distance(begin, end));
		}
		else
		{
//			std::ranges::sort(std::begin(container), std::end(container));
			std::sort(std::begin(container), std::end(container));

//			auto range = std::ranges::unique(container);
//			container.erase(range.begin(), range.end());
			auto it = std::unique(std::begin(container), std::end(container));
			container.erase(it, std::end(container));

		}
	}

	enum typename enum_template
	{
		char,
		wchar_t,
		char8_t,
		char16_t,
		char32_t,
		char,

		int,
		int[1],
		int[2],
		int[3],
		int,

		void,
		void*,
		void**,
		void,

		float,
		double,
		long double,
	};

	enum typename enum_but_shuffled
	{
		// enum_template ==> @mtype
		// @mtype is a builtin type that encapsulates a type and has comparison/relational operators defined. You can sort or unique with it.
		@meta std::array types{@dynamic_type(@enum_types(enum_template))...};

		// shuffle it
		@meta shuffle(types);

		// expand it
		@pack_type(types)...;
	};

	enum typename enum_but_unique_stable
	{
		// elements may be removed, use vector.
		@meta std::vector types{@dynamic_type(@enum_types(enum_template))...};

		@meta unique<true>(types);

		@pack_type(types)...;
	};

	enum typename enum_but_unique_not_stable
	{
		// elements may be removed, use vector.
		@meta std::vector types{@dynamic_type(@enum_types(enum_template))...};

		@meta unique<false>(types);

		@pack_type(types)...;
	};

	enum typename enum_but_sorted
	{
		// elements will not be removed, use array.
		@meta std::array types{@dynamic_type(@enum_types(enum_template))...};

		@meta std::sort(types.begin(), types.end());

		@pack_type(types)...;
	};

	enum typename enum_but_sorted_lexicographically
	{
		// elements will not be removed, use array.
		@meta std::array types{@enum_type_strings(enum_template)...};

		// short strings will be "less than" long strings
		// lexicographical order only if the lengths are the same
		@meta std::sort(types.begin(), types.end());

		// string ==> type
		@meta for(const auto& type: types)
		{
			@type_id(type);
		}
	};

	enum typename enum_but_sorted_lexicographically_stable
	{
		// elements will not be removed, use array.
		// expand the string spellings of the types into an array, along with the index into the type.
//		@meta std::array types{std::make_pair<std::string, int>(@enum_type_strings(enum_template), int...)...};
		@meta std::array types{std::make_pair(@enum_type_strings(enum_template), int...)...};

		// Lexicographically sort the types.
//		@meta std::sort(types.begin(), types.end());
		@meta std::sort(
				   types.begin(),
				   types.end(),
				   [](const auto& lhs, const auto& rhs)
				   {
						if(const auto result = std::strcmp(lhs.first, rhs.first);
						result < 0)
						{
							return true;
						}
						else if(result == 0)
						{
							return lhs.second < rhs.second;
						}
						return false;
					});

		// get enum value from enum_template based on index
		@enum_type(enum_template, @pack_nontype(types).second)...;
	};
}

// ====================================================================
// Typed enums are complete once the closing brace is hit, and cannot be
// modified after that. If you want a dynamically modifiable set of types,
// use an std::vector<@mtype>. @pack_type on an array, std::array or
// std::vector of @mtype will yield a type parameter pack.
// Use @dynamic_type and @static_type to convert to and from @mtypes.
// ====================================================================
namespace enum_with_modify
{
	enum typename enum_but_with_star
	{
		// elements will not be removed, use array.
		@meta std::array packed_types{@dynamic_type(@enum_types(enum_with_algorithm::enum_template))...};

		@meta std::array stared_types{@dynamic_type(@pack_type(packed_types)*)...};

		// Note: Adding `*` has special meaning for array types.(int[42] ==> int(*)[42])
		@pack_type(stared_types)...;
	};

	enum typename enum_but_with_reference
	{
		// Note: void& does not exist.
		// elements will be removed, use vector.
		@meta std::vector packed_types{@dynamic_type(@enum_types(enum_with_algorithm::enum_template))...};

		@meta packed_types.erase(
			std::remove_if(
				packed_types.begin(),
				packed_types.end(),
				[](const auto& type)
				{
					return type == @dynamic_type(void);
				}
			),
			packed_types.end()
		);
		
		@meta std::array referenced_types{@dynamic_type(@pack_type(packed_types)&)...};

		// Note: Adding `&` has special meaning for array types.(int[42] ==> int(&)[42])
		@pack_type(referenced_types)...;
	};
}

namespace enum_with_tuple
{
	template<typename ... Ts>
	struct indexed_tuple
	{
		// int... is an expression that yields the index of the current element.
		Ts @(int...)...;
	};

	enum typename enum_types
	{
		// `void` cannot be instantiated as a member type
		// void,
		int,
		const char*,
		std::string,
		std::vector<std::string>,
		void (*)(int, const char*),
		std::vector<std::string>(&)(const char*, std::string&),
	};

	// Instantiate a tuple with these types.
	using indexed_tuple_t = indexed_tuple<@enum_types(enum_types)...>;

	enum class enum_names
	{
		INT,
		CONST_CHAR_STAR,
		STD_STRING,
		STD_VECTOR_STD_STRING,
		VOID_STAR_INT_CONST_CHAR_STAR,
		STD_VECTOR_STD_STRING_REFERENCE_CONST_CHAR_STAR_STD_STRING_REFERENCE,
	};

	template<typename IndexedTuple, typename ElementName>
	struct named_tuple
	{
		@enum_types(IndexedTuple) @(@enum_names(ElementName))...;
	};

	// Instantiate a tuple with these names.
	using named_tuple_t = named_tuple<enum_types, enum_names>;
}

int main()
{
	@meta print(std::string{"CXX_STANDARD_VERSION ==> "} + std::to_string(__cplusplus));

	@meta print("===========================");
	print("===========================");

	@meta enum_with_attribute::print_enum_with_attribute<enum_with_attribute::errc>();
	enum_with_attribute::print_enum_with_attribute<enum_with_attribute::errc>();

	@meta print("===========================");
	print("===========================");

	@meta enum_with_type::print_enum_with_type<enum_with_type::arithmetic_type>();
	enum_with_type::print_enum_with_type<enum_with_type::arithmetic_type>();

	@meta enum_with_type::print_enum_with_type<enum_with_type::arithmetic_but_const_lvalue_reference>();
	enum_with_type::print_enum_with_type<enum_with_type::arithmetic_but_const_lvalue_reference>();

	@meta enum_with_type::print_enum_with_type<enum_with_type::arithmetic_but_only_floating_point_and_reversed>();
	enum_with_type::print_enum_with_type<enum_with_type::arithmetic_but_only_floating_point_and_reversed>();

	@meta enum_with_type::check_const_lvalue_reference();
	enum_with_type::check_const_lvalue_reference();

	@meta print("===========================");
	print("===========================");

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_shuffled>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_shuffled>();

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_unique_stable>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_unique_stable>();

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_unique_not_stable>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_unique_not_stable>();

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted>();

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted_lexicographically>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted_lexicographically>();

	@meta enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted_lexicographically_stable>();
	enum_with_type::print_enum_with_type<enum_with_algorithm::enum_but_sorted_lexicographically_stable>();

	@meta print("===========================");
	print("===========================");

	@meta enum_with_type::print_enum_with_type<enum_with_modify::enum_but_with_star>();
	enum_with_type::print_enum_with_type<enum_with_modify::enum_but_with_star>();

	@meta enum_with_type::print_enum_with_type<enum_with_modify::enum_but_with_reference>();
	enum_with_type::print_enum_with_type<enum_with_modify::enum_but_with_reference>();

	@meta print("===========================");
	print("===========================");

	// Print the member declares of the tuple.
	@meta print("super indexed tuple:");
	@meta print("\t" + @member_decl_strings(enum_with_tuple::indexed_tuple_t))...;

	// Print the member declares of the tuple.
	@meta print("super named tuple:");
	@meta print("\t" + @member_decl_strings(enum_with_tuple::named_tuple_t))...;
}
