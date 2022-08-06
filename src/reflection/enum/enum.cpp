#include <iostream>
#include <string>

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

int main()
{
	@meta enum_with_attribute::print_enum_with_attribute<enum_with_attribute::errc>();
	enum_with_attribute::print_enum_with_attribute<enum_with_attribute::errc>();
}
