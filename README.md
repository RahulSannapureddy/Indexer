# Indexer

## Web Crawler
- Explores Wikipedia using a Breadth-First Search (BFS) queue to discover new links layer by layer.
- Collects up to 1,000 unique articles to build a solid local database for the search engine.
- Reuses network connections to speed up downloads and reduce stress on the server.
- Smartly handles redirects and fragments so each unique page is only saved once.
- Automatically resumes where it left off by checking which files are already on your disk.

## Document Parser
- Cleans raw HTML pages by extracting only the main text content and removing navigation noise.
- Strips out Wikipedia citation brackets and extra whitespace to create a clean searchable index.
- Uses a local metadata log to skip files that have already been processed, saving time on every run.
- Normalizes text by converting to lowercase and removing common "stop words" for better search accuracy.
- Organizes the final data into structured text files ready for the C++ ranking engine.
