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
	using default_type = void;

	using name [[attribute]] = const char*;
	using type [[attribute(default_type)]] = typename;
	using value [[attribute(.1f)]] = float;

	enum class magic_enum
	{
		a [[.name = "magic_A", .type = int, .value = 3.14f]],
		b [[.name = "magic_B", .type = double, .value = 9.9f]],
		c [[.name = "magic_B", .type, .value = 0.1f]],
		d [[.name = "magic_C"]],
		e
	};

	template<typename Enum>
	constexpr void print_enum_with_attribute()
	{
		@meta print(std::string{"Feel the magical power of "} + __func__ + "!");

		@meta print("magic for-index-loop:");
		@meta for(decltype(@enum_count(Enum)) i = 0; i < @enum_count(Enum); ++i)
		{
			@meta std::string message{@enum_name(Enum, i)};
			@meta message += " - ";

			if constexpr(@enum_has_attribute(Enum, i, name))
			{
				@meta message += @enum_attribute(Enum, i, name);
			}
			else
			{
				@meta message += "anonymous";
			}

			@meta message += " - ";

			if constexpr(@enum_has_attribute(Enum, i, type))
			{
				// todo
				if constexpr(@type_id(@enum_attribute(Enum, i, type)) == default_type)
				{
					@meta message += @type_string(default_type) + " type";
				}
				else
				{
					// @meta message += @type_string(@enum_tattribute(Enum, i, type)) + " type";
					// todo
					@meta message += "custom type";
				}
			}
			else
			{
				@meta message += "none type";
			}

			@meta message += " - ";

			if constexpr(@enum_has_attribute(Enum, i, value))
			{
				@meta message += "value: " + std::to_string(@enum_attribute(Enum, i, value));
			}
			else
			{
				@meta message += "no value";
			}

			@meta message += '.';
			@meta print(message);
		}
	}
}

int main()
{
	enum_with_attribute::print_enum_with_attribute<enum_with_attribute::magic_enum>();
}
