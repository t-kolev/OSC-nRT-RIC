import json
import sys
import os

def split_sarif(input_file, max_runs=20):
    with open(input_file, 'r') as f:
        sarif_data = json.load(f)

    runs = sarif_data.get('runs', [])
    total_runs = len(runs)

    if total_runs <= max_runs:
        return

    num_files = (total_runs // max_runs) + (1 if total_runs % max_runs else 0)
    base_filename, file_extension = os.path.splitext(input_file)

    for i in range(num_files):
        start_index = i * max_runs
        end_index = start_index + max_runs
        split_runs = runs[start_index:end_index]

        split_sarif_data = sarif_data.copy()
        split_sarif_data['runs'] = split_runs

        output_file = f"{base_filename}-part{i+1}{file_extension}"
        with open(output_file, 'w') as out_f:
            json.dump(split_sarif_data, out_f, indent=2)
        print(f"Created {output_file} with {len(split_runs)} runs.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python split_sarif.py <sarif-file>")
        sys.exit(1)
    
    input_sarif_file = sys.argv[1]
    split_sarif(input_sarif_file)
