import os
import yaml

def generate_clangd_config(output_path='.clangd', compilation_database_path=None):
    """
    生成 .clangd 配置文件。

    :param output_path: 生成的 .clangd 配置文件的路径。
    :param compilation_database_path: 编译数据库路径，默认为 None。
    """
    print('生成 .clangd 配置文件...')

    # 获取当前工作目录的绝对路径
    project_root = os.path.abspath(os.getcwd())

    # 存储已处理的路径
    include_dirs = set()

    # 遍历所有 .h 文件并收集目录路径
    for root, dirs, files in os.walk(project_root):
        for file in files:
            if file.endswith('.h'):
                dir_path = os.path.abspath(root)
                # 使用规范化的路径避免重复
                include_dirs.add(os.path.normpath(dir_path))

    # 生成 .clangd 文件内容
    clangd_config = {
        'CompileFlags': {
            'Add': [f'-I{path}' for path in sorted(include_dirs)]
        },
        'Diagnostics': {
            'Suppress': ['implicit-function-declaration']
        }
    }
    
    # 仅在 compilation_database_path 不为 None 时加入 CompilationDatabase
    if compilation_database_path:
        clangd_config['CompileFlags']['CompilationDatabase'] = compilation_database_path

    # 将生成的配置写入 .clangd 文件
    with open(output_path, 'w') as f:
        yaml.dump(clangd_config, f, default_flow_style=False)

    print(f'已生成 .clangd 配置文件: {output_path}')

# 仅在直接运行时调用 generate_clangd_config
if __name__ == "__main__":
    generate_clangd_config()
