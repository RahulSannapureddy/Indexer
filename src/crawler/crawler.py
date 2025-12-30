import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse, parse_qs
from collections import deque
import time
import os
import re
import csv

os.makedirs("data/raw_pages", exist_ok=True)
def url_to_filename(url):
    url = url.replace('https://', '').replace('http://', '')
    filename = re.sub(r'[^a-zA-Z0-9]', '_', url)
    return filename + '.html'

if not os.path.exists("data/raw_pages/url_mapping.csv"):
    with open("data/raw_pages/url_mapping.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["filename", "url"])


headers = {
    'User-Agent': 'IndexerBot/1.0 (educational purposes)'
}

seed_url = 'https://simple.wikipedia.org/wiki/Formula_One'

visited_urls = set()
seen_urls = set([seed_url])
queue = deque([seed_url])
max_pages = 1000

while queue and len(visited_urls) < max_pages:
    current_url = queue.popleft()
    if current_url in visited_urls:
        continue

    try:
        response = requests.get(current_url, headers=headers)
        response.raise_for_status()
    except requests.RequestException as e:
        print(f"Failed to retrieve {current_url}: {e}")
        continue

    filename = url_to_filename(current_url)
    filepath = os.path.join("data/raw_pages", filename)
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(response.text)
    
    with open("data/raw_pages/url_mapping.csv", "a", newline="", encoding="utf-8") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([filename, current_url])

    visited_urls.add(current_url)
    soup = BeautifulSoup(response.text, 'html.parser')

    for link in soup.find_all('a', href=True):
        href = link['href']
        full_url = urljoin(current_url, href)

        if full_url.startswith('https://simple.wikipedia.org/wiki/') and ':' not in href:
            parsed = urlparse(full_url)
            qs = parse_qs(parsed.query)
            if 'redlink' not in qs:
                if full_url not in seen_urls:
                    queue.append(full_url)
                    seen_urls.add(full_url)
    
    time.sleep(1)