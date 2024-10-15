import os
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INC = os.path.join(ROOT_DIR, "inc")
COMPONENTS_INC = os.path.join(ROOT_DIR, "inc", "sqlcpp", "components")
COMPONENTS_SRC = os.path.join(ROOT_DIR, "src", "components")
HEAD_SUFFIX = (".h", ".hpp", ".hh")
if __name__ == "__main__":
    def handler(hpp: str, suffix: str):
        cpp = hpp[: -len(suffix)] + ".cpp"
        cpp = os.path.relpath(cpp, COMPONENTS_INC)
        cpp = os.path.join(COMPONENTS_SRC, cpp)
        if os.path.exists(cpp):
            return False
        print(f"Generating {cpp}...")
        include = os.path.relpath(hpp, INC)
        with open(cpp, "w") as f:
            f.write(f"#include \"{include}\"\n")
            f.write("namespace sqlcpp {\n\n} // namespace sqlcpp\n")

    for root, _, files in os.walk(COMPONENTS_INC):
        for file in files:
            for suffix in HEAD_SUFFIX:
                if file.endswith(suffix):
                    handler(os.path.join(root, file), suffix)
