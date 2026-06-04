# WebScout: A Wikipedia Search Engine

WebScout is a high-performance local search engine designed to crawl, parse, and index Simple Wikipedia articles. It combines a Python-based data pipeline with a C++ search core to provide fast, relevant results using the BM25 ranking algorithm.

## 🚀 Key Features
- **Intelligent BFS Crawler**: Explores Wikipedia links layer-by-layer with persistence and polite request handling.
- **Robust Parser**: Extracts clean text from raw HTML, removing noise and normalizing tokens.
- **High-Performance C++ Core**: Uses an in-memory inverted index and BM25 ranking for sub-millisecond search latency.
- **Real-Time Benchmarking**: Tracks both index loading and query performance with microsecond precision.
- **Modern Web Interface**: A clean, Flask-powered UI for seamless searching.

## 🏗️ Architecture
WebScout follows a **decoupled pipeline architecture**:
1.  **Crawl (Python)**: Fetches raw HTML pages from Wikipedia.
2.  **Parse (Python)**: Transforms HTML into searchable text and human-readable display files.
3.  **Index (C++)**: Builds an in-memory inverted index for fast term lookup.
4.  **Rank (C++)**: Scores documents using the Best Matching 25 (BM25) algorithm.
5.  **Search (Flask/JS)**: Orchestrates the system and provides the user interface.

## 🛠️ Setup Instructions

### 1. Prerequisites
- Python 3.8+
- G++ (supporting C++17 or later)

### 2. Environment Setup
```bash
# Create and activate virtual environment
python3 -m venv venv
source venv/bin/activate

# Install Python dependencies
pip install requests beautifulsoup4 flask flask-cors
```

### 3. Compile the Search Core
```bash
g++ -O3 src/engine_cpp/main.cpp src/engine_cpp/inverted_index.cpp src/engine_cpp/ranking.cpp src/engine_cpp/utils.cpp -o src/engine_cpp/search_engine
```

## 📖 Usage Guide

### Step 1: Data Acquisition (Crawl)
The crawler explores Simple Wikipedia starting from a seed URL.
```bash
python3 src/crawler/crawler.py
```
*Note: Configured for 1,100 pages. Progress is saved automatically.*

### Step 2: Processing (Parse)
Convert the raw HTML into cleaned text files.
```bash
python3 src/parser/parser.py
```

### Step 3: Launch Search Engine
Start the web interface.
```bash
python3 src/ui/app.py
```
The interface will open at `http://127.0.0.1:5000`.

## 📈 Benchmarking
WebScout includes built-in benchmarking tools:
- **Index Build Time**: Reported in the terminal when the server starts.
- **Query Latency**: Displayed in the UI for every search (e.g., *About 10 results (0.457 milliseconds)*).

## 📁 Repository Structure
- `src/crawler/`: Wikipedia BFS crawler.
- `src/parser/`: HTML processing and tokenization.
- `src/engine_cpp/`: C++ ranking engine (BM25).
- `src/ui/`: Flask web server and frontend.
- `data/`: Local storage for raw and processed documents (ignored by Git).
