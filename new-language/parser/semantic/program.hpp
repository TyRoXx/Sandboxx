#ifndef NEW_LANG_SEMANTIC_PROGRAM_HPP
#define NEW_LANG_SEMANTIC_PROGRAM_HPP

#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lexical_cast.hpp>

namespace nl
{
	namespace il
	{
		struct map;
		struct signature;
		struct generic_signature;

		struct integer
		{
			std::string value;
		};

		inline bool operator == (integer const &left, integer const &right)
		{
			return (left.value == right.value);
		}

		inline std::size_t hash_value(integer const &value)
		{
			return boost::hash_value(value.value);
		}

		struct string
		{
			std::string value;

			string()
			{
			}

			explicit string(std::string value)
				: value(std::move(value))
			{
			}
		};

		inline bool operator == (string const &left, string const &right)
		{
			return (left.value == right.value);
		}

		inline std::size_t hash_value(string const &value)
		{
			return boost::hash_value(value.value);
		}

#define NL_DEFINE_TRIVIAL_INLINE_STRUCT(name_) \
		struct name_ \
		{ \
		}; \
		inline bool operator == (name_ const &, name_ const &) \
		{ \
			return true; \
		} \
		inline std::size_t hash_value(name_ const &) \
		{ \
			return boost::hash_value(true); \
		}

		NL_DEFINE_TRIVIAL_INLINE_STRUCT(null)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(meta_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(string_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(integer_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(signature_type)

#undef NL_DEFINE_TRIVIAL_INLINE_STRUCT

		struct compile_time_closure;
		struct indirect_value;
		struct external;

		typedef boost::variant<
			null,
			boost::recursive_wrapper<map>,
			boost::recursive_wrapper<signature>,
			boost::recursive_wrapper<generic_signature>,
			boost::recursive_wrapper<external>,
			integer,
			string,
			meta_type,
			string_type,
			integer_type,
			signature_type,
			boost::recursive_wrapper<compile_time_closure>,
			boost::recursive_wrapper<indirect_value>
		> value;

		typedef value type;

		struct external
		{
			void const *payload;
			il::type type;
		};

		inline bool operator == (external const &left, external const &right)
		{
			return (left.payload == right.payload);
		}

		inline std::size_t hash_value(external const &value)
		{
			return boost::hash_value(value.payload);
		}

		struct indirect_value
		{
			value const *actual;
		};

		inline bool operator == (indirect_value const &left, indirect_value const &right)
		{
			return (left.actual == right.actual);
		}

		inline std::size_t hash_value(indirect_value const &value)
		{
			return hash_value(value.actual);
		}

		struct map
		{
			boost::unordered_map<value, value> elements;
		};

		inline bool operator == (map const &left, map const &right)
		{
			return (left.elements == right.elements);
		}

		inline std::size_t hash_value(map const &value)
		{
			std::size_t digest = 0;
			for (auto &entry : value.elements)
			{
				boost::hash_combine(digest, entry);
			}
			return digest;
		}

		struct signature
		{
			type result;
			std::vector<type> parameters;

			signature()
			{
			}

			signature(type result, std::vector<type> parameters)
				: result(std::move(result))
				, parameters(std::move(parameters))
			{
			}
		};

		inline bool operator == (signature const &left, signature const &right)
		{
			return
					(left.result == right.result) &&
					(left.parameters == right.parameters);
		}

		inline std::size_t hash_value(signature const &value)
		{
			std::size_t digest = 0;
			boost::hash_combine(digest, value.result);
			boost::hash_combine(digest, value.parameters);
			return digest;
		}

		struct compile_time_closure
		{
			il::type type;
			std::function<value (std::vector<value> const &)> call;
		};

		inline bool operator == (compile_time_closure const &, compile_time_closure const &)
		{
			return true;
		}

		inline std::size_t hash_value(compile_time_closure const &)
		{
			return 0;
		}

		struct constant_expression
		{
			value constant;
		};

		inline bool operator == (constant_expression const &left, constant_expression const &right)
		{
			return (left.constant == right.constant);
		}

		enum class local
		{
			bound,
			argument,
			definition,
			this_closure,
			constant
		};

		struct local_identifier
		{
			local type;
			std::size_t index;
		};

		bool operator == (local_identifier const &left, local_identifier const &right)
		{
			return
					(left.type == right.type) &&
					(left.index == right.index);
		}

		inline std::size_t hash_value(local_identifier const &value)
		{
			std::size_t digest = 0;
			boost::hash_combine(digest, value.type);
			boost::hash_combine(digest, value.index);
			return digest;
		}

		struct local_expression
		{
			local_identifier which;
			il::type type;
			std::string name;
			boost::optional<value> const_value;
		};

		struct make_closure;
		struct subscript;
		struct call;

		bool operator == (make_closure const &left, make_closure const &right);
		bool operator == (subscript const &left, subscript const &right);
		bool operator == (call const &left, call const &right);
		bool operator == (local_expression const &left, local_expression const &right);

		typedef boost::variant<
			constant_expression,
			boost::recursive_wrapper<make_closure>,
			boost::recursive_wrapper<subscript>,
			boost::recursive_wrapper<call>,
			local_expression
		> expression;

		struct name_space;

		struct generic_signature
		{
			typedef std::function<type (std::vector<expression> const &, name_space const &)> result_resolver;
			typedef std::function<bool (type const &)> type_predicate;

			result_resolver resolve;
			std::vector<type_predicate> parameters;
		};

		inline bool operator == (generic_signature const &left, generic_signature const &right)
		{
			return (left.parameters.size() == right.parameters.size()); //TODO cannot really be compared
		}

		inline std::size_t hash_value(generic_signature const &value)
		{
			std::size_t digest = 0;
			boost::hash_combine(digest, value.parameters.size()); //TODO does not really have a hash
			return digest;
		}

		struct parameter
		{
			il::type type;
			std::string name;
		};

		struct definition
		{
			std::string name;
			expression value;
		};

		struct block
		{
			std::vector<definition> definitions;
			expression result;
		};

		struct make_closure
		{
			std::vector<parameter> parameters;
			block body;
			std::vector<local_identifier> bind_from_parent;
		};

		struct subscript
		{
			expression left;
			std::string element;
		};

		struct call
		{
			expression function;
			std::vector<expression> arguments;

			call(expression function, std::vector<expression> arguments)
				: function(std::move(function))
				, arguments(std::move(arguments))
			{
//				assert(!boost::get<constant_expression>(&this->function));
			}
		};

		inline bool operator == (local_expression const &left, local_expression const &right)
		{
			return
					(left.which == right.which) &&
					(left.name == right.name) &&
					(left.type == right.type) &&
					(left.const_value == right.const_value);
		}

		inline bool operator == (block const &left, block const &right)
		{
			return
					(left.definitions == right.definitions) &&
					(left.result == right.result);
		}

		inline bool operator == (parameter const &left, parameter const &right)
		{
			return
					(left.type == right.type) &&
					(left.name == right.name);
		}

		inline bool operator == (make_closure const &left, make_closure const &right)
		{
			return
					(left.parameters == right.parameters) &&
					(left.body == right.body);
		}

		inline bool operator == (subscript const &left, subscript const &right)
		{
			return
					(left.left == right.left) &&
					(left.element == right.element);
		}

		inline bool operator == (call const &left, call const &right)
		{
			return
					(left.function == right.function) &&
					(left.arguments == right.arguments);
		}

		inline bool operator == (definition const &left, definition const &right)
		{
			return
					(left.name == right.name) &&
					(left.value == right.value);
		}

		void print(Si::sink<char> &sink, value const &v);
		void print(Si::sink<char> &sink, expression const &expr);

		struct value_printer : boost::static_visitor<>
		{
			explicit value_printer(Si::sink<char> &out)
				: m_out(out)
			{
			}

			void operator()(null) const
			{
				Si::append(m_out, "null");
			}

			void operator()(map const &value) const
			{
				Si::append(m_out, "map[");
				for (auto &elem : value.elements)
				{
					print(m_out, elem.first);
					Si::append(m_out, ": ");
					print(m_out, elem.second);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, "]");
			}

			void operator()(signature const &value) const
			{
				Si::append(m_out, "signature(");
				for (auto &param : value.parameters)
				{
					print(m_out, param);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")->");
				print(m_out, value.result);
			}

			void operator()(generic_signature const &value) const
			{
				Si::append(m_out, "generic_signature(");
				Si::append(m_out, boost::lexical_cast<std::string>(value.parameters.size()));
				Si::append(m_out, ")");
			}

			void operator()(external const &value) const
			{
				Si::append(m_out, boost::lexical_cast<std::string>(value.payload));
			}

			void operator()(integer const &value) const
			{
				Si::append(m_out, value.value);
			}

			void operator()(string const &value) const
			{
				Si::append(m_out, value.value);
			}

			void operator()(meta_type const &) const
			{
				Si::append(m_out, "meta_type");
			}

			void operator()(string_type const &) const
			{
				Si::append(m_out, "string");
			}

			void operator()(integer_type const &) const
			{
				Si::append(m_out, "integer");
			}

			void operator()(signature_type const &) const
			{
				Si::append(m_out, "signature");
			}

			void operator()(compile_time_closure const &) const
			{
				Si::append(m_out, "ctclosure");
			}

			void operator()(indirect_value const &) const
			{
				Si::append(m_out, "indirect");
			}

		private:

			Si::sink<char> &m_out;
		};

		inline void print(Si::sink<char> &sink, value const &v)
		{
			return boost::apply_visitor(value_printer{sink}, v);
		}

		inline void print(Si::sink<char> &sink, block const &b)
		{
			for (definition const &def : b.definitions)
			{
				Si::append(sink, def.name);
				Si::append(sink, " = ");
				print(sink, def.value);
				Si::append(sink, "\n");
			}
			print(sink, b.result);
		}

		inline void print(Si::sink<char> &sink, local_identifier const &id)
		{
			switch (id.type)
			{
			case local::argument: Si::append(sink, "arg:"); break;
			case local::bound: Si::append(sink, "bnd:"); break;
			case local::definition: Si::append(sink, "def:"); break;
			case local::this_closure: Si::append(sink, "this"); return;
			case local::constant: Si::append(sink, "const"); return;
			}
			Si::append(sink, boost::lexical_cast<std::string>(id.index));
		}

		struct expression_printer : boost::static_visitor<>
		{
			explicit expression_printer(Si::sink<char> &out)
				: m_out(out)
			{
			}

			void operator()(constant_expression const &expr) const
			{
				print(m_out, expr.constant);
			}

			void operator()(make_closure const &expr) const
			{
				Si::append(m_out, "(");
				for (parameter const &param : expr.parameters)
				{
					print(m_out, param.type);
					Si::append(m_out, " ");
					Si::append(m_out, param.name);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")\n");
				print(m_out, expr.body);
				Si::append(m_out, "\n");
			}

			void operator()(subscript const &expr) const
			{
				print(m_out, expr.left);
				Si::append(m_out, ".");
				Si::append(m_out, expr.element);
			}

			void operator()(call const &expr) const
			{
				print(m_out, expr.function);
				Si::append(m_out, "(");
				for (auto &argument : expr.arguments)
				{
					print(m_out, argument);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")");
			}

			void operator()(local_expression const &expr) const
			{
				Si::append(m_out, expr.name);
				Si::append(m_out, "[");
				print(m_out, expr.which);
				Si::append(m_out, "]");
				if (expr.const_value)
				{
					Si::append(m_out, "=");
					print(m_out, *expr.const_value);
				}
			}

		private:

			Si::sink<char> &m_out;
		};

		inline void print(Si::sink<char> &sink, expression const &expr)
		{
			return boost::apply_visitor(expression_printer{sink}, expr);
		}

		inline std::ostream &operator << (std::ostream &out, expression const &expr)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, expr);
			return out;
		}

		inline std::ostream &operator << (std::ostream &out, block const &b)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, b);
			return out;
		}

		inline boost::optional<signature> get_signature(type const &t)
		{
			if (auto const * const s = boost::get<signature>(&t))
			{
				return *s;
			}
			return boost::none;
		}
	}
}

#endif
