# WebScout Search Engine Architecture

This document provides a comprehensive technical overview of the WebScout search engine, covering its design, components, data flow, and implementation details.

---

## 1. Project Overview

WebScout is a local search engine designed to crawl, parse, and index Wikipedia articles to provide fast and relevant search results. It is built for educational and personal use, demonstrating the core principles of search engine architecture, including web crawling, document processing, inverted indexing, and information retrieval using the BM25 ranking algorithm.

### Core Problem & Approach
The project solves the challenge of finding information within a specific subset of web data (Simple Wikipedia) without relying on external search APIs. 
- **Data Acquisition**: A Python-based crawler explores Wikipedia and saves pages locally.
- **Processing**: A Python-based parser cleans HTML and prepares text for indexing.
- **Search Core**: A high-performance C++ engine builds an in-memory inverted index and ranks documents.
- **User Interface**: A Flask-based web application provides a familiar search interface.

### Technology Choices
- **Python**: Used for the crawler, parser, and web server due to its excellent libraries for networking (Requests), HTML parsing (BeautifulSoup), and web development (Flask).
- **C++**: Used for the indexing and ranking engine to ensure low-latency search responses and efficient memory management for the inverted index.
- **BM25**: Chosen as the ranking algorithm for its industry-standard effectiveness in scoring document relevance.

---

## 2. Repository Structure

```
Indexer/
├── data/                       # Central data store (ignored by git)
│   ├── raw_pages/              # Raw HTML files from crawler
│   │   ├── url_mapping.csv     # Maps hash-based filenames to original URLs
│   │   └── queue.txt           # BFS queue for crawler persistence
│   ├── processed_docs/         # Processed text data
│   │   ├── display/            # Cleaned text for UI display
│   │   ├── index/              # Tokenized/normalized text for search engine
│   │   └── metadata.csv        # Maps internal Doc IDs to URLs and filenames
│   └── stopwords.txt           # List of common words ignored during indexing
├── src/                        # Source code
│   ├── crawler/                # Data acquisition
│   │   ├── crawler.py          # Main BFS crawler script
│   │   └── migrate_data.py     # Utility to migrate/rename raw page files
│   ├── parser/                 # Document processing
│   │   └── parser.py           # HTML cleaning and tokenization script
│   ├── engine_cpp/             # Core search engine
│   │   ├── main.cpp            # Entry point for the search engine binary
│   │   ├── inverted_index.h/cpp # Inverted index data structure and logic
│   │   ├── ranking.h/cpp       # BM25 implementation and ranker logic
│   │   ├── utils.h/cpp         # Shared utilities (metadata loading, formatting)
│   │   └── test_ranking.cpp    # Unit tests for the ranking logic
│   └── ui/                     # Web interface
│       ├── app.py              # Flask server and subprocess orchestrator
│       ├── static/             # Frontend assets (CSS, JS)
│       └── templates/          # HTML templates
├── README.md                   # High-level overview
└── ARCHITECTURE.md             # This document
```

---

## 3. Architecture & Design

### Architectural Style
WebScout follows a **decoupled pipeline architecture**. The system is divided into discrete stages (Crawl -> Parse -> Index -> Search) that communicate primarily through the file system (`data/` directory). This separation allows for independent development, testing, and optimization of each component.

### Major Components
1.  **Crawler (Python)**: Fetches raw HTML.
2.  **Parser (Python)**: Transforms HTML into searchable text.
3.  **Search Engine (C++)**: The performance core; indexes text and ranks queries.
4.  **Web UI (Python/Flask)**: Orchestrates the search engine and provides the user interface.

### Data Flow
1.  **Crawl**: `crawler.py` starts from a seed URL, performs BFS, and saves HTML files to `data/raw_pages/`.
2.  **Parse**: `parser.py` reads HTML files, extracts main content, removes stopwords/punctuation, and writes to `data/processed_docs/index/`.
3.  **Index**: Upon startup, the C++ `search_engine` reads all files in `data/processed_docs/index/` and builds an in-memory `InvertedIndex`.
4.  **Search**: 
    - User enters a query in the browser.
    - Flask (`app.py`) sends the query to the C++ subprocess via `stdin`.
    - `search_engine` calculates BM25 scores and returns the top 10 results via `stdout`.
    - Flask parses the output and displays results to the user.

### Design Patterns
- **Subprocess Communication**: The Flask UI uses the `subprocess` pattern to run the C++ engine as a persistent backend, enabling language interoperability between Python and C++.
- **Inverted Index**: The core data structure used for efficient full-text search.
- **Strategy Pattern (Implicit)**: The ranking logic is encapsulated in the `Ranker` class, allowing for potential future swap-ins of different ranking algorithms.

---

## 4. Core Components — Deep Dive

### 4.1. Crawler (`src/crawler/crawler.py`)
- **Purpose**: Automate the collection of Wikipedia articles.
- **Key Features**:
    - **BFS Strategy**: Uses a queue to explore links layer-by-layer.
    - **Persistence**: Saves the queue to `queue.txt` and uses `url_mapping.csv` to resume interrupted runs.
    - **Deduplication**: Hashes URLs to create unique filenames and strips fragments.
    - **Politeness**: Includes a `time.sleep(1)` between requests and a custom User-Agent.

### 4.2. Parser (`src/parser/parser.py`)
- **Purpose**: Clean and normalize text for the search engine.
- **Key Features**:
    - **HTML Extraction**: Targeted extraction of the `#mw-content-text` div to avoid noise (sidebar, footer).
    - **Normalization**: Lowercasing, punctuation removal, and stopword filtering.
    - **Dual Output**: Generates `display` text (human-readable) and `index` text (tokenized for search).

### 4.3. Inverted Index (`src/engine_cpp/inverted_index.cpp`)
- **Structure**:
    - `vocabulary`: Maps terms (strings) to unique `TermID`s (integers).
    - `Index`: Maps `TermID` to a `vector` of `pair<DocID, Frequency>`.
- **Optimization**: Uses `std::unordered_map` for O(1) term lookups and `TermID`s to minimize string comparisons during ranking.
- **Finalization**: The index is built in a "temp" phase and then "finalized" (sorted and IDF computed) before queries are accepted.

### 4.4. Ranker (`src/engine_cpp/ranking.cpp`)
- **Algorithm**: Implements **BM25 (Best Matching 25)**.
- **Parameters**: Defaults to $k1 = 1.5$ and $b = 0.75$.
- **Scoring**: Iterates through query terms, retrieves postings from the `InvertedIndex`, and accumulates scores for all matching documents.

---

## 5. Configuration & Environment

### Environment Variables
The project does not currently use environment variables. Configuration is managed through hardcoded relative paths and default values.

### File-Based Configuration
- **Stopwords**: Located at `data/stopwords.txt`. Modifying this file affects both the Parser and the C++ Ranker.
- **BM25 Parameters**: Hardcoded in `src/engine_cpp/main.cpp` when initializing the `Ranker`.

---

## 6. Data Flow & State Management

### End-to-End Search Flow
1.  **Input**: User types "formula one" in the UI.
2.  **UI**: JavaScript `fetch()` calls `/search?q=formula+one`.
3.  **Flask**: `app.py` receives the request and writes `formula one\n` to the `cpp_process.stdin`.
4.  **C++ Engine**: 
    - Receives query.
    - Tokenizes and filters query ("formula", "one").
    - Retrieves postings for "formula" and "one".
    - Calculates BM25 score for each document.
    - Sorts documents by score.
    - Prints `doc_id|url` for the top 10 results followed by `END_OF_RESULTS`.
5.  **Flask**: Reads from `stdout` until `END_OF_RESULTS` and returns a JSON array.
6.  **Browser**: JS renders the results as links.

### State Management
- **Persistent State**: Stored on disk in the `data/` directory (HTML pages, mapping CSVs, processed text).
- **Transient State**: The `InvertedIndex` is kept in-memory within the C++ process. It is rebuilt from disk every time the C++ engine starts.

---

## 7. External Dependencies

### Python
- `requests`: High-level HTTP library for the crawler.
- `beautifulsoup4`: HTML parsing and text extraction.
- `flask`: Web framework for the UI.
- `flask-cors`: Enables Cross-Origin Resource Sharing.

### C++
- **Standard Library only**: No external dependencies like Boost or Lucene, ensuring portability and minimal build complexity.

---

## 8. Error Handling & Resilience

- **Crawler**: Wraps requests in `try-except` blocks. Handles 404s and network timeouts gracefully. Saves state on `KeyboardInterrupt`.
- **Parser**: Skips files where the content div is not found or if the file is corrupted.
- **Search Engine**:
    - Validates file paths during startup.
    - Includes a sentinel `END_OF_RESULTS` to prevent the UI from hanging on stdout reads.
    - Basic error reporting to `std::cerr`.

---

## 9. Testing Strategy

### Automated Tests
- **`src/engine_cpp/test_ranking.cpp`**: Contains unit tests for the ranking logic. It builds a small mock index and asserts that specific queries return expected document IDs.
- **Manual Verification**: The crawler and parser output can be verified by inspecting the `data/` directory.

### What is Not Tested
- Crawler network resilience under heavy load.
- UI browser compatibility.
- Integration tests for the Flask-to-C++ subprocess bridge.

---

## 10. Build, Deployment & CI/CD

### Build Process
- **C++ Compilation**: Compiled manually using `g++`.
    ```bash
    g++ -O3 src/engine_cpp/main.cpp src/engine_cpp/inverted_index.cpp src/engine_cpp/ranking.cpp src/engine_cpp/utils.cpp -o src/engine_cpp/search_engine
    ```
- **Python Setup**: Requires a virtual environment with dependencies installed via `pip`.

### Deployment
WebScout is intended for local execution. It runs as a local web server on port 5000.

---

## 11. Performance Considerations

- **C++ Engine**: The use of C++ for the index and ranking is the primary performance optimization.
- **In-Memory Index**: Avoids slow disk I/O during query time.
- **TermID Vocabulary**: Reduces string overhead.
- **Vector Pre-allocation**: The `Ranker` pre-allocates memory for tokens to minimize heap churn.
- **Negative IDF**: The current BM25 implementation uses a formula that can yield negative values for very frequent terms. This is a known trade-off for simplicity in the current version.

---

## 12. Security Considerations

- **Local-Only**: The Flask app defaults to `127.0.0.1`, preventing external access.
- **Input Validation**: The query is trimmed in JS and Python, but raw input is passed to the C++ subprocess. Since it is only used for search, the risk of "injection" is minimal, but not zero if the C++ parser were more complex.
- **Secrets**: No API keys or secrets are required or stored.

---

## 13. Known Limitations & Technical Debt

- **No Index Persistence**: The C++ engine must rebuild the index from text files on every launch. This becomes slow as the corpus grows beyond 10,000 documents.
- **Negative IDF**: Standard BM25 formula issue for very common terms.
- **Single Process**: The subprocess model is not designed for high concurrency.
- **Hardcoded Paths**: Most paths are relative to the project root, which can be fragile if the project structure changes.
- **BFS Depth**: The crawler doesn't have a explicit depth limit, only a total page limit.

---

## 14. Developer Guide

### Setup Instructions
1.  **Clone the Repository**:
    ```bash
    git clone <repo-url>
    cd Indexer
    ```
2.  **Create Virtual Environment**:
    ```bash
    python -m venv venv
    source venv/bin/activate
    pip install requests beautifulsoup4 flask flask-cors
    ```
3.  **Compile the Search Engine**:
    ```bash
    g++ -O3 src/engine_cpp/main.cpp src/engine_cpp/inverted_index.cpp src/engine_cpp/ranking.cpp src/engine_cpp/utils.cpp -o src/engine_cpp/search_engine
    ```

### Running the Project
1.  **Crawl Data**:
    ```bash
    python src/crawler/crawler.py
    ```
2.  **Parse Data**:
    ```bash
    python src/parser/parser.py
    ```
3.  **Start UI**:
    ```bash
    python src/ui/app.py
    ```
    The search interface will open automatically in your browser.
