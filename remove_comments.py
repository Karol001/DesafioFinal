import re
import os

def remove_comments(content):
    lines = content.split('\n')
    result = []
    in_block_comment = False
    
    for line in lines:
        if in_block_comment:
            if '*/' in line:
                in_block_comment = False
                line = line.split('*/', 1)[1]
            else:
                continue
        
        if '/*' in line:
            parts = line.split('/*', 1)
            before = parts[0]
            after = parts[1]
            if '*/' in after:
                after = after.split('*/', 1)[1]
                line = before + after
            else:
                line = before
                in_block_comment = True
        
        if '//' in line:
            line = line.split('//')[0]
        
        line = line.rstrip()
        if line or not result or result[-1]:
            result.append(line)
    
    return '\n'.join(result)

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        new_content = remove_comments(content)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        print(f"Processed: {filepath}")
    except Exception as e:
        print(f"Error processing {filepath}: {e}")

def main():
    base_dir = "Juego/ProyectoFinal"
    
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith(('.cpp', '.h')):
                filepath = os.path.join(root, file)
                process_file(filepath)

if __name__ == "__main__":
    main()

