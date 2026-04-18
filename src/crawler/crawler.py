import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse, parse_qs
from collections import deque
import time
import os
import hashlib
import csv

os.makedirs("data/raw_pages", exist_ok=True)

def url_to_filename(url):
    return hashlib.md5(url.encode()).hexdigest() + '.html'

# Initialize CSV if it doesn't exist
if not os.path.exists("data/raw_pages/url_mapping.csv"):
    with open("data/raw_pages/url_mapping.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["filename", "url"])

# Use a session for connection pooling
session = requests.Session()
session.headers.update({
    'User-Agent': 'IndexerBot/1.0 (educational purposes)'
})

seed_url = 'https://simple.wikipedia.org/wiki/Formula_One'.split('#')[0]
visited_urls = set()
seen_urls = {seed_url}

# Load existing progress from CSV and verify files exist
if os.path.exists("data/raw_pages/url_mapping.csv"):
    with open("data/raw_pages/url_mapping.csv", "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            filename = row['filename']
            url = row['url']
            seen_urls.add(url)
            # Only mark as visited if the actual file exists on disk
            if os.path.exists(os.path.join("data/raw_pages", filename)):
                visited_urls.add(url)

queue = deque()
if os.path.exists("data/raw_pages/queue.txt"):
    with open("data/raw_pages/queue.txt", "r", encoding="utf-8") as f:
        for line in f:
            url = line.strip()
            if url and url not in visited_urls:
                queue.append(url)
                seen_urls.add(url)

if not queue and seed_url not in visited_urls:
    queue.append(seed_url)
    seen_urls.add(seed_url)

max_pages = 1000

# Open mapping file once for efficient appending
with open("data/raw_pages/url_mapping.csv", "a", newline="", encoding="utf-8") as csvfile:
    writer = csv.writer(csvfile)

    while queue and len(visited_urls) < max_pages:
        current_url = queue.popleft()
        if current_url in visited_urls:
            continue

        try:
            response = session.get(current_url, timeout=10)
            response.raise_for_status()
            
            final_url = response.url.split('#')[0]
            if final_url in visited_urls:
                continue
                
            current_url = final_url
            seen_urls.add(current_url)
        except requests.RequestException as e:
            print(f"Failed to retrieve {current_url}: {e}")
            continue

        filename = url_to_filename(current_url)
        filepath = os.path.join("data/raw_pages", filename)
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(response.text)
        
        writer.writerow([filename, current_url])
        csvfile.flush() # Ensure data is written to disk in case of crash

        visited_urls.add(current_url)
        soup = BeautifulSoup(response.text, 'html.parser')

        for link in soup.find_all('a', href=True):
            href = link['href']
            # Strip fragments (#) to avoid downloading the same page multiple times
            full_url = urljoin(current_url, href).split('#')[0]

            if full_url.startswith('https://simple.wikipedia.org/wiki/'):
                parsed = urlparse(full_url)
                # Check for colons (namespaces) only in the actual page title
                page_title = parsed.path.split('/wiki/', 1)[-1]
                
                if ':' not in page_title:
                    qs = parse_qs(parsed.query)
                    if 'redlink' not in qs:
                        if full_url not in seen_urls:
                            queue.append(full_url)
                            seen_urls.add(full_url)
        
        with open("data/raw_pages/queue.txt", "w", encoding="utf-8") as qf:
            for url in queue:
                qf.write(url + "\n")
        
        time.sleep(1)

