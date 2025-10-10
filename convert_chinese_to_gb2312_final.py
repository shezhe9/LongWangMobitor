#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
将 C 代码打印函数中的中文转换为 GB2312 编码（八进制格式）

功能：
1. 将中文字符转换为 GB2312 八进制转义序列
2. 将中文标点转换为英文标点（更安全）
3. 在中英文间自动添加空格
4. 保留原中文在行末注释

使用方法：
    python convert_chinese_to_gb2312_final.py

注意：
- 只处理打印函数（PRINT, uinfo 等）的字符串参数
- 不影响其他代码（#include, 注释等）
- 源文件应为 UTF-8 编码
"""

import re
import os

# 支持的打印函数列表
PRINT_FUNCTIONS = [
    'PRINT', 'uinfo',
    'key_uinfo', 'ad_uinfo', 'ws_uinfo', 'bump_uinfo', 'cd_uinfo', 
    'fan_uinfo', 'pwm_uinfo',
    'utrace', 'udebug', 'uwarn', 'uerror', 'ufatal',
    'key_utrace', 'key_udebug', 'key_uwarn',
    'ad_utrace', 'ad_udebug', 'ad_uwarn',
    'ws_utrace', 'ws_udebug', 'ws_uwarn',
    'bump_utrace', 'bump_udebug', 'bump_uwarn',
    'cd_utrace', 'cd_udebug', 'cd_uwarn',
    'fan_utrace', 'fan_udebug', 'fan_uwarn',
    'pwm_utrace', 'pwm_udebug', 'pwm_uwarn',
]

# 中文标点到英文标点的映射
PUNCTUATION_MAP = {
    '：': ':',  '。': '.', '，': ',',
    '！': '!',  '？': '?', '；': ';',
    '、': ',',  '（': '(', '）': ')',
    '【': '[',  '】': ']', '《': '<', '》': '>',
}

def has_chinese(text):
    """检查是否包含中文字符"""
    return bool(re.search(r'[\u4e00-\u9fff]', text))

def extract_chinese_only(text):
    """只提取中文字符（不含标点）"""
    return ''.join(re.findall(r'[\u4e00-\u9fff]', text))

def chinese_char_to_gb2312_octal(char):
    """将单个中文字符转换为 GB2312 八进制转义"""
    try:
        gb_bytes = char.encode('gb2312')
        return ''.join([f'\\{b:03o}' for b in gb_bytes])
    except:
        return char

def process_string_content(string_content):
    """
    处理字符串内容：
    1. 转换中文字符为 GB2312 八进制
    2. 转换中文标点为英文标点
    3. 在中英文间添加空格
    """
    if not has_chinese(string_content) and not any(p in string_content for p in PUNCTUATION_MAP.keys()):
        return string_content, None
    
    # 提取中文字符作为注释
    chinese_comment = extract_chinese_only(string_content)
    
    # 1. 转换中文字符
    new_string = string_content
    for char in string_content:
        if '\u4e00' <= char <= '\u9fff':
            octal_escape = chinese_char_to_gb2312_octal(char)
            new_string = new_string.replace(char, octal_escape)
    
    # 2. 转换中文标点为英文标点
    for chinese_punct, english_punct in PUNCTUATION_MAP.items():
        new_string = new_string.replace(chinese_punct, english_punct)
    
    # 3. 在中英文间添加空格
    # 八进制转义后跟英文字母/下划线
    new_string = re.sub(r'(\\[0-7]{3})([A-Za-z_])', r'\1 \2', new_string)
    # 英文字母/下划线后跟八进制转义
    new_string = re.sub(r'([A-Za-z_])(\\[0-7]{3})', r'\1 \2', new_string)
    
    return new_string, chinese_comment if chinese_comment else None

def process_line(line):
    """处理单行代码"""
    # 跳过预处理指令和纯注释行
    stripped = line.lstrip()
    if stripped.startswith('#') or stripped.startswith('//'):
        return line
    
    # 构建打印函数的匹配模式
    func_pattern = '|'.join(re.escape(f) for f in PRINT_FUNCTIONS)
    if not re.search(rf'\b({func_pattern})\s*\(', line):
        return line
    
    # 匹配字符串字面量：function("string")
    string_pattern = rf'({func_pattern})\s*\(\s*"([^"]*)"'
    match = re.search(string_pattern, line)
    
    if not match:
        return line
    
    string_content = match.group(2)
    new_string, chinese_comment = process_string_content(string_content)
    
    if new_string == string_content:
        return line  # 没有变化
    
    # 重建行
    new_line = line[:match.start(2)] + new_string + line[match.end(2):]
    
    # 添加注释（如果有中文且行尾还没有注释）
    if chinese_comment:
        comment_match = re.search(r'//.*$', new_line)
        if not comment_match:
            new_line = new_line.rstrip()
            if new_line.endswith(';'):
                new_line = new_line + f'  // {chinese_comment}'
            else:
                new_line = new_line + f';  // {chinese_comment}'
            # 恢复换行符
            if line.endswith('\n'):
                new_line += '\n'
    
    return new_line

def process_file(file_path):
    """处理单个文件"""
    # 跳过 obj 目录和编译生成的文件
    if 'obj' in file_path or file_path.endswith('.lst'):
        return False
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"ERROR reading {file_path}: {e}")
        return False
    
    new_lines = []
    modified = False
    
    for line in lines:
        new_line = process_line(line)
        if new_line != line:
            modified = True
        new_lines.append(new_line)
    
    if modified:
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.writelines(new_lines)
            print(f"[OK] Updated: {file_path}")
            return True
        except Exception as e:
            print(f"ERROR writing {file_path}: {e}")
            return False
    
    return False

def find_c_files(base_dir='.', exclude_dirs=None):
    """查找所有 C/H 源文件"""
    if exclude_dirs is None:
        exclude_dirs = ['obj', '__pycache__', '.git', 'OTA', 'PC_App', 'win_ble_hid', 'Ld', 'Startup']
    
    c_files = []
    for root, dirs, files in os.walk(base_dir):
        # 过滤排除的目录
        dirs[:] = [d for d in dirs if d not in exclude_dirs]
        
        for file in files:
            if file.endswith(('.c', '.h')):
                c_files.append(os.path.join(root, file))
    
    return c_files

def main():
    """主函数"""
    print("=" * 60)
    print("中文转 GB2312 编码工具")
    print("=" * 60)
    print()
    
    # 查找所有 C 文件
    c_files = find_c_files()
    print(f"找到 {len(c_files)} 个 C/H 文件")
    print()
    
    # 处理每个文件
    updated_count = 0
    for file_path in c_files:
        if process_file(file_path):
            updated_count += 1
    
    print()
    print("=" * 60)
    print(f"完成！共更新 {updated_count} 个文件")
    print("=" * 60)

if __name__ == '__main__':
    main()
