import os
import csv
import hashlib
import shutil

RAW_PAGES_DIR = "data/raw_pages"
MAPPING_FILE = os.path.join(RAW_PAGES_DIR, "url_mapping.csv")

def get_hashed_filename(url):
    return hashlib.sha256(url.encode()).hexdigest() + '.html'

def migrate():
    if not os.path.exists(MAPPING_FILE):
        return

    temp_mapping = []
    changes_made = False

    with open(MAPPING_FILE, mode='r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            old_filename = row['filename']
            url = row['url']
            new_filename = get_hashed_filename(url)

            if old_filename != new_filename:
                old_path = os.path.join(RAW_PAGES_DIR, old_filename)
                new_path = os.path.join(RAW_PAGES_DIR, new_filename)

                if os.path.exists(old_path):
                    shutil.move(old_path, new_path)
                    changes_made = True
                
                row['filename'] = new_filename
            
            temp_mapping.append(row)

    if changes_made:
        with open(MAPPING_FILE, mode='w', newline='', encoding='utf-8') as f:
            writer = csv.DictWriter(f, fieldnames=["filename", "url"])
            writer.writeheader()
            writer.writerows(temp_mapping)

if __name__ == "__main__":
    migrate()
