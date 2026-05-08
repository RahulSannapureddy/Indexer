import os
import csv
from bs4 import BeautifulSoup
import string
import re

# Ensuring necessary directories exist
os.makedirs("data/processed_docs", exist_ok=True)
os.makedirs("data/processed_docs/display", exist_ok=True)
os.makedirs("data/processed_docs/index", exist_ok=True)

def load_stopwords(file_path):
    with open(file_path, encoding="utf-8") as f:
        stopwords = set(word.strip() for word in f if word.strip())
    return stopwords

stopwords = load_stopwords("data/stopwords.txt")

# Creating CSV file with headers
if not os.path.exists("data/processed_docs/metadata.csv"):
    with open("data/processed_docs/metadata.csv", mode="w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow(["doc_id", "filename", "url"])

processed_urls = set()
max_doc_id = -1
if os.path.exists("data/processed_docs/metadata.csv"):
    with open("data/processed_docs/metadata.csv", mode="r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            processed_urls.add(row['url'])
            if 'doc_id' in row and row['doc_id'].isdigit():
                max_doc_id = max(max_doc_id, int(row['doc_id']))

with open('data/raw_pages/url_mapping.csv', mode="r", encoding="utf-8") as mapping_file:
    reader = csv.DictReader(mapping_file)

    with open("data/processed_docs/metadata.csv", mode="a", newline="", encoding="utf-8") as metadata_file:
        fieldnames = ["doc_id", "filename", "url"]
        writer = csv.DictWriter(metadata_file, fieldnames=fieldnames)
        
        # Get the next doc_id based on the highest existing doc_id
        current_id_count = max_doc_id + 1

        for row in reader:
            filename = row['filename']
            url = row['url']

            if url in processed_urls:
                continue

            doc_id = f"{current_id_count:06d}"
            current_id_count += 1
            
            html_path = os.path.join("data/raw_pages", filename)
            if not os.path.exists(html_path):
                continue
            display_path = os.path.join("data/processed_docs/display", f"{doc_id}.txt")
            index_path = os.path.join("data/processed_docs/index", f"{doc_id}.txt")

            try:
                with open(html_path, mode="r", encoding="utf-8") as html_file:
                    soup = BeautifulSoup(html_file, "html.parser")
                
                content_div = soup.find("div", id="mw-content-text")

                if content_div is None:
                    print(f"Skipping {filename}: content not found")
                    continue
            except Exception as e:
                print(f"Error processing {filename}: {e}")
                continue
            
            paragraphs = content_div.find_all("p")

            text_content = "\n\n".join(
                p.get_text(separator=" ", strip=True)
                for p in paragraphs
                if p.get_text(strip=True)
            )

            text_content = re.sub(r"\[\d+\]", "", text_content)
            text_content = re.sub(r"[ \t]+", " ", text_content)
            text_content = re.sub(r"\n{3,}", "\n\n", text_content)
            text_content = text_content.strip()

            with open(display_path, mode="w", encoding="utf-8") as display_file:
                display_file.write(text_content)
            
            normalized_text = text_content.lower()
            normalized_text = normalized_text.replace("’", "'")
            normalized_text = re.sub(r"\b(\w+)('s|')\b", r"\1", normalized_text)
            normalized_text = normalized_text.translate(str.maketrans("", "", string.punctuation))
            normalized_text = re.sub(r'\s+', ' ', normalized_text).strip()
            tokens = normalized_text.split()
            tokens = [t for t in tokens if t not in stopwords]
            index_text = " ".join(tokens)

            with open(index_path, mode="w", encoding="utf-8") as index_file:
                index_file.write(index_text)
            
            # Update the csv mapping file
            new_row = {
                "doc_id": doc_id,
                "filename": filename,
                "url": url
            }
            
            writer.writerow(new_row)


