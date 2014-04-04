#include "tree_sitter/compiler.h"

namespace tree_sitter_examples {
    using tree_sitter::Grammar;
    using namespace tree_sitter::rules;

    static rule_ptr optional(rule_ptr rule) {
        return choice({ rule, blank() });
    }

    static rule_ptr in_braces(rule_ptr rule) {
        return seq({ str("{"), rule, str("}") });
    }

    static rule_ptr comma_sep(rule_ptr element) {
        return choice({
            seq({ element, repeat(seq({ str(","), element })) }),
            blank(),
        });
    }

    extern const Grammar javascript({
        { "program", repeat(sym("statement")) },
        { "statement", choice({
            sym("statement_block"),
            sym("if_statement"),
            sym("switch_statement"),
            sym("for_statement"),
            sym("break_statement"),
            sym("var_declaration"),
            sym("expression_statement") }) },

        // Statements
        { "statement_block", in_braces(err(repeat(sym("statement")))) },
        { "for_statement", seq({
            sym("_for"),
            str("("),
            choice({
                sym("var_declaration"),
                sym("expression_statement"),
            }),
            sym("expression_statement"),
            err(sym("expression")),
            str(")"),
            sym("statement") }) },
        { "if_statement", seq({
            sym("_if"),
            str("("),
            err(sym("expression")),
            str(")"),
            sym("statement"),
            optional(seq({
                sym("_else"),
                sym("statement") })) }) },
        { "switch_statement", seq({
            sym("_switch"),
            str("("),
            err(sym("expression")),
            str(")"),
            in_braces(repeat(sym("switch_case"))) }) },
        { "switch_case", seq({
            choice({
                seq({
                    sym("_case"),
                    sym("expression"),
                }),
                sym("_default")
            }),
            str(":"),
            repeat(sym("statement")) }) },
        { "break_statement", seq({
            sym("_break"),
            sym("_terminator") }) },
        { "var_declaration", seq({
            sym("_var"),
            choice({
                sym("assignment"),
                sym("identifier") }),
            sym("_terminator") }) },
        { "expression_statement", seq({
            err(sym("expression")),
            sym("_terminator") }) },
        { "expression", choice({
            sym("function_expression"),
            sym("function_call"),
            sym("property_access"),
            sym("assignment"),
            sym("literal"),
            sym("identifier") }) },

        // Expressions
        { "assignment", seq({
            sym("identifier"),
            str("="),
            sym("expression") })},
        { "function_expression", seq({
            sym("_function"),
            optional(sym("identifier")),
            sym("formal_parameters"),
            sym("statement_block") }) },
        { "function_call", seq({
            sym("expression"),
            str("("),
            comma_sep(sym("expression")),
            str(")") }) },
        { "property_access", seq({
            sym("expression"),
            str("."),
            sym("identifier") }) },
        { "formal_parameters", seq({
            str("("),
            comma_sep(sym("identifier")),
            str(")") })},
        { "literal", choice({
            sym("object"),
            sym("array"),
            sym("string"),
            sym("number"),
            sym("true"),
            sym("false"),
            sym("null"), }) },

        // Literals
        { "object", seq({
            str("{"),
            comma_sep(err(seq({
                choice({ sym("string"), sym("identifier") }),
                str(":"),
                sym("expression") }))),
            str("}"), }) },
        { "array", seq({
            str("["),
            comma_sep(err(sym("expression"))),
            str("]") }) },

        // Keywords
        { "_break", str("break") },
        { "_case", str("case") },
        { "_default", str("default") },
        { "_else", str("else") },
        { "_for", str("for") },
        { "_function", str("function") },
        { "_if", str("if") },
        { "_switch", str("switch") },
        { "_var", str("var") },
        { "null", str("null") },
        { "true", str("true") },
        { "false", str("false") },

        { "_terminator", pattern("[;\n]") },
        { "string", pattern("\"([^\"]|\\\\\")+\"") },
        { "identifier", pattern("\\a[\\w_$]*") },
        { "number", pattern("\\d+(.\\d+)?") },
    });
}
