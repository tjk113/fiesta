import sys
import re

UTILITIES = ["str"]
UTILITY_FUNCTIONS = {utility: {} for utility in UTILITIES}
UTILITY_MACROS = {"str": ["STR(string)", "DSTR(string)", "STRARR(string_array)"]}

DECLARATION_PATTERN = re.compile(r"(?P<return_type>[0-9a-z_]+)\s+(?P<signature>.+);$")

HEAD_HTML = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8" />
    <title>fiesta 🎉 docs</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="styles.css" type="text/css" rel="stylesheet">
</head>
<body>
    <div class="nav-wrapper">
        <h1 class="bordered header">fiesta 🎉 docs</h1>
        <h1 id="gh-link" class="bordered header"><a href="https://github.com/tjk113/fiesta">github</a></h1>
        <nav id="nav" class="bordered">
"""
FOOT_HTML = """</body>
</html>
<script>
    let dropDowns = document.getElementsByClassName("drop-down");
    for (const elem of dropDowns) {
        elem.addEventListener("click", function () {
            this.classList.toggle("active");
            let dropDownElems = this.nextElementSibling;
            let parentElem = this.parentElement;
            if (dropDownElems.style.maxHeight != 0)
                dropDownElems.style.setProperty("max-height", null);
            else {
                let curParentHeight = parentElem.clientHeight;
                parentElem.style.setProperty("max-height", `${curParentHeight + dropDownElems.scrollHeight}px`);
                dropDownElems.style.setProperty("max-height", `${dropDownElems.scrollHeight}px`);
            }
        });
    }
    document.getElementById("nav").style.setProperty("min-height", `87%`);
</script>"""

UTILITY_CATEGORIES = {utility: [] for utility in UTILITIES}

TYPENAMES = ["dynstr", "dynstr*", "str_arr", "str_arr*", "str", "str*", "void", "char", "char*", "uint8_t", "int", "int64_t", "double"]

# Parse utility headers
for utility in UTILITIES:
    with open(f"{utility}.h", "r", encoding="utf-8") as file:
        cur_doc_strings = []
        category = None
        for line in file:
            if line.startswith('/*'):
                category = line.split(' ')[1]
                if category not in UTILITY_CATEGORIES[utility]:
                    UTILITY_CATEGORIES[utility].append(category)
            elif line.startswith("// "):
                cur_doc_strings.append(line[3:])
            elif len(cur_doc_strings) > 0:
                declaration = DECLARATION_PATTERN.match(line)
                # Skip over macros
                if not declaration:
                    cur_doc_strings = []
                    continue
                function_name = declaration.group("signature").split("(")[0]
                UTILITY_FUNCTIONS[utility][function_name] = {
                    "return_type": declaration.group("return_type"),
                    "signature": declaration.group("signature"),
                    "category": category,
                    "docs": " ".join(cur_doc_strings).replace("\n", "")
                }
                cur_doc_strings = []
                # print(UTILITY_FUNCTIONS[utility][function_name])

def write(file, string, indent):
    file.write(("    " * indent) + string)

def get_drop_down_html(name, header_level, indent):
    return f'<a class="drop-down"><{"h" + str(header_level)}>{name} <svg width="12" height="12" xmlns="http://www.w3.org/2000/svg" shape-rendering="geometricPrecision" text-rendering="geometricPrecision" image-rendering="optimizeQuality" fill-rule="evenodd" clip-rule="evenodd" viewBox="0 0 512.02 319.26"><path d="M5.9 48.96 48.97 5.89c7.86-7.86 20.73-7.84 28.56 0l178.48 178.48L434.5 5.89c7.86-7.86 20.74-7.82 28.56 0l43.07 43.07c7.83 7.84 7.83 20.72 0 28.56l-192.41 192.4-.36.37-43.07 43.07c-7.83 7.82-20.7 7.86-28.56 0l-43.07-43.07-.36-.37L5.9 77.52c-7.87-7.86-7.87-20.7 0-28.56z"/></svg></{"h" + str(header_level)}></a>' \
           + f'\n{"    " * indent}<div class="drop-down-elems">\n'

def get_header_html(text, header_level, id=None, newline=True):
    # SyntaxError: f-string expression part cannot include a backslash
    newline_char = "\n"
    id_html = f'id="{id}"'
    return f"<h{header_level}{' ' + id_html if id else ''}>" + text + f"</h{header_level}>{newline_char if newline else ''}"

def get_paragraph_html(text):
    return f"<p>{text}</p>\n"

def get_nav_link_html(function, header_level):
    return f'<a href="#{function}">{get_header_html(function, header_level, newline=False)}</a>\n'

def get_signature_html(signature):
    highlighted_signature = signature
    for typename in TYPENAMES:
        highlighted_signature = highlighted_signature.replace(f"{typename} ", f'<span class="typename">{typename}</span> ')
    return highlighted_signature

OUT_DIR = "docs"
if len(sys.argv) > 0:
    OUT_DIR = sys.argv[1]

# Write docs
with open(f"{OUT_DIR}/index.html", "w+", encoding="utf-8") as file:
    file.write(HEAD_HTML)
    # Write nav bar
    for utility in UTILITIES:
        indent = 3
        write(file, get_drop_down_html(utility, 2, indent), indent)
        for category in UTILITY_CATEGORIES[utility]:
            indent = 4
            write(file, get_drop_down_html(category, 3, indent), indent)
            indent = 5
            for function in UTILITY_FUNCTIONS[utility].keys():
                if UTILITY_FUNCTIONS[utility][function]["category"] == category:
                    write(file, get_nav_link_html(function, 4), indent)
            write(file, "</div>\n", indent - 1)
        indent = 3
        write(file, "</div>\n", indent)
    file.write("""            <div style="clear: both; margin-top: 100%;"></div>
        </nav>
    </div>
    <div class="content-body bordered">
""")
    # Write content body
    for utility in UTILITIES:
        indent = 2
        write(file, get_header_html(utility, 1), indent)
        for function in UTILITY_FUNCTIONS[utility].keys():
            return_type = UTILITY_FUNCTIONS[utility][function]["return_type"]
            signature = UTILITY_FUNCTIONS[utility][function]["signature"]
            full_signature = f"{return_type} {signature}"
            write(file, get_header_html(get_signature_html(full_signature), 2, function), indent)
            docs = UTILITY_FUNCTIONS[utility][function]["docs"]
            write(file, get_paragraph_html(docs), indent)
    write(file, "</div>\n", indent - 1)
    file.write(FOOT_HTML)