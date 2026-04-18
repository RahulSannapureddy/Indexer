# Indexer

## Web Crawler
- Crawls Simple Wikipedia articles starting from a seed URL until it hits a 1,000-page limit.
- Uses session connection pooling to speed up requests and reduce network overhead.
- Automatically handles resuming by checking existing CSV logs and local HTML files.
- Strips URL fragments and normalizes paths to prevent downloading duplicate content.
- Saves raw HTML locally and maintains a clean mapping of filenames to original URLs.