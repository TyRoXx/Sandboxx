package taac.assembly;

import taac.assembly.representations.CharacterLiteral;
import taac.assembly.representations.FloatingPointLiteral;
import taac.assembly.representations.Identifier;
import taac.assembly.representations.IntegerLiteral;
import taac.assembly.representations.Keyword;
import taac.assembly.representations.Operator;
import taac.assembly.representations.Representation;
import taac.assembly.representations.StringLiteral;

public enum TokenType {
	S64_Equal("equal", new Operator("==")),
	S64_Unequal("unequal", new Operator("<>", "!=")),
	S64_Smaller("smaller", new Operator("<")),
	S64_SmallerEqual("smaller equal", new Operator("<=")),
	S64_Greater("greater", new Operator(">")),
	S64_GreaterEqual("greater equal", new Operator(">=")),
	S64_Add("add", new Operator("+")),
	S64_Sub("subtract", new Operator("-")),
	S64_Mul("multiply", new Operator("*")),
	S64_Div("divide", new Operator("/")),
	S64_Mod("modulo", new Operator("%")),
	S64_And("bitwise and", new Operator("&")),
	S64_Or("bitwise or", new Operator("|")),
	S64_Xor("bitwise xor", new Operator("^")),
	S64_Add_Assign("addition assignment", new Operator("+=")),
	S64_Sub_Assign("subtraction assignment", new Operator("-=")),
	S64_Mul_Assign("multiplication assignment", new Operator("*=")),
	S64_Div_Assign("division assignment", new Operator("/=")),
	S64_Mod_Assign("modulo assignment", new Operator("%=")),
	S64_And_Assign("bitwise and assignment", new Operator("&=")),
	S64_Or_Assign("bitwise or assignment", new Operator("|=")),
	S64_Xor_Assign("bitwise xor assignment", new Operator("^=")),
	S64_LShift_Assign("left shift", new Operator("<<=")),
	S64_RShift_Assign("right shift with sign adoption", new Operator(">>=")),
	U64_RShift_Assign("right shift", new Operator(">>>=")),

	F64_Add("add", new Operator("+f")),
	F64_Sub("subtract", new Operator("-f")),
	F64_Mul("multiply", new Operator("*f")),
	F64_Div("divide", new Operator("/f")),
	F64_Mod("modulo", new Operator("%f")),
	F64_Power("power", new Operator("^f")),
	F64_Add_Assign("addition assignment", new Operator("+f=")),
	F64_Sub_Assign("subtraction assignment", new Operator("-f=")),
	F64_Mul_Assign("multiplication assignment", new Operator("*f=")),
	F64_Div_Assign("division assignment", new Operator("/f=")),
	F64_Mod_Assign("modulo assignment", new Operator("%f=")),

	Assign("assignment", new Operator(":=")),
	Comma("comma", new Operator(",")),

	Key_val("keyword", new Keyword("val")),
	Key_adr("keyword", new Keyword("adr")),
	Key_con("keyword", new Keyword("con")),
	Key_if("keyword", new Keyword("if")),
	Key_then("keyword", new Keyword("then")),
	Key_goto("keyword", new Keyword("goto")),
	Key_nop("keyword", new Keyword("nop")),
	Key_stop("keyword", new Keyword("stop")),
	Key_jsr("keyword", new Keyword("jsr")),
	Key_rts("keyword", new Keyword("rts")),
	Key_jumpProc("keyword", new Keyword("jumpProc")),
	Key_returnProc("keyword", new Keyword("returnProc")),
	Key_push("keyword", new Keyword("push")),
	Key_pull("keyword", new Keyword("pull")),
	Key_get("keyword", new Keyword("get")),
	Key_getPull("keyword", new Keyword("getPull")),
	Key_put("keyword", new Keyword("put")),

	S64_Literal_Dec("decimal literal", new IntegerLiteral()),
	S64_Literal_Bin("dual literal", new IntegerLiteral()),
	S64_Literal_Oct("octal literal", new IntegerLiteral()),
	S64_Literal_Hex("hexadecimal literal", new IntegerLiteral()),
	F64_Literal("floating point literal", new FloatingPointLiteral()),
	Char_Literal("character literal", new CharacterLiteral()),
	String_Literal("string literal", new StringLiteral()),

	Identifier("identifier", new Identifier());

	public final String name;
	public final Representation representation;

	private TokenType(String name, Representation representation) {
		this.name = name;
		this.representation = representation;
	}
}
