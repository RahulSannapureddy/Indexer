# Indexer

## Web Crawler
- Explores Wikipedia using a Breadth-First Search (BFS) queue to discover new links layer by layer.
- Collects up to 1,000 unique articles to build a solid local database for the search engine.
- Reuses network connections to speed up downloads and reduce stress on the server.
- Smartly handles redirects and fragments so each unique page is only saved once.
- Automatically resumes where it left off by checking which files are already on your disk.