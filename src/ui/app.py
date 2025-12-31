from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app) # Allows your browser to talk to this server

# Start your C++ program once and keep it running
cpp_process = subprocess.Popen(['./src/engine_cpp/search_engine'], 
                               stdin=subprocess.PIPE, 
                               stdout=subprocess.PIPE, 
                               text=True)

@app.route('/search')
def search():
    query = request.args.get('q', '')
    if not query:
        return jsonify([])

    # 1. Send query to C++
    cpp_process.stdin.write(query + "\n")
    cpp_process.stdin.flush()

    # 2. Read lines until we hit the sentinel
    results = []
    while True:
        line = cpp_process.stdout.readline().strip()
        
        if line == "END_OF_RESULTS" or not line:
            break
            
        # Parse the ID|URL format
        if "|" in line:
            doc_id, url = line.split('|')
            results.append({"id": doc_id, "url": url})

    return jsonify(results)

if __name__ == '__main__':
    app.run(port=5000)