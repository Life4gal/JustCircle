#include <iostream>
#include <string>
#include <type_traits>

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
				@type_string(Enum) + "[" +
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
				@type_string(Enum) + " in function " +
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

int main()
{
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
}
