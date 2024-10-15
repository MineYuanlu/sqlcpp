import re
import os
from loguru import logger


def update(filepath: str, new_macro: str, header: "list[str]") -> int:
    # 读取文件内容
    try:
        with open(filepath, 'r', encoding='utf-8') as file:
            lines = file.readlines()
    except IOError as e:
        logger.warning(f"Error opening file: {e}")
        return -1
    if len(lines) < len(header) or any(a.strip() != b.strip() for a, b in zip(lines, header)):
        lines = header + lines
    # 获取第一个和最后一个有效行的索引
    first_valid_index = get_first_valid_line(lines)
    last_valid_index = get_last_valid_line(lines)
    if first_valid_index is None:
        logger.warning("Failed locate fst line")
        return -2
    if last_valid_index is None:
        logger.warning("Failed locate lst line")
        return -3

    # 检查头部宏定义
    if not lines[first_valid_index].startswith('#ifndef'):
        logger.warning(f"Bad ifndef line: {lines[first_valid_index]}")
        return -4
    if not lines[first_valid_index + 1].startswith('#define'):
        logger.warning(f"Bad define line: {lines[first_valid_index+1]}")
        return -5
    ifndef_macro = lines[first_valid_index].strip().split()[1]
    define_macro = lines[first_valid_index + 1].strip().split()[1]
    if ifndef_macro != define_macro:
        return -6

    # 替换宏定义
    lines[first_valid_index] = f'#ifndef {new_macro}\n'
    lines[first_valid_index + 1] = f'#define {new_macro}\n'
    lines[last_valid_index] = f'#endif// {new_macro}\n'

    # 写回文件
    try:
        with open(filepath, 'w', encoding='utf-8') as file:
            file.writelines(lines)
    except IOError as e:
        logger.warning(f"Error writing file: {e}")
        return -7

    return 0


def get_first_valid_line(lines: "list[str]"):
    in_comment = False
    for i, line in enumerate(lines):
        if in_comment:
            if '*/' in line:
                in_comment = False
            continue
        if line.lstrip().startswith('//') or re.match(r'^\s*$', line):
            continue
        if line.lstrip().startswith('/*'):
            in_comment = True
            continue
        return i
    return None


def get_last_valid_line(lines):
    in_comment = False
    for i in range(len(lines) - 1, -1, -1):
        line = lines[i]
        if in_comment:
            if '/*' in line:
                in_comment = False
            continue
        if line.startswith('//') or re.match(r'^\s*$', line):
            continue
        if line.endswith('*/'):
            in_comment = True
            continue
        return i
    return None


def get_macro(filepath: str) -> str:
    return filepath.replace(os.sep, '_').replace('.', '__').upper() + '_GUARD'


def walk_inc(base_dir: str, header: str):
    # 遍历文件夹
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.lower().endswith('.h') or file.lower().endswith('.hpp'):
                full_path = os.path.join(root, file)
                relative_path = os.path.relpath(full_path, base_dir)
                macro = get_macro(relative_path)
                code = update(full_path, macro, header)
                if code:
                    logger.warning(f"Failed update: {relative_path} = {code}")
                else:
                    logger.success(f"Update: {relative_path} {macro}")


ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INC_DIR = os.path.join(ROOT_DIR, "inc")
assert os.path.isdir(INC_DIR), INC_DIR
HEADER_FILE = os.path.join(ROOT_DIR, ".snippets/header.hpp")
HEADER = open(HEADER_FILE, 'r', encoding='utf-8').readlines()

if __name__ == "__main__":
    walk_inc(INC_DIR, HEADER)
