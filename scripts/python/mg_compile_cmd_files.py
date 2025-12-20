import json
import sys
import os

def merge_compile_commands(input_files, output_file):
    merged = []
    for f in input_files:
        if not os.path.exists(f):
            print(f"Warning: Compilation command file does not exist, skip: {f}")
            continue
        with open(f, 'r', encoding='utf-8') as fp:
            try:
                data = json.load(fp)
                if not isinstance(data, list):
                    print(f"Warning: {f} is not a JSON array, skip")
                    continue
            except json.JSONDecodeError:
                print(f"Error: {f} is not a valid JSON file, skip")
                continue

        for entry in data:
            if all(k in entry for k in ['directory', 'command', 'file']):
                # entry['directory'] = os.path.abspath(entry['directory'])
                # entry['file'] = os.path.abspath(entry['file'])
                merged.append(entry)
    # Deduplicate by file field
    seen = set()
    unique_merged = []
    for entry in merged:
        file_key = entry['file']
        if file_key not in seen:
            seen.add(file_key)
            unique_merged.append(entry)
    # Write merged file
    with open(output_file, 'w', encoding='utf-8') as fp:
        json.dump(unique_merged, fp, indent=2)
    print(f"Successfully merged {len(unique_merged)} compilation commands to: {output_file}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python {} <output_file> <input_file1> <input_file2> ...".format(sys.argv[0]))
        sys.exit(1)
    output_file = sys.argv[1]
    input_files = sys.argv[2:]
    merge_compile_commands(input_files, output_file)
