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

## 📊 Benchmarking & Performance

Detailed performance analysis of the WebScout search engine using `hyperfine` for end-to-end latency measurement.

### Benchmarking Environment
- **Operating System:** Linux (Ubuntu/Debian recommended)
- **CPU:** High-performance multi-core processor
- **Compiler:** `g++` with `-O3` optimization
- **Tooling:** `hyperfine` (command-line benchmarking tool)
- **Dataset:** 1,100 Wikipedia articles (processed and indexed)

### Methodology
Benchmarks were conducted by piping randomized, legible queries of varying lengths into the C++ search engine. Each test included a **3-run warmup** to eliminate disk cache bias, followed by at least **20 measurement runs** per query. 

> **Note:** The total execution time reported by `hyperfine` includes both the **Index Load Time** (reading 1,100 files from disk) and the **Query Ranking Time**. For search-only latency, refer to the "milliseconds" reported in the Web UI.

### Results
| Query Category | Example Query | Mean Time (ms) | Std Dev (±ms) | Min/Max Range (ms) | Runs |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Single Word** | `"Ferrari"` | 123.7 ms | 4.1 ms | 117.8 – 133.9 | 24 |
| **Single Word** | `"Championship"` | 131.2 ms | 5.3 ms | 123.8 – 142.2 | 21 |
| **Multi-Word** | `"Grand Prix"` | 136.4 ms | 7.3 ms | 125.8 – 150.9 | 21 |
| **Multi-Word** | `"World Champion"` | 139.4 ms | 6.2 ms | 126.9 – 152.7 | 21 |
| **Long Phrase** | `"Fédération Internationale..."` | 135.5 ms | 9.3 ms | 123.1 – 164.3 | 21 |
| **Long Phrase** | `"Fastest lap record"` | 135.6 ms | 9.3 ms | 121.8 – 156.5 | 22 |

### Performance Analysis
- **Startup Overhead:** approximately **120ms** of the total time is dedicated to loading the 1,100 document index from disk.
- **Scaling:** Query length has a minimal impact on total latency, demonstrating the efficiency of the `InvertedIndex` and `BM25` implementation.
- **Consistency:** Low standard deviation (4-9ms) across all runs indicates a stable execution environment and predictable engine behavior.

## 📁 Repository Structure
- `src/crawler/`: Wikipedia BFS crawler.
- `src/parser/`: HTML processing and tokenization.
- `src/engine_cpp/`: C++ ranking engine (BM25).
- `src/ui/`: Flask web server and frontend.
- `data/`: Local storage for raw and processed documents (ignored by Git).
